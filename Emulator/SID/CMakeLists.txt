target_sources(vc64Core PRIVATE

Muxer.cpp
SIDStreams.cpp
ReSID.cpp

)

add_subdirectory(resid)
add_subdirectory(fastsid)
