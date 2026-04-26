target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

Peddle.cpp
PeddleDebugger.cpp
PeddleDisassembler.cpp
StrWriter.cpp
SymbolTable.cpp

)
