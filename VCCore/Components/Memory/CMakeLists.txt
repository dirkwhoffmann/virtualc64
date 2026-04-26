target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

Memory.cpp
MemoryBase.cpp
MemoryDebugger.cpp
Heatmap.cpp

)
