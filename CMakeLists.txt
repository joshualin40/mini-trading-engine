cmake_minimum_required(VERSION 3.15)
project(mini_trading_system LANGUAGES CXX)
add_subdirectory(external/googletest)
set(CMAKE_CXX_STANDARD 23)
add_executable(mini_trading_system
    market_data/market_data.cpp
    tests/market_data_test.cpp
    order_repository/order_repository.cpp
    tests/order_repository_test.cpp
)

target_link_libraries(mini_trading_system gtest_main)
include(GoogleTest)
gtest_discover_tests(mini_trading_system)
enable_testing()