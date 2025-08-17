#include "storage.h"
#include <sqlite3.h>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <optional>

struct Storage::Impl {
    sqlite3* db = nullptr;
};

Storage::Storage(const std::string& path) : pImpl(new Impl()) {
    if (sqlite3_open((path).c_str(), &pImpl->db) != SQLITE_OK) {
        std::string e = sqlite3_errmsg(pImpl->db);
        sqlite3_close(pImpl->db);
        throw std::runtime_error("sqlite open: " + e);
    }

    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS telemetry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            gpu_id TEXT NOT NULL,
            ts INTEGER NOT NULL,
            gpu_util REAL,
            memory_util REAL,
            temperature REAL
        );
        CREATE INDEX IF NOT EXISTS idx_gpu_ts ON telemetry (gpu_id, ts);
    )";
    char* err = nullptr;
    if (sqlite3_exec(pImpl->db, schema, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string e = err ? err : "unknown";
        if (err) sqlite3_free(err);
        sqlite3_close(pImpl->db);
        throw std::runtime_error("sqlite init: " + e);
    }
}

Storage::~Storage() {
    if (pImpl) {
        if (pImpl->db) sqlite3_close(pImpl->db);
        delete pImpl;
    }
}

void Storage::insert(const Telemetry& t) {
    const char* sql = "INSERT INTO telemetry (gpu_id, ts, gpu_util, memory_util, temperature) VALUES (?,?,?,?,?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("prepare insert");
    sqlite3_bind_text(stmt, 1, t.gpu_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, t.ts);
    sqlite3_bind_double(stmt, 3, t.gpu_util);
    sqlite3_bind_double(stmt, 4, t.memory_util);
    sqlite3_bind_double(stmt, 5, t.temperature);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("insert failed");
    }
    sqlite3_finalize(stmt);
}

std::vector<std::string> Storage::list_gpus() {
    std::vector<std::string> out;
    const char* sql = "SELECT DISTINCT gpu_id FROM telemetry;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(pImpl->db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("prepare list_gpus");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) out.emplace_back(reinterpret_cast<const char*>(text));
    }
    sqlite3_finalize(stmt);
    return out;
}

std::vector<TelemetryRow> Storage::query(const std::string& gpu, const std::optional<int64_t>& start, const std::optional<int64_t>& end) {
    std::vector<TelemetryRow> out;
    std::ostringstream oss;
    oss << "SELECT gpu_id, ts, gpu_util, memory_util, temperature FROM telemetry WHERE gpu_id = ? ";
    if (start) oss << " AND ts >= ? ";
    if (end) oss << " AND ts <= ? ";
    oss << " ORDER BY ts ASC;";
    std::string sql = oss.str();
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(pImpl->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("prepare query");
    int idx = 1;
    sqlite3_bind_text(stmt, idx++, gpu.c_str(), -1, SQLITE_TRANSIENT);
    if (start) sqlite3_bind_int64(stmt, idx++, *start);
    if (end) sqlite3_bind_int64(stmt, idx++, *end);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TelemetryRow r;
        r.gpu_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        r.ts = sqlite3_column_int64(stmt, 1);
        r.gpu_util = sqlite3_column_double(stmt, 2);
        r.memory_util = sqlite3_column_double(stmt, 3);
        r.temperature = sqlite3_column_double(stmt, 4);
        out.push_back(r);
    }
    sqlite3_finalize(stmt);
    return out;
}
