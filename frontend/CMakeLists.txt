cmake_minimum_required(VERSION 3.11)
project(OutbackFrontend)

# Tell CMake where the headers are
include_directories(include)
include_directories(../common/include)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Download Raylib
FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz)
set(PLATFORM "Web" CACHE STRING "Compile for WebAssembly" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# 2. Download nlohmann/json (Header-only library)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(outback_web_client 
	src/main.cpp
	src/game_client.cpp
)
set_target_properties(outback_web_client PROPERTIES SUFFIX ".js")

target_link_options(outback_web_client PRIVATE "-s" "USE_GLFW=3" "-lwebsocket.js")

# Link both Raylib and the JSON library
target_link_libraries(outback_web_client PRIVATE raylib nlohmann_json::nlohmann_json)
