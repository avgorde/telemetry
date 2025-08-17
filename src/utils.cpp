#include "telemetry.h"
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

static inline std::string ts_to_iso(int64_t ms) {
    std::time_t t = ms/1000;
    std::tm tm = *std::gmtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%FT%TZ");
    return oss.str();
}
