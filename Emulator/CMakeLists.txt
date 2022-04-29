cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set the project name
project(vc64Core)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Set the optimization level (if not specified on the command line)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
endif()

# Add emulator library
add_library(vc64Core C64.cpp)

# Add the console app (VirtualC64 Headless)
if(NOT WIN32)
# add_executable(vc64Console Headless.cpp)
# target_link_libraries(vc64Console vc64Core)
endif()

# Specify compile options
target_compile_definitions(vc64Core PUBLIC _USE_MATH_DEFINES)
if(MSVC)
  target_compile_options(vc64Core PUBLIC /W4 /WX)
  target_compile_options(vc64Core PUBLIC /wd4100 /wd4201 /wd4324 /wd4458)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(vc64Core PUBLIC -Wall -Werror)
  target_compile_options(vc64Core PUBLIC -Wno-unused-parameter)
  target_compile_options(vc64Core PUBLIC -fconcepts)
else()
  target_compile_options(vc64Core PUBLIC -Wall -Werror)
  target_compile_options(vc64Core PUBLIC -Wno-unused-parameter)
endif()

# Add include paths
target_include_directories(vc64Core PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/.
${CMAKE_CURRENT_SOURCE_DIR}/Utilities
${CMAKE_CURRENT_SOURCE_DIR}/Base
${CMAKE_CURRENT_SOURCE_DIR}/Cartridges
${CMAKE_CURRENT_SOURCE_DIR}/Cartridges/CustomCartridges
${CMAKE_CURRENT_SOURCE_DIR}/CIA
${CMAKE_CURRENT_SOURCE_DIR}/CPU
${CMAKE_CURRENT_SOURCE_DIR}/FileSystems
${CMAKE_CURRENT_SOURCE_DIR}/LogicBoard
${CMAKE_CURRENT_SOURCE_DIR}/Media
${CMAKE_CURRENT_SOURCE_DIR}/Memory
${CMAKE_CURRENT_SOURCE_DIR}/Misc
${CMAKE_CURRENT_SOURCE_DIR}/Misc/RegressionTester
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/Drive
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/Datasette
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/Joystick
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/Mouse
${CMAKE_CURRENT_SOURCE_DIR}/Peripherals/Keyboard
${CMAKE_CURRENT_SOURCE_DIR}/RetroShell
${CMAKE_CURRENT_SOURCE_DIR}/SID
${CMAKE_CURRENT_SOURCE_DIR}/SID/resid
${CMAKE_CURRENT_SOURCE_DIR}/SID/fastsid
${CMAKE_CURRENT_SOURCE_DIR}/VICII
${CMAKE_CURRENT_SOURCE_DIR}/VICII/Recorder
)

# Add sub directories
add_subdirectory(Utilities)
add_subdirectory(Base)
add_subdirectory(Cartridges)
add_subdirectory(CIA)
add_subdirectory(CPU)
add_subdirectory(FileSystems)
add_subdirectory(LogicBoard)
add_subdirectory(Media)
add_subdirectory(Memory)
add_subdirectory(Misc)
add_subdirectory(Peripherals)
add_subdirectory(RetroShell)
add_subdirectory(SID)
add_subdirectory(VICII)

# Add libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(vc64Core resid Threads::Threads)
