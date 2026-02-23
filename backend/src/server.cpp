#include <drogon/drogon.h>
#include <json/json.h>
#include <thread>
#include <chrono>

#include "server.hpp"
#include "player.hpp"
#include "clock.hpp"

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
		}
	} 

	void GameServer::handleConnectionClosed(const drogon::WebSocketConnectionPtr &conn) {
		std::lock_guard<std::mutex> lock(stateMutex);
		players.erase(conn);
		LOG_INFO << "Player left. Total players: " << players.size();
	}

	// The Tick Engine (Runs at 60 FPS)
	void gameTickLoop() {
		Clock serverTimer;
		while (true) {
			double td = serverTimer.getTimeDelta();
			serverTimer.restart();

			// try to sleep for exactly 16 milliseconds (16ms - td), td is how long it took to execute the code
		    double sleep = TARGET_TICK_RATE - td;
			if (sleep < 0) sleep = 0;
			std::this_thread::sleep_for(std::chrono::duration<double>(sleep));

			std::lock_guard<std::mutex> lock(stateMutex);
			
			if (!players.empty()) {// Don't do math if the server is empty

				// Build the state JSON and calculate physics (e.g., apply gravity to all players) 
				Json::Value stateArray(Json::arrayValue);
				for (auto & [conn, player] : players) {
					Json::Value p;
					p["x"] = player.getX();
					p["y"] = player.getY();
					p["z"] = player.getZ();
					stateArray.append(p);
					player.updatePhysics(td);
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
}
