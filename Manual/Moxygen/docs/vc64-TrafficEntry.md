---
title: "TrafficEntry"
---
# TrafficEntry

```cpp
#include <Misc/RemoteServers/RemoteManagerTypes.h>
```

```cpp
struct TrafficEntry
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:114

An entry of the traffic log.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`nr`](#nr-2)  | Sequence number (unique, monotonically increasing). |
| `ServerType` | [`server`](#server)  | The server that received or sent the packet. |
| `TrafficDirection` | [`direction`](#direction)  | Tells whether the packet was received or sent. |
| `i64` | [`time`](#time-1)  | Host time stamp (milliseconds since epoch). |
| `string` | [`payload`](#payload-1)  | Packet contents. |

---

### nr

```cpp
isize nr
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:117

Sequence number (unique, monotonically increasing).

---

### server

```cpp
ServerType server
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:120

The server that received or sent the packet.

---

### direction

```cpp
TrafficDirection direction
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:123

Tells whether the packet was received or sent.

---

### time

```cpp
i64 time
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:126

Host time stamp (milliseconds since epoch).

---

### payload

```cpp
string payload
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:129

Packet contents.

