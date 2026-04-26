target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

AudioPort.cpp
AudioPortBase.cpp
ControlPort.cpp
ControlPortBase.cpp
ExpansionPort.cpp
ExpansionPortBase.cpp
PowerPort.cpp
PowerPortBase.cpp
SampleRateDetector.cpp
SerialPort.cpp
SerialPortBase.cpp
UserPort.cpp
UserPortBase.cpp
VideoPort.cpp

)
