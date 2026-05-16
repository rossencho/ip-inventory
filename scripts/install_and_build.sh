#!/usr/bin/env bash
# ============================================================
# scripts/install_and_build.sh
# Installs all dependencies on Debian 12 (bookworm) and builds
# the ip_inventory binary.
# Run as root or with sudo.
# ============================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"

echo "=== [1/5] Update package lists ==="
apt-get update -y

echo "=== [2/5] Install build tools and library dependencies ==="
apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    zlib1g-dev \
    libjsoncpp-dev \
    uuid-dev \
    libpq-dev \
    postgresql-client \
    libbrotli-dev \
    libhiredis-dev

echo "=== [3/5] Build and install Drogon from source ==="
if ! command -v drogon_ctl &>/dev/null; then
    TMP_DROGON=$(mktemp -d)
    git clone --depth 1 --recurse-submodules \
        https://github.com/drogonframework/drogon.git "$TMP_DROGON"
    cmake -S "$TMP_DROGON" -B "$TMP_DROGON/build" \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_CTL=ON
    cmake --build "$TMP_DROGON/build" --parallel "$(nproc)"
    cmake --install "$TMP_DROGON/build"
    ldconfig
    rm -rf "$TMP_DROGON"
    echo "Drogon installed."
else
    echo "Drogon already installed — skipping."
fi

echo "=== [4/5] Configure and build ip_inventory ==="
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo "=== [5/5] Done ==="
echo "Binary: ${BUILD_DIR}/ip_inventory"
echo ""
echo "Next steps:"
echo "  1. Edit config/config.json (set DB host/user/pass/dbname)"
echo "  2. Create the database:  scripts/setup_db.sh"
echo "  3. Run:                  ${BUILD_DIR}/ip_inventory config/config.json"
