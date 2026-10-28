---
title: "RemoteManagerAPI"
---
# RemoteManagerAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class RemoteManagerAPI
```

Defined in VirtualC64.h:1383

RemoteManager Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| const [`RemoteManagerInfo`](vc64-RemoteManagerInfo.md#remotemanagerinfo) & | [`getInfo`](#getinfo-15) `const` | Returns the component's current state. |
| const [`RemoteManagerInfo`](vc64-RemoteManagerInfo.md#remotemanagerinfo) & | [`getCachedInfo`](#getcachedinfo-15) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-15)). |
| `void` | [`send`](#send)  | Sends a raw payload through the specified remote server. It is delivered through the server's currently configured transport and silently dropped if no client is connected. |

---

### getInfo

`const`

```cpp
const RemoteManagerInfo & getInfo() const
```

Defined in VirtualC64.h:1403

Returns the component's current state.

[getInfo()](#getinfo-15) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-15) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const RemoteManagerInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1405

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-15)).

---

### send

```cpp
void send(ServerType server, const string & payload)
```

Defined in VirtualC64.h:1418

Sends a raw payload through the specified remote server. It is delivered through the server's currently configured transport and silently dropped if no client is connected.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `ServerType` | The server to send through. |
| `payload` | `const string &` | The raw payload to send. |

