---
title: "Remote servers"
---
# Remote servers

The remote servers and their traffic log.

## Classes

| Name | Description |
|------|-------------|
| [`RemoteManagerAPI`](vc64-RemoteManagerAPI.md#remotemanagerapi) | RemoteManager Public [API](vc64-API.md#api) |
| [`RemoteManagerInfo`](vc64-RemoteManagerInfo.md#remotemanagerinfo) | State of the remote servers. |
| [`RemoteServerInfo`](vc64-RemoteServerInfo.md#remoteserverinfo) | State of a remote server. |
| [`ServerConfig`](vc64-ServerConfig.md#serverconfig) | Configuration of a remote server. |
| [`TrafficEntry`](vc64-TrafficEntry.md#trafficentry) | An entry of the traffic log. |

## Enumerations

| Name | Description |
|------|-------------|
| [`ServerType`](#servertype)  |  |
| [`TrafficDirection`](#trafficdirection)  | Direction of a packet in the traffic log. |

---

### ServerType

```cpp
enum ServerType
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:23

| Value | Description |
|-------|-------------|
| `RSH` |  |
| `RPC` |  |
| `DAP` |  |
| `PROM` |  |

---

### TrafficDirection

```cpp
enum TrafficDirection
```

Defined in Misc/RemoteServers/RemoteManagerTypes.h:62

Direction of a packet in the traffic log.

| Value | Description |
|-------|-------------|
| `RECEIVED` | The packet was received from a client. |
| `SENT` | The packet was sent to a client. |
