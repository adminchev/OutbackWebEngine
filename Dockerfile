# ==========================================
# STAGE 1: Build the WebAssembly Frontend
# ==========================================
FROM emscripten/emsdk:latest AS frontend-builder

# Set the workdir to the root so we can copy multiple folders
WORKDIR /project

# CRITICAL FIX: Copy the shared logic so the frontend can see it
COPY common/ ./common/
COPY frontend/ ./frontend/

# Move into the frontend folder to run the build
WORKDIR /project/frontend
RUN emcmake cmake -B build && cmake --build build

# ==========================================
# STAGE 2: Build the C++ Backend & Serve
# ==========================================
FROM drogonframework/drogon:latest

WORKDIR /project

# CRITICAL FIX: The backend also needs the shared logic to compile!
COPY common/ ./common/
COPY backend/ ./backend/

# Create the public folder inside the backend directory
RUN mkdir -p ./backend/public

# Copy our custom HTML from the HOST machine context
COPY frontend/web/index.html ./backend/public/index.html

# Copy the compiled JS and WASM files from STAGE 1
COPY --from=frontend-builder /project/frontend/build/outback_web_client.js ./backend/public/
COPY --from=frontend-builder /project/frontend/build/outback_web_client.wasm ./backend/public/

# Move into the backend folder to compile
WORKDIR /project/backend
RUN cmake -B build && cmake --build build

EXPOSE 8080

# Run the server!
CMD ["./build/outback_server"]
