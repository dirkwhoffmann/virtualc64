target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

CIA.cpp
CIABase.cpp
CIARegs.cpp
TOD.cpp
TODBase.cpp

)
