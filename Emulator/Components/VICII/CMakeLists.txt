target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

VICII.cpp
VICIIInfo.cpp
VICIIConfig.cpp
VICIIDraw.cpp
VICIISprites.cpp
VICIIMemory.cpp
VICIICycles.cpp
VICIITable.cpp
DmaDebugger.cpp

)

add_subdirectory(Recorder)
