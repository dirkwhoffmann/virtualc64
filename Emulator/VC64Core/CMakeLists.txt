target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

Emulator.cpp
EmulatorBase.cpp

)

add_subdirectory(Infrastructure)
add_subdirectory(Components)
add_subdirectory(Ports)
add_subdirectory(FileSystems)
add_subdirectory(Media)
add_subdirectory(Misc)
add_subdirectory(Peripherals)
add_subdirectory(Utilities)
