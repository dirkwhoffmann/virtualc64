target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

ReSID.cpp
SID.cpp
SIDBase.cpp
SIDBridge.cpp

)

add_subdirectory(resid)
