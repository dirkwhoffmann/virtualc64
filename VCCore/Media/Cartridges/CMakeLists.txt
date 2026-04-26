target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

Cartridge.cpp
CartridgeBase.cpp
CartridgeRom.cpp
FlashRom.cpp

)

add_subdirectory(CustomCartridges)
