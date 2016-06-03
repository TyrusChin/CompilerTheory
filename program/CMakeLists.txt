cmake_minimum_required(VERSION 3.3)
project(program)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp src/pretreatment.cpp header/pretreatment.h header/error_process.h src/error_process.cpp src/lex.cpp header/lex.h header/recursive_descent.h src/recursive_descent.cpp header/word_code.h)
add_executable(program ${SOURCE_FILES})