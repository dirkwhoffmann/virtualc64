target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

Drive.cpp
DriveBase.cpp
DriveMemory.cpp
DriveMemoryBase.cpp
VIA.cpp
PIA.cpp
ParallelCable.cpp
Disk.cpp
DiskAnalyzer.cpp

)
