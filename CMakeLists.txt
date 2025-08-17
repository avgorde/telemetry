# cmake_minimum_required(VERSION 3.16)
# project(gpu_telemetry_cpp LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# # Options
# option(BUILD_TESTS "Build unit tests" ON)
# option(ENABLE_COVERAGE "Enable coverage flags" OFF)

# # Include vendored headers
# include_directories(${PROJECT_SOURCE_DIR}/include)

# # Sources
# set(SRCS
#     src/main.cpp
#     src/server.cpp
#     src/storage.cpp
#     src/utils.cpp
#     src/emitter.cpp
# )

# # Executable
# add_executable(server ${SRCS})
# target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Dependencies
# # find_package(nlohmann_json CONFIG REQUIRED)
# # target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)

# # SQLite + Threads
# find_package(SQLite3 REQUIRED)
# find_package(Threads REQUIRED)
# target_link_libraries(server PRIVATE SQLite::SQLite3 Threads::Threads)

# # cpp-httplib option (disable OpenSSL for now)
# target_compile_definitions(server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT=0)

# # Coverage
# if (ENABLE_COVERAGE)
#   if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     target_compile_options(server PRIVATE -O0 -g --coverage)
#     target_link_libraries(server PRIVATE --coverage)
#   endif()
# endif()

# # Tests
# # if (BUILD_TESTS)
# #   enable_testing()
# #   add_subdirectory(tests)
# # endif()

cmake_minimum_required(VERSION 3.16)
project(telemetry LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_COVERAGE "Enable coverage flags" OFF)

# Include vendored headers
include_directories(${PROJECT_SOURCE_DIR}/include)

include_directories(${CURL_INCLUDE_DIR})
find_package(OpenSSL REQUIRED)


# Tell CMake where to look
set(SQLITE3_INCLUDE_DIR "sqlite/include")
set(SQLITE3_LIBRARY "${PROJECT_SOURCE_DIR}/include/sqlite3.lib")

set(CURL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/curl/include")
set(CURL_LIBRARY "${PROJECT_SOURCE_DIR}/include/curl/lib/libcurl.lib")

find_package(CURL REQUIRED)

# include_directories(${SQLITE3_INCLUDE_DIR})

# # Link with SQLite
# target_link_libraries(server PRIVATE ${SQLITE3_LIBRARY})

find_package(SQLite3 REQUIRED)




# Sources
set(SRCS
    src/main.cpp
    src/server.cpp
    src/storage.cpp
    src/utils.cpp
    src/emitter.cpp
)

add_executable(server ${SRCS})
target_compile_definitions(server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT=0)
#target_link_libraries(server PRIVATE sqlite3 pthread)
target_link_libraries(server PRIVATE OpenSSL::SSL OpenSSL::Crypto)
#target_link_libraries(server PRIVATE CURL::libcurl)
#target_link_libraries(server PRIVATE ${CURL_STATIC_LIB})

target_link_libraries(server PRIVATE ${CURL_LIBRARY})
target_link_libraries(server PRIVATE ${SQLITE3_LIBRARY})


if (ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(server PRIVATE -O0 -g --coverage)
    target_link_libraries(server PRIVATE --coverage)
  endif()
endif()

# Tests
# if (BUILD_TESTS)
#   enable_testing()
#   add_subdirectory(tests)
# endif()
