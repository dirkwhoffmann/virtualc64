add_library(RetroVault)

# Include paths
target_include_directories(RetroVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler options
target_compile_options(RetroVault PUBLIC

    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4100>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4201>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4324>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4458>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996>

    # GCC
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Werror>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-restrict>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>

    # Clang / AppleClang
    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Werror>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-gnu-anonymous-struct>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-nested-anon-types>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall>
    $<$<CXX_COMPILER_ID:AppleClang>:-Werror>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wno-gnu-anonymous-struct>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wno-nested-anon-types>
)

# Source files
add_subdirectory(Devices)
add_subdirectory(Images)
add_subdirectory(FileSystems)
add_subdirectory(ThirdParty)

# Libraries
target_link_libraries(RetroVault PRIVATE utlib xdms)
