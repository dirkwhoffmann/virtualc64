target_include_directories(VACore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VACore PRIVATE

               AmigaEncoder.cpp
               AmigaDecoder.cpp
               C64Encoder.cpp
               C64Decoder.cpp
               DiskDecoder.cpp
               DOSEncoder.cpp
               DOSDecoder.cpp
               GCR.cpp
               MFM.cpp

               )
