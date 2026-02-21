cmake_minimum_required(VERSION 3.11)
project(my_web_client)
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

add_executable(my_web_client main.cpp)
set_target_properties(my_web_client PROPERTIES SUFFIX ".js")

target_link_options(my_web_client PRIVATE "-s" "USE_GLFW=3" "-lwebsocket.js")

# Link both Raylib and the JSON library
target_link_libraries(my_web_client PRIVATE raylib nlohmann_json::nlohmann_json)
