target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

CIA.cpp
CIAConfig.cpp
CPUInfo.cpp
CIARegs.cpp
TOD.cpp

)
