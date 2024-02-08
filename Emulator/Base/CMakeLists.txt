target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Error.cpp
Colors.cpp
CoreObject.cpp
CoreComponent.cpp
SubComponent.cpp
Suspandable.cpp
Wakeable.cpp
Thread.cpp
MsgQueue.cpp
Defaults.cpp
Host.cpp
)
