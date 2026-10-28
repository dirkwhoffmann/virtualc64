---
title: "ServerConfig"
---
# ServerConfig

```cpp
#include <Misc/RemoteServers/RemoteServerTypes.h>
```

```cpp
struct ServerConfig
```

Defined in Misc/RemoteServers/RemoteServerTypes.h:25

Configuration of a remote server.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`enable`](#enable-1)  | Enable status. |
| `TransportProtocol` | [`transport`](#transport)  | The transport protocol of this server. |
| `u16` | [`port`](#port-4)  | The socket port of this server. |
| `bool` | [`verbose`](#verbose)  | If true, transmitted packets are shown in RetroShell. |

---

### enable

```cpp
bool enable
```

Defined in Misc/RemoteServers/RemoteServerTypes.h:28

Enable status.

---

### transport

```cpp
TransportProtocol transport
```

Defined in Misc/RemoteServers/RemoteServerTypes.h:31

The transport protocol of this server.

---

### port

```cpp
u16 port
```

Defined in Misc/RemoteServers/RemoteServerTypes.h:34

The socket port of this server.

---

### verbose

```cpp
bool verbose
```

Defined in Misc/RemoteServers/RemoteServerTypes.h:37

If true, transmitted packets are shown in RetroShell.

