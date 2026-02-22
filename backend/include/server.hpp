#ifndef OTUBACK_BACKEND_HPP
#define OTUBACK_BACKEND_HPP

#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <unordered_map>
#include <mutex>

namespace Backend {
	// 1. DESIGN PRIMITIVE: The Entity
	struct Player {
		float x = 0.0f;
		float y = 1.0f; // 1 unit above the ground
		float z = 0.0f;
	};

	// 2. DESIGN PRIMITIVE: The Global Game State
	extern std::unordered_map<drogon::WebSocketConnectionPtr, Player> players;
	extern std::mutex stateMutex; // Protects the map from multithreading crashes


	// 3. DESIGN PRIMITIVE: The WebSocket Controller (Only handles inputs)
	class GameServer : public drogon::WebSocketController<GameServer> {
	public:
		void handleNewConnection(const drogon::HttpRequestPtr &req, const drogon::WebSocketConnectionPtr &conn);

		void handleNewMessage(const drogon::WebSocketConnectionPtr &conn, std::string &&message, const drogon::WebSocketMessageType &type);
		void handleConnectionClosed(const drogon::WebSocketConnectionPtr &conn);
		WS_PATH_LIST_BEGIN
		WS_PATH_ADD("/game", drogon::Get);
		WS_PATH_LIST_END
	};

	void gameTickLoop();
}
#endif
