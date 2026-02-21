#include "raylib.h"
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

struct PlayerData { float x, y, z; };
std::vector<PlayerData> otherPlayers;
EMSCRIPTEN_WEBSOCKET_T ws;
Camera camera = { 0 };

// Variables for latency tracking
double lastPingTime = 0.0;
double currentPingMs = 0.0;

EM_BOOL on_message(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
    std::string data((char*)websocketEvent->data, websocketEvent->numBytes);
    
    try {
        // Parse the JSON array sent by the Drogon backend
        json stateArray = json::parse(data);
        
        otherPlayers.clear();
        for (const auto& p : stateArray) {
            otherPlayers.push_back({
                p["x"].get<float>(),
                p["y"].get<float>(),
                p["z"].get<float>()
            });
        }
    } catch (json::parse_error& e) {
        // If the server sends bad data, don't crash the browser!
        TraceLog(LOG_ERROR, "JSON Parse Error: %s", e.what());
    }

    // Calculate Ping: The time between server ticks
    double now = GetTime();
    currentPingMs = (now - lastPingTime) * 1000.0;
    lastPingTime = now;

    return EM_TRUE;
}

void setupWebSocket() {
    EmscriptenWebSocketCreateAttributes ws_attrs = {
        "ws://localhost:8080/game",
        NULL, EM_TRUE
    };
    ws = emscripten_websocket_new(&ws_attrs);
    emscripten_websocket_set_onmessage_callback(ws, NULL, on_message);
}

void sendInput(float dx, float dz) {
    std::string msg = "{\"dx\":" + std::to_string(dx) + ", \"dz\":" + std::to_string(dz) + "}";
    emscripten_websocket_send_utf8_text(ws, msg.c_str());
}

void UpdateDrawFrame(void) {
    // --- INPUT ---
    float baseSpeed = 5.0f;
    // if (IsKeyDown(KEY_W)) sendInput(0, -speed);
    // if (IsKeyDown(KEY_S)) sendInput(0, speed);
    // if (IsKeyDown(KEY_A)) sendInput(-speed, 0);
    // if (IsKeyDown(KEY_D)) sendInput(speed, 0);
	float distanceThisFrame = baseSpeed * GetFrameTime();
	float dx = 0.0f; 
	float dz = 0.0f;

	if (IsKeyDown(KEY_W)) dz -= distanceThisFrame;
    if (IsKeyDown(KEY_S)) dz += distanceThisFrame;
    if (IsKeyDown(KEY_A)) dx -= distanceThisFrame;
    if (IsKeyDown(KEY_D)) dx += distanceThisFrame;

	if (dx != 0.0f || dz != 0.0f) {
        sendInput(dx, dz);
    }

    // --- 3D RENDERING ---
    BeginDrawing();
    ClearBackground(RAYWHITE); // If it's black, this line isn't running!

    BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        for (const auto& p : otherPlayers) {
            DrawCube({p.x, p.y, p.z}, 1.0f, 1.0f, 1.0f, RED);
            DrawCubeWires({p.x, p.y, p.z}, 1.0f, 1.0f, 1.0f, MAROON);
        }
    EndMode3D();

    // --- 2D HUD (Heads Up Display) ---
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // 1. Center Crosshair
    DrawLine(screenW/2 - 10, screenH/2, screenW/2 + 10, screenH/2, DARKGRAY);
    DrawLine(screenW/2, screenH/2 - 10, screenW/2, screenH/2 + 10, DARKGRAY);

    // 2. Ping & FPS Info
    Color pingColor = (currentPingMs > 100) ? RED : DARKGREEN;
    DrawText(TextFormat("Ping: %.0f ms", currentPingMs), 10, 10, 20, pingColor);
    DrawFPS(10, 40);

    EndDrawing();
}

int main() {
    InitWindow(1024, 768, "3D Web Game");

    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    setupWebSocket();
    lastPingTime = GetTime();

    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    CloseWindow();
    return 0;
}
