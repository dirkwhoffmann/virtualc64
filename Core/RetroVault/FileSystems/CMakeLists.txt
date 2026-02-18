target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

FSError.cpp
PosixView.cpp

)

add_subdirectory(Amiga)
add_subdirectory(CBM)
