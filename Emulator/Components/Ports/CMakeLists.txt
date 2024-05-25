target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

AudioPort.cpp
AudioPortConfig.cpp
ControlPort.cpp
ExpansionPort.cpp
IEC.cpp
PowerPort.cpp
VideoPort.cpp

)
