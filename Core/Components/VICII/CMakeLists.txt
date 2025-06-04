target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

VICII.cpp
VICIIBase.cpp
VICIIDraw.cpp
VICIISprites.cpp
VICIIMemory.cpp
VICIICycles.cpp
VICIITable.cpp
DmaDebugger.cpp
DmaDebuggerBase.cpp

)
