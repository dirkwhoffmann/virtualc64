target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

CmdQueue.cpp
Colors.cpp
Configurable.cpp
CoreError.cpp
CoreObject.cpp
CoreComponent.cpp
Defaults.cpp
Dumpable.cpp
Emulator.cpp
Error.cpp
Host.cpp
Inspectable.cpp
MsgQueue.cpp
Option.cpp
SubComponent.cpp
Thread.cpp

)
