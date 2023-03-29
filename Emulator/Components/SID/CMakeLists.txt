target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Muxer.cpp
SIDStreams.cpp
ReSID.cpp

)

add_subdirectory(resid)
add_subdirectory(fastsid)
