target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

AnyFile.cpp
RomFile.cpp
Snapshot.cpp
TAPFile.cpp
CRTFile.cpp
AnyCollection.cpp
T64File.cpp
PRGFile.cpp
P00File.cpp
D64File.cpp
G64File.cpp
Folder.cpp
Script.cpp

)
