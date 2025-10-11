target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

AudioStream.cpp
ReSID.cpp
SID.cpp
SIDBase.cpp
SIDBridge.cpp

)

add_subdirectory(resid)
