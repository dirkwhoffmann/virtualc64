target_include_directories(RetroVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(RetroVault PRIVATE

    AnyImage.cpp
    DiskImage.cpp
    FloppyDiskImage.cpp
    HardDiskImage.cpp
    ImageError.cpp
)

add_subdirectory(ADF)
add_subdirectory(ADZ)
add_subdirectory(D64)
add_subdirectory(DMS)
add_subdirectory(EADF)
add_subdirectory(Encoders)
add_subdirectory(EXE)
add_subdirectory(HDF)
add_subdirectory(HDZ)
add_subdirectory(IMG)
add_subdirectory(ST)
