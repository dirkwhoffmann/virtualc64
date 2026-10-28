---
title: "User and serial ports"
---
# User and serial ports

The user port with the RS232 adapter, the serial (IEC) port, and the power port.

## Classes

| Name | Description |
|------|-------------|
| [`RS232API`](vc64-RS232API.md#rs232api) | RS232 Public [API](vc64-API.md#api) |
| [`SerialPortAPI`](vc64-SerialPortAPI.md#serialportapi) | Serial Port [API](vc64-API.md#api) |
| [`UserPortAPI`](vc64-UserPortAPI.md#userportapi) | User Port [API](vc64-API.md#api) |
| [`PowerPortConfig`](vc64-PowerPortConfig.md#powerportconfig) | Configuration of the power port. |
| [`RS232Config`](vc64-RS232Config.md#rs232config) | Configuration of the RS232 adapter. |
| [`UserPortConfig`](vc64-UserPortConfig.md#userportconfig) | Configuration of the user port. |
| [`UserPortInfo`](vc64-UserPortInfo.md#userportinfo) | State of the user port (currently carries no data). |

## Enumerations

| Name | Description |
|------|-------------|
| [`CommunicationDevice`](#communicationdevice)  | Devices that can be connected to the RS232 adapter. |
| [`PowerGrid`](#powergrid)  | Power grip characteristics. |
| [`UserPortDevice`](#userportdevice)  |  |

---

### CommunicationDevice

```cpp
enum CommunicationDevice
```

Defined in Peripherals/Network/RS232Types.h:25

Devices that can be connected to the RS232 adapter.

| Value | Description |
|-------|-------------|
| `NONE` |  |
| `LOOPBACK` |  |
| `RETROSHELL` |  |
| `COMMANDER` |  |

---

### PowerGrid

```cpp
enum PowerGrid
```

Defined in Ports/PowerPortTypes.h:25

Power grip characteristics.

| Value | Description |
|-------|-------------|
| `STABLE_50HZ` | 50Hz constant |
| `UNSTABLE_50HZ` | 50Hz with jitter |
| `STABLE_60HZ` | 60Hz constant |
| `UNSTABLE_60HZ` | 60Hz with jitter |

---

### UserPortDevice

```cpp
enum UserPortDevice
```

Defined in Ports/UserPortTypes.h:23

| Value | Description |
|-------|-------------|
| `NONE` |  |
| `PARCABLE` |  |
| `PARCABLE_DOLPHIN` |  |
| `RS232` |  |
