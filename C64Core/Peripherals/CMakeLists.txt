target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

)

add_subdirectory(Drive)
add_subdirectory(Datasette)
add_subdirectory(Joystick)
add_subdirectory(Keyboard)
add_subdirectory(Monitor)
add_subdirectory(Mouse)
add_subdirectory(Network)
add_subdirectory(Paddle)
