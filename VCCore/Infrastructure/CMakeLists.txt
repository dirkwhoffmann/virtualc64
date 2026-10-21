target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

CmdQueue.cpp
Configurable.cpp
CoreError.cpp
CoreObject.cpp
CoreComponent.cpp
Defaults.cpp
Dumpable.cpp
Emulator.cpp
Host.cpp
MsgQueue.cpp
Option.cpp
SubComponent.cpp
Thread.cpp

)
