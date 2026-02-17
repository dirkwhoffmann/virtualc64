target_include_directories(VACore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(xdms

  u_heavy.c
  tables.c
  u_deep.c
  u_medium.c
  u_init.c
  maketbl.c
  crc_csum.c
  pfile.c
  u_rle.c
  getbits.c
  u_quick.c
)
