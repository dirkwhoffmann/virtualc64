target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

RetroShellCmd.cpp
Console.cpp
CommandConsole.cpp
DebugConsole.cpp
RetroShell.cpp
TextStorage.cpp

)
