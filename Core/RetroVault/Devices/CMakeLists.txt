target_include_directories(RetroVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(RetroVault PRIVATE

BlockDevice.cpp
DeviceError.cpp
DeviceDescriptors.cpp
LinearDevice.cpp
TrackDevice.cpp
Volume.cpp

)
