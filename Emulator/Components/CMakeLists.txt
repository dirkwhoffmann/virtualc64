
target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

C64.cpp
Emulator.cpp
EmulatorBase.cpp

)

add_subdirectory(CPU)
add_subdirectory(CIA)
add_subdirectory(Ports)
add_subdirectory(Memory)
add_subdirectory(SID)
add_subdirectory(VICII)
