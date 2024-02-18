target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Error.cpp
Colors.cpp
CoreObject.cpp
CoreComponent.cpp
SubComponent.cpp
Synchronizable.cpp
Serializable.cpp
Suspendable.cpp
Inspectable.cpp
Wakeable.cpp
Thread.cpp
CmdQueue.cpp
MsgQueue.cpp
Defaults.cpp
Host.cpp
)
