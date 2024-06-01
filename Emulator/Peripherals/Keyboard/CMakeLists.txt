target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

C64Key.cpp
Keyboard.cpp
KeyboardBase.cpp

)
