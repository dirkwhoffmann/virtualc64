target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

Peddle.cpp
PeddleDebugger.cpp
PeddleDisassembler.cpp
StrWriter.cpp

)
