target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

Console.cpp
CommanderConsole.cpp
DebuggerConsole.cpp
RetroShell.cpp
RSCommand.cpp
RSError.cpp
TextStorage.cpp

)
