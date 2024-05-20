target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

ReSID.cpp
SIDBridge.cpp
SIDBridgeConfig.cpp
SID.cpp
SIDConfig.cpp
SIDInfo.cpp
SIDStreams.cpp

)

add_subdirectory(resid)
