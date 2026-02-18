add_library(RetroVault)

target_include_directories(RetroVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RetroVault PRIVATE utlib xdms)
add_subdirectory(Devices)
add_subdirectory(Images)
add_subdirectory(FileSystems)
add_subdirectory(ThirdParty)
