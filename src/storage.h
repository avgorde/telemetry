#pragma once
#include <string>
#include <vector>
#include "telemetry.h"
#include <optional>

struct TelemetryRow {
    std::string gpu_id;
    int64_t ts;
    double gpu_util;
    double memory_util;
    double temperature;
};

class Storage {
public:
    Storage(const std::string& path);
    ~Storage();

    void insert(const Telemetry& t);
    std::vector<std::string> list_gpus();
    std::vector<TelemetryRow> query(const std::string& gpu, const std::optional<int64_t>& start, const std::optional<int64_t>& end);
private:
    struct Impl;
    Impl* pImpl;
};
