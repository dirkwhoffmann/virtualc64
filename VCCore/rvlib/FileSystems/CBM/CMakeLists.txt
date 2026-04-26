target_include_directories(rvlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(rvlib PRIVATE

FileSystem.cpp
FileSystemLayer1.cpp
FileSystemLayer2.cpp
FileSystemLayer3.cpp
FSAllocator.cpp
FSBlock.cpp
FSCache.cpp
FSContract.cpp
FSDirEntry.cpp
FSDoctor.cpp
FSExporter.cpp
FSImporter.cpp
FSObjects.cpp
FSService.cpp
FSTraits.cpp
PosixAdapter.cpp

)
