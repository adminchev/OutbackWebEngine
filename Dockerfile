# ==========================================
# STAGE 1: Build the WebAssembly Frontend
# ==========================================
FROM emscripten/emsdk:latest AS frontend-builder

WORKDIR /project/frontend
# Copy only the frontend source code into this container
COPY frontend/ .

# Compile the Raylib C++ code into WebAssembly
RUN emcmake cmake -B build && cmake --build build

# ==========================================
# STAGE 2: Build the C++ Backend & Serve
# ==========================================
FROM drogonframework/drogon:latest

WORKDIR /project/backend
COPY backend/ .
RUN mkdir -p public

# Copy our custom, clean HTML file from the frontend source folder
COPY frontend/index.html ./public/index.html

# Copy the compiled JS and WASM files from the Emscripten builder
COPY --from=frontend-builder /project/frontend/build/my_web_client.js ./public/
COPY --from=frontend-builder /project/frontend/build/my_web_client.wasm ./public/

# Compile the Drogon Tick Server
RUN cmake -B build && cmake --build build

EXPOSE 8080
CMD ["./build/my_game_server"]
