find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ../include)

add_executable(storage_test storage_test.cpp ../src/storage.cpp)
target_link_libraries(storage_test PRIVATE GTest::GTest GTest::Main sqlite3 pthread)
add_test(NAME storage_test COMMAND storage_test)
