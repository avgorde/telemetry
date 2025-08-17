# telemetry data pipeline
# GPU Telemetry Pipeline (C++)
This repo implements a minimal GPU telemetry pipeline in C++:
- Emitter (reads CSV, POSTs telemetry)
- Server (REST API: list GPUs, query telemetry)
- SQLite persistence
- Dockerfiles and Helm chart
- Unit tests using GoogleTest
- Static OpenAPI spec
## Build on  (Windows)
cd build
cmake ..
cmake --build .

## Prereqs (Linux / WSL / Windows)
- CMake >= 3.16
- A C++ compiler (g++/clang on Linux, MSVC on Windows)
- sqlite3 development library
- (Optional) docker, helm for packaging/deploy

On Ubuntu:
```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev lcov
