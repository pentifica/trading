include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_trading "")

link_directories(test_trading
        "${PROJECT_BINARY_DIR/}../src"
)

target_link_libraries(test_trading
    PRIVATE
        GTest::GTest
        trading
)

add_subdirectory(exch)
add_subdirectory(parser)

target_include_directories(test_trading
    PUBLIC
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../src/exch"
        "${PROJECT_BINARY_DIR}/../src/parser"
)