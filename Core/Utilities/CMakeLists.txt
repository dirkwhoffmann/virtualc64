target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

  Buffer.cpp
  Chrono.cpp
  Compression.cpp
  Concurrency.cpp
  MemUtils.cpp
  Checksum.cpp
  StringUtils.cpp
  IOUtils.cpp
  Parser.cpp
)
