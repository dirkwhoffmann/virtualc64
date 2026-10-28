---
title: "ExpansionPortAPI"
---
# ExpansionPortAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class ExpansionPortAPI
```

Defined in VirtualC64.h:1197

Expansion Port Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| [`CartridgeTraits`](vc64-CartridgeTraits.md#cartridgetraits) | [`getCartridgeTraits`](#getcartridgetraits) `const` | Returns the traits of the current cartridge. |
| const [`CartridgeInfo`](vc64-CartridgeInfo.md#cartridgeinfo) & | [`getInfo`](#getinfo-6) `const` | Returns the state of the current cartridge. |
| const [`CartridgeInfo`](vc64-CartridgeInfo.md#cartridgeinfo) & | [`getCachedInfo`](#getcachedinfo-6) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-6)). |
| [`CartridgeRomInfo`](vc64-CartridgeRomInfo.md#cartridgerominfo) | [`getRomInfo`](#getrominfo) `const` | Returns the state of one of the cartridge ROM packets. |
| `void` | [`attachCartridge`](#attachcartridge)  | Attaches a cartridge to the expansion port. |
| `void` | [`attachCartridgeAsync`](#attachcartridgeasync)  | Asynchronous version of [attachCartridge()](#attachcartridge): queues the request and returns at once. |
| `void` | [`attachReu`](#attachreu)  | Attaches a RAM Expansion Unit to the expansion port. |
| `void` | [`attachGeoRam`](#attachgeoram)  | Attaches a GeoRAM module to the expansion port. |
| `void` | [`attachIsepicCartridge`](#attachisepiccartridge)  | Attaches an Isepic module to the expansion port. |
| `void` | [`detachCartridge`](#detachcartridge)  | Detaches the currently plugged in cartridge. |
| `void` | [`exportCRT`](#exportcrt) `const` | Export cartridge. |

---

### getCartridgeTraits

`const`

```cpp
CartridgeTraits getCartridgeTraits() const
```

Defined in VirtualC64.h:1210

Returns the traits of the current cartridge.

---

### getInfo

`const`

```cpp
const CartridgeInfo & getInfo() const
```

Defined in VirtualC64.h:1221

Returns the state of the current cartridge.

[getInfo()](#getinfo-6) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-6) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const CartridgeInfo & getCachedInfo() const
```

Defined in VirtualC64.h:1223

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-6)).

---

### getRomInfo

`const`

```cpp
CartridgeRomInfo getRomInfo(isize nr) const
```

Defined in VirtualC64.h:1228

Returns the state of one of the cartridge ROM packets.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `nr` | `isize` | Number of the ROM packet. |

---

### attachCartridge

```cpp
void attachCartridge(const std::filesystem::path & path, bool reset = true)
```

Defined in VirtualC64.h:1238

Attaches a cartridge to the expansion port.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be read or does not contain valid data. |

---

### attachCartridgeAsync

```cpp
void attachCartridgeAsync(const std::filesystem::path & path, bool reset = true)
```

Defined in VirtualC64.h:1245

Asynchronous version of [attachCartridge()](#attachcartridge): queues the request and returns at once.

The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.

---

### attachReu

```cpp
void attachReu(isize capacity)
```

Defined in VirtualC64.h:1249

Attaches a RAM Expansion Unit to the expansion port.

---

### attachGeoRam

```cpp
void attachGeoRam(isize capacity)
```

Defined in VirtualC64.h:1253

Attaches a GeoRAM module to the expansion port.

---

### attachIsepicCartridge

```cpp
void attachIsepicCartridge()
```

Defined in VirtualC64.h:1257

Attaches an Isepic module to the expansion port.

---

### detachCartridge

```cpp
void detachCartridge()
```

Defined in VirtualC64.h:1261

Detaches the currently plugged in cartridge.

---

### exportCRT

`const`

```cpp
void exportCRT(const std::filesystem::path & path) const
```

Defined in VirtualC64.h:1265

Export cartridge.

