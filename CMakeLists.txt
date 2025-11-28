cmake_minimum_required(VERSION 3.10)
project(sp_predictor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(predictor
	src/predictor.cpp
	src/csv_loader.cpp
	src/indicator.cpp
	src/feature_engineer.cpp
	src/linear_regression.cpp
)

add_executable(predictor_tests
	tests/predictor_tests.cpp
	src/linear_regression.cpp
	src/feature_engineer.cpp
	src/indicator.cpp
	src/csv_loader.cpp
)

enable_testing()
add_test(NAME predictor_tests COMMAND predictor_tests)
