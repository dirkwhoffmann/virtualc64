cmake_minimum_required(VERSION 3.20)
project(utlib LANGUAGES C CXX)

# -------------------------------------------------------------------
# Options
# -------------------------------------------------------------------
option(UTLIB_BUILD_TESTS "Build utlib tests" OFF)

# -------------------------------------------------------------------
# Add library
# -------------------------------------------------------------------
# Header-only portion
add_library(utlib INTERFACE)

# Include directories for consumers
target_include_directories(utlib
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Require C++17 minimum
target_compile_features(utlib INTERFACE cxx_std_17)

# -------------------------------------------------------------------
# Add optional compiled sources (if you have .cpp)
# -------------------------------------------------------------------
# Example: exceptions
file(GLOB_RECURSE UTL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/*.c"
)

if(UTL_SRC)
    add_library(utlib_core STATIC ${UTL_SRC})
    target_include_directories(utlib_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(utlib_core PUBLIC cxx_std_17)

    # Make INTERFACE library propagate compiled library
    target_link_libraries(utlib INTERFACE utlib_core)
endif()

# -------------------------------------------------------------------
# Optional: Testing
# -------------------------------------------------------------------
if(UTLIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
