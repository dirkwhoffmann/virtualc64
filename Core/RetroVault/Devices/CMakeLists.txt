target_include_directories(VACore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VACore PRIVATE

BlockDevice.cpp
DeviceError.cpp
DeviceDescriptors.cpp
LinearDevice.cpp
TrackDevice.cpp
Volume.cpp

)
