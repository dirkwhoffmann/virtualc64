target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

Command.cpp
Console.cpp
CommandConsole.cpp
DebugConsole.cpp
RetroShell.cpp
TextStorage.cpp

)
