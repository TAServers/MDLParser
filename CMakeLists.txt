cmake_minimum_required (VERSION 3.20)

project("MDLParser" CXX)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)

add_library(
	${PROJECT_NAME}
	"source/MDLParser.cpp"
	"source/VVDParser.cpp"
	"source/VTXParser.cpp"
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	"source"
)
