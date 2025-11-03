cmake_minimum_required(VERSION 3.10)
project(simple_strategy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(strategy
	src/main.cpp
	src/csv_loader.cpp
	src/indicator.cpp
	src/strategy.cpp
	src/backtester.cpp
	src/metrics.cpp
)

add_executable(indicator_tests
	tests/indicator_tests.cpp
	src/indicator.cpp
)

enable_testing()
add_test(NAME indicator_tests COMMAND indicator_tests)
