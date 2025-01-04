cmake_minimum_required(VERSION 3.18)
project(tpm_cpp)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

# Detect platform
if (WIN32)
    set(PLATFORM_WINDOWS ON)
    add_compile_definitions(PLATFORM_WINDOWS)
    link_libraries(kernel32.lib user32.lib tbs.lib)
    set(CMAKE_CXX_FLAGS "/utf-8")
    message(STATUS "Detected Windows platform")
elseif (UNIX AND NOT APPLE)
    add_compile_definitions(PLATFORM_LINUX)
    set(PLATFORM_LINUX ON)
    message(STATUS "Detected Linux platform")
else ()
    message(FATAL_ERROR "Unsupported platform '${CMAKE_SYSTEM_NAME}'")
endif ()

# Library
file(GLOB_RECURSE TPM_CPP_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")
#add_library(tpm-cpp STATIC ${TPM_CPP_SOURCE_FILES})
add_executable(tpm-cpp ${TPM_CPP_SOURCE_FILES})
target_include_directories(tpm-cpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Fetch fmt
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        GIT_PROGRESS true
)
FetchContent_MakeAvailable(fmt)
add_compile_definitions(FMT_HEADER_ONLY)
target_include_directories(tpm-cpp PRIVATE "${CMAKE_BINARY_DIR}/_deps/fmt-src/include")
