add_executable(VC64Tests main.cpp PaperTests.cpp IecListenerTests.cpp Mps803Tests.cpp)
target_link_libraries(VC64Tests VCCore)
add_test(NAME UnitTests COMMAND VC64Tests)
