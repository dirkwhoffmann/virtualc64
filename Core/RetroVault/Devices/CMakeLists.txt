target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

BlockDevice.cpp
DeviceError.cpp
DeviceDescriptors.cpp
LinearDevice.cpp
TrackDevice.cpp
Volume.cpp

)
