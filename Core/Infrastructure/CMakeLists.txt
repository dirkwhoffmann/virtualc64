target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

CmdQueue.cpp
Colors.cpp
Configurable.cpp
CoreObject.cpp
CoreComponent.cpp
Defaults.cpp
Dumpable.cpp
Emulator.cpp
EmulatorBase.cpp
Error.cpp
Host.cpp
Inspectable.cpp
MsgQueue.cpp
Option.cpp
SubComponent.cpp
Synchronizable.cpp
Serializable.cpp
Thread.cpp
Wakeable.cpp

)
