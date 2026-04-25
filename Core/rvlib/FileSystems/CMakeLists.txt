target_include_directories(rvlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(rvlib PRIVATE
        FSError.cpp
        PosixView.cpp)

add_subdirectory(Amiga)
add_subdirectory(CBM)
