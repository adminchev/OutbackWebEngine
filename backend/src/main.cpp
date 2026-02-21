#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <json/json.h>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

using namespace drogon;

// 1. DESIGN PRIMITIVE: The Entity
struct Player {
    float x = 0.0f;
    float y = 1.0f; // 1 unit above the ground
    float z = 0.0f;
};

// 2. DESIGN PRIMITIVE: The Global Game State
std::unordered_map<WebSocketConnectionPtr, Player> players;
std::mutex stateMutex; // Protects the map from multithreading crashes

// 3. DESIGN PRIMITIVE: The WebSocket Controller (Only handles inputs)
class GameServer : public drogon::WebSocketController<GameServer> {
public:
    void handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &conn) override {
		std::lock_guard<std::mutex> lock(stateMutex);
		Player start;
		start.x = 0.0f; // Force Start at Center
		start.y = 1.0f;
		start.z = 0.0f; // Force Start at Center
		players[conn] = start;
	}

    void handleNewMessage(const WebSocketConnectionPtr &conn, std::string &&message, const WebSocketMessageType &type) {
		Json::Value input;
		Json::Reader reader;
		
		if (reader.parse(message, input)) {
			std::lock_guard<std::mutex> lock(stateMutex);
			
			// 1. Apply the movement
			if (input.isMember("dx")) players[conn].x += input["dx"].asFloat();
			if (input.isMember("dz")) players[conn].z += input["dz"].asFloat();

			// 2. THE LOGIC EXPERIMENT: Stay on the grid!
			// If X is greater than 10, set it back to 10. If less than -10, set to -10.
			if (players[conn].x > 10.0f)  players[conn].x = 10.0f;
			if (players[conn].x < -10.0f) players[conn].x = -10.0f;
			
			if (players[conn].z > 10.0f)  players[conn].z = 10.0f;
			if (players[conn].z < -10.0f) players[conn].z = -10.0f;
		}
} 

    void handleConnectionClosed(const WebSocketConnectionPtr &conn) override {
        std::lock_guard<std::mutex> lock(stateMutex);
        players.erase(conn);
        LOG_INFO << "Player left. Total players: " << players.size();
    }

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/game", Get);
    WS_PATH_LIST_END
};

// 4. DESIGN PRIMITIVE: The Tick Engine (Runs at 60 FPS)
void gameTickLoop() {
    while (true) {
        // Sleep for ~16 milliseconds (1000ms / 60 = 16.6ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        std::lock_guard<std::mutex> lock(stateMutex);
        
        if (players.empty()) continue; // Don't do math if the server is empty

        // 1. Calculate physics (e.g., apply gravity to all players)
        // 2. Build the state JSON
        Json::Value stateArray(Json::arrayValue);
        for (auto const& [conn, player] : players) {
            Json::Value p;
            p["x"] = player.x;
            p["y"] = player.y;
            p["z"] = player.z;
            stateArray.append(p);
        }

        Json::FastWriter writer;
        std::string stateString = writer.write(stateArray);

        // 3. Broadcast the state to everyone
        for (auto const& [conn, player] : players) {
            conn->send(stateString);
        }
    }
}

int main() {
    LOG_INFO << "Starting 3D Authoritative Tick Server...";
    
    // Start the Game Engine in a separate background thread
    std::thread engineThread(gameTickLoop);
    engineThread.detach();

    // ==========================================
    // THE FIX: Teach Drogon how to serve WebAssembly
    // ==========================================
    app().setDocumentRoot("./public");
    app().registerCustomExtensionMime("wasm", "application/wasm"); // <-- Fixed method name!
    app().setFileTypes({"html", "js", "wasm"});               

    // Start the Network Listener on the main thread
    app().addListener("0.0.0.0", 8080);
    app().run();
    
    return 0;
}
