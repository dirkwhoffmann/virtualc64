target_include_directories(RetroVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(RetroVault PRIVATE

FSError.cpp
PosixView.cpp

)

add_subdirectory(Amiga)
add_subdirectory(CBM)
