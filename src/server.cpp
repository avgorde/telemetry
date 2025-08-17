#include "telemetry.h"
#include "server.h"
#include "nlohmann/json.hpp"      // nlohmann::json (include/json.hpp)
#include "httplib.h"     // cpp-httplib header
#include <string>
#include <iostream>
#include <optional>
#include <sstream>
using json = nlohmann::json;

static json telemetry_row_to_json(const TelemetryRow& r) {
    json j;
    j["gpu_id"] = r.gpu_id;
    j["ts"] = r.ts;
    j["gpu_util"] = r.gpu_util;
    j["memory_util"] = r.memory_util;
    j["temperature"] = r.temperature;
    return j;
}

void start_server(Storage* db, int port = 8080) {
    httplib::Server svr;

    svr.Get("/api/v1/gpus", [db](const httplib::Request&, httplib::Response& res) {
        try {
            auto gpus = db->list_gpus();
            json j = gpus;
            res.set_content(j.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 500;
            json j = { {"error", e.what()} };
            res.set_content(j.dump(), "application/json");
        }
    });

    svr.Get(R"(/api/v1/gpus/(\w+)/telemetry)", [db](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        std::optional<int64_t> start, end;
        if (req.has_param("start_time")) {
            try { start = std::stoll(req.get_param_value("start_time")); } catch(...) { res.status = 400; return; }
        }
        if (req.has_param("end_time")) {
            try { end = std::stoll(req.get_param_value("end_time")); } catch(...) { res.status = 400; return; }
        }
        try {
            auto rows = db->query(id, start, end);
            json arr = json::array();
            for (auto &r : rows) arr.push_back(telemetry_row_to_json(r));
            res.set_content(arr.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 500;
            json j = { {"error", e.what()} };
            res.set_content(j.dump(), "application/json");
        }
    });

    svr.Post("/api/v1/ingest", [db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = json::parse(req.body);
            if (!j.contains("gpu_id") || !j.contains("ts")) {
                res.status = 400;
                return;
            }
            Telemetry t;
            t.gpu_id = j["gpu_id"].get<std::string>();
            t.ts = j["ts"].get<int64_t>();
            t.gpu_util = j.value("gpu_util", 0.0);
            t.memory_util = j.value("memory_util", 0.0);
            t.temperature = j.value("temperature", 0.0);
            db->insert(t);
            res.status = 200;
        } catch (const json::parse_error& e) {
            res.status = 400;
        } catch (const std::exception& e) {
            res.status = 500;
            json r = { {"error", e.what()} };
            res.set_content(r.dump(), "application/json");
        }
    });

    std::cout << "Starting server on port " << port << "\n";
    svr.listen("0.0.0.0", port);
}
