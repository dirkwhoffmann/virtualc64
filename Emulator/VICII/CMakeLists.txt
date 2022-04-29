target_sources(vc64Core PRIVATE

VICII.cpp
VICII_draw.cpp
VICII_sprites.cpp
VICII_colors.cpp
VICII_memory.cpp
VICII_cycles.cpp
VICII_table.cpp
VICII_debug.cpp
DmaDebugger.cpp

)

add_subdirectory(Recorder)
