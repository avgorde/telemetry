#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/include/curl/curl.h>   // alternative: use cpp-httplib client; but to avoid extra deps use libcurl if available
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// We'll use a minimal HTTP POST using cpp-httplib client if header is available.
// But to avoid dependency surprises, implement a simple POST using curl easy interface.
// Make sure libcurl is installed and linkable. Alternatively, you can use httplib::Client if you vendored httplib.

static int http_post_json(const std::string& url, const std::string& body) {
    CURL* curl = curl_easy_init();
    if (!curl) return -1;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? (int)http_code : -1;
}

// CSV expected: ts_ms,gpu_id,gpu_util,memory_util,temperature
int emitter_main(const std::string& csvPath, const std::string& server, int rate) {
    std::ifstream in(csvPath);
    if (!in.is_open()) {
        std::cerr << "Failed open CSV: " << csvPath << "\n";
        return 1;
    }
    std::string line;
    // header
    std::getline(in, line);
    int delay_ms = (rate > 0) ? (1000 / rate) : 100;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string ts_s, gpu, gutil_s, mem_s, temp_s;
        if (!std::getline(ss, ts_s, ',')) continue;
        if (!std::getline(ss, gpu, ',')) continue;
        if (!std::getline(ss, gutil_s, ',')) continue;
        if (!std::getline(ss, mem_s, ',')) continue;
        if (!std::getline(ss, temp_s, ',')) continue;
        json j;
        j["ts"] = std::stoll(ts_s);
        j["gpu_id"] = gpu;
        j["gpu_util"] = std::stod(gutil_s);
        j["memory_util"] = std::stod(mem_s);
        j["temperature"] = std::stod(temp_s);
        std::string url = server + "/api/v1/ingest";
        int code = http_post_json(url, j.dump());
        if (code <= 0) {
            std::cerr << "POST failed code=" << code << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    return 0;
}
