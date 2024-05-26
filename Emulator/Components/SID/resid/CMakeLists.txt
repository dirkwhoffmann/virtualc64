target_include_directories(vc64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(resid

  envelope.cc
  pot.cc
  voice.cc
  sid.cc
  filter.cc
  dac.cc
  extfilt.cc
  wave.cc
  version.cc
)

if(MSVC)
  target_compile_options(resid PUBLIC /wd5055)
else()
  target_compile_options(resid PUBLIC -w)
endif()
