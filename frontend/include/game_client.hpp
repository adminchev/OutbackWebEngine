#ifndef OUTBACK_FRONTEND
#define OUTBACK_FRONTEND

#include <vector>
#include "raylib.h"
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

namespace Frontend {
	struct PlayerData { float x, y, z; };
	extern std::vector<PlayerData> otherPlayers;
	extern EMSCRIPTEN_WEBSOCKET_T ws;
	extern Camera camera;

	extern double lastPingTime;
	extern double currentPingMs;

	EM_BOOL on_message(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);
	void setupWebSocket();
	void sendInput(float dx, float dz);
	void UpdateDrawFrame(void);
}

#endif
