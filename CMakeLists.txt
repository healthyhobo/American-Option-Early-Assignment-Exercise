cmake_minimum_required(VERSION 3.16)
project(AmericanOptionEarlyExerciseLab VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(option_pricer
    src/BinomialPricer.cpp
)

target_include_directories(option_pricer PUBLIC include)

add_executable(option_lab
    src/main.cpp
)

target_link_libraries(option_lab PRIVATE option_pricer)

add_executable(test_binomial
    tests/test_binomial.cpp
)

target_link_libraries(test_binomial PRIVATE option_pricer)

enable_testing()
add_test(NAME BinomialEarlyExerciseTests COMMAND test_binomial)
