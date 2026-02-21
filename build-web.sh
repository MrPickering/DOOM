#!/bin/bash
set -e

PROJECT_DIR="$(pwd)"

echo "=== Installing Emscripten SDK ==="
git clone --depth 1 https://github.com/emscripten-core/emsdk.git /tmp/emsdk
cd /tmp/emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
echo "emcc version: $(emcc --version | head -1)"

echo "=== Building DOOM for WebAssembly ==="
cd "$PROJECT_DIR/linuxdoom-1.10"
emmake make -f Makefile.web

echo "=== Build output ==="
ls -la web/
