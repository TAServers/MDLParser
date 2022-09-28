cmake_minimum_required (VERSION 3.20)

project("MDLParser" CXX)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_library(
	${PROJECT_NAME}
	"source/MDLParser.cpp"
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	"source"
)
