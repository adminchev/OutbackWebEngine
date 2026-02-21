cmake_minimum_required(VERSION 3.10)
project(my_game_server)

set(CMAKE_CXX_STANDARD 17)

# Find the Drogon framework
find_package(Drogon REQUIRED)

# Create the executable
add_executable(my_game_server main.cpp)

# Link Drogon to our server
target_link_libraries(my_game_server PRIVATE Drogon::Drogon)
