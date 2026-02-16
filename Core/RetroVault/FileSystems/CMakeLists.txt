target_include_directories(VACore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VACore PRIVATE

FSError.cpp
PosixView.cpp

)

add_subdirectory(Amiga)
add_subdirectory(CBM)
