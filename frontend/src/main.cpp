#include "raylib.h"
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "game_client.hpp"
using Frontend::camera;

int main() {
    InitWindow(1024, 768, "3D Web Game");

    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

	Frontend::setupWebSocket();
	Frontend::lastPingTime = GetTime();

    emscripten_set_main_loop(Frontend::UpdateDrawFrame, 0, 1);

    CloseWindow();
    return 0;
};
