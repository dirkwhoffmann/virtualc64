target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

Codecs.cpp
Drive.cpp
DriveBase.cpp
DriveMemory.cpp
DriveMemoryBase.cpp
VIA.cpp
PIA.cpp
ParallelCable.cpp
FloppyDisk.cpp
DiskAnalyzer.cpp

)
