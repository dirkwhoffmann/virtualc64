target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Drive.cpp
DriveBase.cpp
DriveMemory.cpp
DriveMemoryBase.cpp
VIA.cpp
PIA.cpp
ParCable.cpp
Disk.cpp
DiskAnalyzer.cpp

)
