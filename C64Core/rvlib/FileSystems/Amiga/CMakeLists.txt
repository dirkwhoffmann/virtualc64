target_include_directories(rvlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(rvlib PRIVATE

FileSystem.cpp
FileSystemLayer1.cpp
FileSystemLayer2.cpp
FileSystemLayer3.cpp
FSAllocator.cpp
FSBlock.cpp
FSBootBlockImage.cpp
FSCache.cpp
FSContract.cpp
FSDescriptor.cpp
FSDoctor.cpp
FSExporter.cpp
FSImporter.cpp
FSObjects.cpp
FSService.cpp
FSTree.cpp
PosixAdapter.cpp

)
