target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

C64Memory.cpp
C64MemoryBase.cpp
Heatmap.cpp

)
