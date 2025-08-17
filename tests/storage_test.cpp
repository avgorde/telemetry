#include <gtest/gtest.h>
#include "../src/storage.cpp"
#include <cstdio>
#include <filesystem>

TEST(StorageTest, InsertListQuery) {
    const char* path = "test_storage.db";
    std::filesystem::remove(path);
    Storage s(path);
    Telemetry t;
    t.gpu_id = "gpu-0";
    t.ts = 1000;
    t.gpu_util = 12.5;
    t.memory_util = 20.0;
    t.temperature = 60.0;
    s.insert(t);

    auto gpus = s.list_gpus();
    ASSERT_EQ(gpus.size(), 1);
    ASSERT_EQ(gpus[0], "gpu-0");

    auto rows = s.query("gpu-0", std::optional<int64_t>(), std::optional<int64_t>());
    ASSERT_EQ(rows.size(), 1);
    EXPECT_EQ(rows[0].ts, 1000);

    std::filesystem::remove(path);
}
