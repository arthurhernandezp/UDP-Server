cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME server)

project(${PROJECT_NAME}  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
find_library(NSL_LIBRARY nsl)

set(SOURCES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/UDP/Server.cpp
	${SRC_DIR}/UDP/MessageParser.cpp
)

add_subdirectory(external)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)