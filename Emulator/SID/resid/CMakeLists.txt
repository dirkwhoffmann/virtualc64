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
endif()
