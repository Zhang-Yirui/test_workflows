cmake_minimum_required(VERSION 3.22)
project(C_Calendar_t C)

set(CMAKE_C_STANDARD 23)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fexec-charset=GBK")
ENDIF()

add_compile_options(-O3)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB INCLUDES ${PROJECT_SOURCE_DIR}/inc/*.h)
include_directories("${PROJECT_SOURCE_DIR}/inc")

add_executable(calendar main.c main.h ${SOURCES} ${INCLUDES})