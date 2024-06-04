cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set the project name
project(vc64Core)

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

# Add the emulator library
add_library(vc64Core VirtualC64.cpp config.cpp)

# Add the console app (VirtualC64 Headless)
add_executable(vc64Console Headless.cpp config.cpp)
target_link_libraries(vc64Console vc64Core)

# Specify compile options
target_compile_definitions(vc64Core PUBLIC _USE_MATH_DEFINES)
if(MSVC)
  target_compile_options(vc64Core PUBLIC /W4 /WX /Zc:preprocessor)
  target_compile_options(vc64Core PUBLIC /wd4100 /wd4201 /wd4324 /wd4458 /wd4996)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(vc64Core PUBLIC -Wall -Werror)
  target_compile_options(vc64Core PUBLIC -Wno-restrict)
  target_compile_options(vc64Core PUBLIC -Wno-unused-parameter)
  target_compile_options(vc64Core PUBLIC -Wno-attributes)
  target_compile_options(vc64Core PUBLIC -fconcepts)
else()
  target_compile_options(vc64Core PUBLIC -Wall -Werror)
  target_compile_options(vc64Core PUBLIC -Wno-unused-parameter)
  target_compile_options(vc64Core PUBLIC -Wno-gnu-anonymous-struct)
  target_compile_options(vc64Core PUBLIC -Wno-nested-anon-types)
endif()

# Add include paths
target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add sub directories
add_subdirectory(Base)
add_subdirectory(Components)
add_subdirectory(FileSystems)
add_subdirectory(Media)
add_subdirectory(Misc)
add_subdirectory(Peripherals)
add_subdirectory(Utilities)

# Add libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(vc64Core resid Threads::Threads)

# Add tests
add_test(NAME SelfTest COMMAND vc64Console --selftest)
