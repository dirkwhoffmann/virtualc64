---
title: "RemoteManagerInfo"
kind: struct
---
# RemoteManagerInfo

```cpp
#include <Misc/RemoteServers/RemoteManagerTypes.h>
```

```cpp
struct RemoteManagerInfo
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:99

State of the remote servers.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo) | [`rshInfo`](#rshinfo)  | State of the RetroShell server. |
| [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo) | [`rpcInfo`](#rpcinfo)  | State of the RPC server. |
| [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo) | [`dapInfo`](#dapinfo)  | State of the DAP server. |
| [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo) | [`promInfo`](#prominfo)  | State of the Prometheus server. |

---

### rshInfo

```cpp
RemoteServerInfo rshInfo
```

Type: [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo)

Defined in Misc/RemoteServers/RemoteManagerTypes.h:101

State of the RetroShell server.

---

### rpcInfo

```cpp
RemoteServerInfo rpcInfo
```

Type: [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo)

Defined in Misc/RemoteServers/RemoteManagerTypes.h:102

State of the RPC server.

---

### dapInfo

```cpp
RemoteServerInfo dapInfo
```

Type: [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo)

Defined in Misc/RemoteServers/RemoteManagerTypes.h:103

State of the DAP server.

---

### promInfo

```cpp
RemoteServerInfo promInfo
```

Type: [`RemoteServerInfo`](/reference/vc64-RemoteServerInfo.md#remoteserverinfo)

Defined in Misc/RemoteServers/RemoteManagerTypes.h:104

State of the Prometheus server.

