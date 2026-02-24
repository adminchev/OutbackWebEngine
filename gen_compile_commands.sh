#!/bin/bash

# This script automates the generation of `compile_commands.json` for the frontend and backend.
# It runs CMake within a Docker container to generate the command databases,
# then merges them and corrects the file paths for local development tools.

# Exit immediately if any command fails.
set -e

# --- Pre-flight Checks & Warnings ---

# 1. Check for jq dependency
if ! command -v jq &> /dev/null
then
    echo "Error: 'jq' is not installed, but is required to merge the compile command files."
    echo "Please install jq to continue (e.g., 'sudo apt-get install jq' or 'sudo pacman -S jq')."
    exit 1
fi

# 2. Warning for language server dependencies
echo "---------------------------------------------------------------------"
echo "Note: For your IDE's language server (e.g., clangd) to work correctly,"
echo "you may need dependencies like Drogon installed on your host system."
echo "This allows your IDE to find header files for code completion and analysis."
echo "---------------------------------------------------------------------"


# --- Configuration ---
DOCKER_IMAGE="3d-fullstack-game"
PROJECT_ROOT=$(pwd)
CONTAINER_WORKDIR="/web3d"

# Use separate build directories to avoid conflicts.
BACKEND_BUILD_DIR="/tmp/build/backend"
FRONTEND_BUILD_DIR="/tmp/build/frontend"

# Temporary files for merging.
BACKEND_COMMANDS_TMP="backend_compile_commands.json"
FRONTEND_COMMANDS_TMP="frontend_compile_commands.json"
FINAL_COMMANDS="compile_commands.json"

echo "Generating compile commands for backend..."
docker run --rm -v "${PROJECT_ROOT}:${CONTAINER_WORKDIR}" -w "${CONTAINER_WORKDIR}" "${DOCKER_IMAGE}" \
  bash -c "cmake -S backend -B ${BACKEND_BUILD_DIR} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cp ${BACKEND_BUILD_DIR}/compile_commands.json ${CONTAINER_WORKDIR}/${BACKEND_COMMANDS_TMP}"

echo "Generating compile commands for frontend..."
docker run --rm -v "${PROJECT_ROOT}:${CONTAINER_WORKDIR}" -w "${CONTAINER_WORKDIR}" "${DOCKER_IMAGE}" \
  bash -c "cmake -S frontend -B ${FRONTEND_BUILD_DIR} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cp ${FRONTEND_BUILD_DIR}/compile_commands.json ${CONTAINER_WORKDIR}/${FRONTEND_COMMANDS_TMP}"

echo "Merging command files..."
jq -s '.[0] + .[1]' "${BACKEND_COMMANDS_TMP}" "${FRONTEND_COMMANDS_TMP}" > "${FINAL_COMMANDS}"

echo "Cleaning up temporary files..."
rm "${BACKEND_COMMANDS_TMP}" "${FRONTEND_COMMANDS_TMP}"

echo "Correcting file paths..."
# The `sed` command uses '|' as a separator to avoid conflicts with the '/' in file paths.
sed -i "s|${CONTAINER_WORKDIR}|${PROJECT_ROOT}|g" "${FINAL_COMMANDS}"

echo "Done. '${FINAL_COMMANDS}' has been created successfully."
