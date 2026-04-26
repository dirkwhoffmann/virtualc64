target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

AudioStream.cpp
ReSID.cpp
SID.cpp
SIDBase.cpp
SIDBridge.cpp

)

add_subdirectory(resid)
