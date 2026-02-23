#ifndef OTUBACK_BACKEND_HPP
#define OTUBACK_BACKEND_HPP

#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <unordered_map>
#include <mutex>

#include "player.hpp"

namespace Backend {
	constexpr double TARGET_TICK_RATE = 0.0166;

	extern std::unordered_map<drogon::WebSocketConnectionPtr, Player> players;
	extern std::mutex stateMutex; // Protects the map from multithreading crashes

	// The WebSocket Controller (Only handles inputs)
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
