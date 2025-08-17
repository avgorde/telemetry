#include "storage.h"
#include "server.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>

// main supports two modes: server (default) and emitter via --mode emitter
int main(int argc, char** argv) {
    std::string mode = "server";
    std::string dbpath = "telemetry.db";
    std::string csvpath = "testdata/dcgm_metrics_20250718_134233.csv";
    std::string server_url = "http://localhost:8080";
    int rate = 10;
    int port = 8080;
    std::cout <<std::flush<<"Starting telemetry application...\n";
    for (int i=1;i<argc;i++) {
        if (strcmp(argv[i], "--mode") == 0 && i+1<argc) { mode = argv[++i]; }
        else if (strcmp(argv[i], "--db") == 0 && i+1<argc) { dbpath = argv[++i]; }
        else if (strcmp(argv[i], "--csv") == 0 && i+1<argc) { csvpath = argv[++i]; }
        else if (strcmp(argv[i], "--rate") == 0 && i+1<argc) { rate = atoi(argv[++i]); }
        else if (strcmp(argv[i], "--server") == 0 && i+1<argc) { server_url = argv[++i]; }
        else if (strcmp(argv[i], "--port") == 0 && i+1<argc) { port = atoi(argv[++i]); }
    }

    if (mode == "emitter") {
        // call emitter main in same binary
        std::cout<<"I am inside emitter mode\n";
        extern int emitter_main(const std::string&, const std::string&, int);
        return emitter_main(csvpath, server_url, rate);
    }

    Storage db(dbpath);
    start_server(&db, port);
    return 0;
}
