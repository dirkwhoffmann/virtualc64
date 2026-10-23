target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

VICII.cpp
VICIIBase.cpp
VICIIDraw.cpp
VICIISprites.cpp
VICIIMemory.cpp
VICIICycles.cpp
VICIITable.cpp
XRay.cpp
XRayBase.cpp
Texture.cpp

)
