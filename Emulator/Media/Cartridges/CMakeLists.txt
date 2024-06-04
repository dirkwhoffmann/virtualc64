target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Cartridge.cpp
CartridgeBase.cpp
CartridgeRom.cpp
FlashRom.cpp

)

add_subdirectory(CustomCartridges)
