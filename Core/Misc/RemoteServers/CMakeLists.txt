target_include_directories(VC64Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(VC64Core PRIVATE

RemoteManager.cpp
RemoteServer.cpp
SocketServer.cpp
HttpServer.cpp
RshServer.cpp
DapServer.cpp
DapServerCmds.cpp
GdbServer.cpp
GdbServerCmds.cpp
Socket.cpp

)
