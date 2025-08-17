#pragma once
#include <string>
#include <chrono>

struct Telemetry {
    std::string gpu_id;
    int64_t ts; // epoch ms
    double gpu_util;
    double memory_util;
    double temperature;
};
