#pragma once
#include <string>
#include <chrono>

struct Telemetry {
    std::string gpu_id;
    std::chrono::system_clock::time_point ts; // parse from CSV
    double gpu_util;
    double memory_util;
    double temperature;
    // add fields as required
};

// helper to convert time_point <-> ISO8601 string
std::string timepoint_to_iso(const std::chrono::system_clock::time_point& tp);
std::chrono::system_clock::time_point iso_to_timepoint(const std::string& iso);
