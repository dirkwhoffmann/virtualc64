target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

CPU.cpp
CPUBase.cpp

)

add_subdirectory(Peddle)
