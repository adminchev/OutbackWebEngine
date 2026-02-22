#include <drogon/drogon.h>
#include <json/json.h>
#include <thread>
#include <chrono>

#include "server.hpp"
#include "player.hpp"

namespace Backend {
	 std::unordered_map<drogon::WebSocketConnectionPtr, Player> players;
     std::mutex stateMutex;

	void GameServer::handleNewConnection(const drogon::HttpRequestPtr &req, const drogon::WebSocketConnectionPtr &conn) {
		std::lock_guard<std::mutex> lock(stateMutex);
		players[conn] = Player();
	}	

	void GameServer::handleNewMessage(const drogon::WebSocketConnectionPtr &conn, std::string &&message, const drogon::WebSocketMessageType &type) {
		Json::Value input;
		Json::Reader reader;
		
		if (reader.parse(message, input)) {
			std::lock_guard<std::mutex> lock(stateMutex);
			
			// 1. Apply the movement
			if (input.isMember("j") && input["j"].asBool()) players[conn].jump();
			if (input.isMember("dx") || input.isMember("dz")) players[conn].move(input["dx"].asFloat(), input["dz"].asFloat());

			// 2. THE LOGIC EXPERIMENT: Stay on the grid!
			// If X is greater than 10, set it back to 10. If less than -10, set to -10.
			// if (players[conn].x > 10.0f)  players[conn].move(10.0f, 0.0f);
			// if (players[conn].x > 10.0f)  players[conn].x = 10.0f;
			// if (players[conn].x < -10.0f) players[conn].x = -10.0f;
			//
			// if (players[conn].z > 10.0f)  players[conn].z = 10.0f;
			// if (players[conn].z < -10.0f) players[conn].z = -10.0f;
		}
	} 

	void GameServer::handleConnectionClosed(const drogon::WebSocketConnectionPtr &conn) {
		std::lock_guard<std::mutex> lock(stateMutex);
		players.erase(conn);
		LOG_INFO << "Player left. Total players: " << players.size();
	}

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
				p["x"] = player.getX();
				p["y"] = player.getY();
				p["z"] = player.getZ();
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
}
