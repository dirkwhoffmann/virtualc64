---
title: "CPU"
---
# CPU

The CPU, its debugger, and the breakpoints and watchpoints.

## Classes

| Name | Description |
|------|-------------|
| [`CPUAPI`](vc64-CPUAPI.md#cpuapi) | CPU [API](vc64-API.md#api) |
| [`CPUDebuggerAPI`](vc64-CPUDebuggerAPI.md#cpudebuggerapi) | CPU Debugger [API](vc64-API.md#api) |
| [`GuardsAPI`](vc64-GuardsAPI.md#guardsapi) | Guards [API](vc64-API.md#api) |
| [`CPUConfig`](vc64-CPUConfig.md#cpuconfig) | Configuration of the CPU. |
| [`CPUInfo`](vc64-CPUInfo.md#cpuinfo) | State of the CPU. |
| [`GuardCmd`](vc64-GuardCmd.md#guardcmd) | Payload of breakpoint and watchpoint commands. |
| [`GuardInfo`](vc64-GuardInfo.md#guardinfo) | Description of a breakpoint or watchpoint. |
| [`Guard`](vc64-peddle-Guard.md#guard) | Base structure for a single breakpoint or watchpoint. |

## Enumerations

| Name | Description |
|------|-------------|
| [`DasmNumbers`](#dasmnumbers)  |  |
| [`GuardType`](#guardtype)  |  |

---

### DasmNumbers

```cpp
enum DasmNumbers
```

Defined in Components/CPU/CPUTypes.h:58

| Value | Description |
|-------|-------------|
| `HEX` |  |
| `HEX0` |  |
| `DEC` |  |
| `DEC0` |  |

---

### GuardType

```cpp
enum GuardType
```

Defined in Components/CPU/CPUTypes.h:94

| Value | Description |
|-------|-------------|
| `BREAKPOINT` |  |
| `WATCHPOINT` |  |
