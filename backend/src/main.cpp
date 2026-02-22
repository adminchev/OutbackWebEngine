#include <drogon/drogon.h>
#include <thread>

#include "server.hpp"

int main() {
    LOG_INFO << "Starting 3D Authoritative Tick Server...";
    
    // Start the Game Engine in a separate background thread
    std::thread engineThread(Backend::gameTickLoop);
    engineThread.detach();

    // ==========================================
    // THE FIX: Teach Drogon how to serve WebAssembly
    // ==========================================
	drogon::app().setDocumentRoot("./public");
	drogon::app().registerCustomExtensionMime("wasm", "application/wasm");
	drogon::app().setFileTypes({"html", "js", "wasm"});

    // Start the Network Listener on the main thread
	drogon::app().addListener("0.0.0.0", 8080);
	drogon::app().run();
    
    return 0;
}
