
target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

C64.cpp
C64Base.cpp

)

add_subdirectory(CPU)
add_subdirectory(CIA)
add_subdirectory(Memory)
add_subdirectory(SID)
add_subdirectory(VICII)
