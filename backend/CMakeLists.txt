cmake_minimum_required(VERSION 3.10)
project(OutbackBackend)

# Tell CMake where the headers are
include_directories(include)
include_directories(../common/include)


set(CMAKE_CXX_STANDARD 17)

# Find the Drogon framework
find_package(Drogon REQUIRED)

# Create the executable
add_executable(outback_server 
    src/main.cpp 
    src/server.cpp
	src/player.cpp
	src/clock.cpp
)
# Link Drogon to our server
target_link_libraries(outback_server PRIVATE Drogon::Drogon)
target_include_directories(outback_server PRIVATE include)
