target_include_directories(VCCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VCCore PRIVATE

CPU.cpp
CPUBase.cpp

)

add_subdirectory(Peddle)
