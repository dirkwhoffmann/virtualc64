target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(vc64Core PRIVATE

  Buffer.cpp
  Chrono.cpp
  Concurrency.cpp
  MemUtils.cpp
  Checksum.cpp
  StringUtils.cpp
  IOUtils.cpp
  Parser.cpp
)
