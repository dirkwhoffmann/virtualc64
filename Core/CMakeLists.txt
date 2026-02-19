cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set the project name
project(VC64Core)

# Enable testing
enable_testing()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Set the optimization level (if not specified on the command line)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
endif()

# Find and optionally link zlib
find_package(ZLIB QUIET)
if(ZLIB_FOUND)
  message(STATUS "Found ZLIB: ${ZLIB_LIBRARIES}")
else()
  message(STATUS "ZLIB not found. Compression support will be disabled.")
endif()

# Add the emulator library
add_library(VC64Core VirtualC64.cpp debug.cpp)

# Add the headless app
add_executable(VC64Headless Headless.cpp debug.cpp)
target_link_libraries(VC64Headless VC64Core)

# Specify compile options
target_compile_definitions(VC64Core PUBLIC _USE_MATH_DEFINES)
if(WIN32)
  target_link_libraries(VC64Headless ws2_32)
endif()
if(MSVC)
  target_compile_options(VC64Core PUBLIC /W4 /WX /Zc:preprocessor)
  target_compile_options(VC64Core PUBLIC /wd4100 /wd4201 /wd4324 /wd4458 /wd4996)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(VC64Core PUBLIC -Wall -Werror)
  target_compile_options(VC64Core PUBLIC -Wno-restrict)
  target_compile_options(VC64Core PUBLIC -Wno-unused-parameter)
  target_compile_options(VC64Core PUBLIC -Wno-attributes)
  target_compile_options(VC64Core PUBLIC -fconcepts)
else()
  target_compile_options(VC64Core PUBLIC -Wall -Werror)
  target_compile_options(VC64Core PUBLIC -Wno-unused-parameter)
  target_compile_options(VC64Core PUBLIC -Wno-gnu-anonymous-struct)
  target_compile_options(VC64Core PUBLIC -Wno-nested-anon-types)
endif()

# Add include paths
target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add sub directories
add_subdirectory(utlib)
add_subdirectory(Infrastructure)
add_subdirectory(Components)
add_subdirectory(Ports)
add_subdirectory(RetroVault)
add_subdirectory(Media)
add_subdirectory(Misc)
add_subdirectory(Peripherals)
add_subdirectory(ThirdParty)

# Add libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(VC64Core utlib resid RetroVault Threads::Threads)
# target_link_libraries(VC64Core lz4)
# target_link_libraries(VC64Headless lz4)

# Link zlib if available
if(ZLIB_FOUND)
  target_link_libraries(VC64Core ZLIB::ZLIB)
  target_link_libraries(VC64Headless ZLIB::ZLIB)
  target_compile_definitions(VC64Core PUBLIC USE_ZLIB=1)  # Optional define for conditional compilation
endif()

# Add tests
add_test(NAME SelfTest1 COMMAND VC64Headless --verbose --footprint)
add_test(NAME SelfTest2 COMMAND VC64Headless --verbose --smoke)
add_test(NAME SelfTest3 COMMAND VC64Headless --verbose --diagnose)
