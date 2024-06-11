target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

CmdQueue.cpp
Colors.cpp
Configurable.cpp
CoreObject.cpp
CoreComponent.cpp
Defaults.cpp
Dumpable.cpp
Error.cpp
Host.cpp
Inspectable.cpp
MsgQueue.cpp
Option.cpp
SubComponent.cpp
Synchronizable.cpp
Serializable.cpp
Suspendable.cpp
Thread.cpp
Wakeable.cpp

)
