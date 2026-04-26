target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

Console.cpp
CommanderConsole.cpp
DebuggerConsole.cpp
RetroShell.cpp
RSCommand.cpp
RSError.cpp
TextStorage.cpp

)
