target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

MediaFile.cpp
AnyCollection.cpp
AnyFile.cpp

CRTFile.cpp
D64File.cpp
Folder.cpp
G64File.cpp
PRGFile.cpp
P00File.cpp
RomFile.cpp
Script.cpp
Snapshot.cpp
TAPFile.cpp
T64File.cpp

)

add_subdirectory(Cartridges)
