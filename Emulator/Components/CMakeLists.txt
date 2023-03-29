
target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(CPU)
add_subdirectory(CIA)
add_subdirectory(LogicBoard)
add_subdirectory(Memory)
add_subdirectory(SID)
add_subdirectory(VICII)
