cmake_minimum_required(VERSION 3.16)
project(CFGParser_master)

set(CMAKE_CXX_STANDARD 14)

include_directories(CFGParser)
include_directories(CFGParser/parser_source)

add_executable(CFGParser_master
        CFGParser/parser_source/Compiler.cpp
        CFGParser/parser_source/Compiler.h
        CFGParser/parser_source/Parser.cpp
        CFGParser/parser_source/Parser.h
        CFGParser/CFGPairs.cpp
        CFGParser/CFGPairs.h
        CFGParser/Source_test.cpp)
