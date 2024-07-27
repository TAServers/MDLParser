cmake_minimum_required(VERSION 3.20)

project("MDLParser" CXX)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)

add_library(MDLParser
        source/vvd.cpp
        source/vtx.cpp
        source/enums.hpp
        source/errors.hpp
        source/limits.hpp
        source/structs/common.hpp
        source/structs/vvd.hpp
        source/structs/vtx.hpp
        source/structs/mdl.hpp
        source/vvd.hpp
        source/vtx.hpp
        source/helpers/check-bounds.hpp
        source/helpers/offset-data-view.cpp
        source/helpers/offset-data-view.hpp
        source/mdl.cpp
        source/mdl.hpp
)

target_include_directories(
        MDLParser PRIVATE
        "source"
)
