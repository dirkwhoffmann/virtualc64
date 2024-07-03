target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

AudioPort.cpp
AudioPortBase.cpp
ControlPort.cpp
ControlPortBase.cpp
ExpansionPort.cpp
ExpansionPortBase.cpp
PowerPort.cpp
PowerPortBase.cpp
SerialPort.cpp
SerialPortBase.cpp
UserPort.cpp
UserPortBase.cpp
VideoPort.cpp
VideoPortBase.cpp

)
