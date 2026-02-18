target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

MediaError.cpp
MediaFile.cpp
AnyCollection.cpp
AnyFile.cpp

CRTFile.cpp
D64File.cpp
G64File.cpp
PRGFile.cpp
P00File.cpp
RomFile.cpp
Script.cpp
Snapshot.cpp
TAPFile.cpp
T64File.cpp
Workspace.cpp

)

add_subdirectory(Cartridges)
