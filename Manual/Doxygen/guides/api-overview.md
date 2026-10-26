---
title: API Overview
description: How the public API is organized, and where to find each API class
---


This page is the entry point for looking up an API function. It explains how the public API is organized and links to all classes that make it up.

## Structure

The whole public API hangs off a single object of class [vc64::VirtualC64](/api-reference/vc64-VirtualC64.html). The class has two kinds of members:

- **Functions** that act on the emulator as a whole: querying and controlling the execution state, resetting, single-stepping, configuring, and synchronizing with the emulator thread. They are grouped into sections (*Controlling the emulator state*, *Configuring the emulator*, *Danger zone*, ...) on the class page.
- **Sub-APIs**, one per hardware component or subsystem. They are public data members, so a call reads like `c64.cpu.getInfo()` or `c64.drive8.insert(...)`. Sub-APIs may contain further sub-APIs, for example `c64.cpu.breakpoints` or `c64.controlPort1.joystick`.

All API classes are derived from [vc64::API](/api-reference/vc64-API.html), which provides [suspend()](/api-reference/vc64-API.html#suspend) and [resume()](/api-reference/vc64-API.html#resume). They all follow the same conventions for [threading and error handling](threading.md), which is why those rules are not repeated for each function.

Three naming patterns recur throughout the API:

| Pattern | Meaning |
|:--------|:--------|
| `getConfig()`, `get(Opt)` / `set(Opt, ...)` | Read and change the configuration. Options are identified by the enumeration [vc64::Opt](/api-reference/grp_config.html#opt). |
| `getInfo()`, `getCachedInfo()`, `getMetrics()`, ... | Read the state of a component (see [Threading and Errors](threading.md)). |
| `foo()` / `fooAsync()` | Blocking and asynchronous flavor of a state-changing function. |

To be notified about events, register a message callback with [launch()](/api-reference/vc64-VirtualC64.html#launch). The messages are listed in the enumeration [vc64::Msg](/api-reference/grp_messages.html#msg).

## The main class

| Class | Purpose |
|:------|:--------|
| [vc64::VirtualC64](/api-reference/vc64-VirtualC64.html) | Entry point. Execution state, resetting, configuration, synchronization, and the sub-APIs listed below. |
| [vc64::API](/api-reference/vc64-API.html) | Base class of all APIs (suspend and resume). |

## Components

| Member | Class | Purpose |
|:-------|:------|:--------|
| `c64` | [vc64::C64API](/api-reference/vc64-C64API.html) | The machine as a whole: snapshots, workspaces, ROMs, media files, auto-inspection. |
| `mem` | [vc64::MemoryAPI](/api-reference/vc64-MemoryAPI.html) | Memory configuration, state, heat map. Its member `mem.debugger` ([vc64::MemoryDebuggerAPI](/api-reference/vc64-MemoryDebuggerAPI.html)) dumps memory. |
| `cpu` | [vc64::CPUAPI](/api-reference/vc64-CPUAPI.html) | CPU state. Its members `cpu.debugger` ([vc64::CPUDebuggerAPI](/api-reference/vc64-CPUDebuggerAPI.html)) disassembles code, `cpu.breakpoints` and `cpu.watchpoints` ([vc64::GuardsAPI](/api-reference/vc64-GuardsAPI.html)) manage guards. |
| `cia1`, `cia2` | [vc64::CIAAPI](/api-reference/vc64-CIAAPI.html) | The two CIA chips. |
| `sid` | [vc64::SIDAPI](/api-reference/vc64-SIDAPI.html) | The SID chip(s). |
| `vicii` | [vc64::VICIIAPI](/api-reference/vc64-VICIIAPI.html) | The VICII graphics chip. |

## Ports

| Member | Class | Purpose |
|:-------|:------|:--------|
| `audioPort` | [vc64::AudioPortAPI](/api-reference/vc64-AudioPortAPI.html) | Audio output: sample retrieval, volume, waveform drawing. |
| `videoPort` | [vc64::VideoPortAPI](/api-reference/vc64-VideoPortAPI.html) | Video output: textures, screenshots. |
| `controlPort1`, `controlPort2` | [vc64::ControlPortAPI](/api-reference/vc64-ControlPortAPI.html) | The control ports. Their members `joystick` ([vc64::JoystickAPI](/api-reference/vc64-JoystickAPI.html)), `mouse` ([vc64::MouseAPI](/api-reference/vc64-MouseAPI.html)), and `paddle` ([vc64::PaddleAPI](/api-reference/vc64-PaddleAPI.html)) control the connected device. |
| `expansionPort` | [vc64::ExpansionPortAPI](/api-reference/vc64-ExpansionPortAPI.html) | Cartridges, REU, GeoRAM. |
| `serialPort` | [vc64::SerialPortAPI](/api-reference/vc64-SerialPortAPI.html) | The serial (IEC) port. |
| `userPort` | [vc64::UserPortAPI](/api-reference/vc64-UserPortAPI.html) | The user port. Its member `rs232` ([vc64::RS232API](/api-reference/vc64-RS232API.html)) is the RS232 adapter. |

## Peripherals

| Member | Class | Purpose |
|:-------|:------|:--------|
| `keyboard` | [vc64::KeyboardAPI](/api-reference/vc64-KeyboardAPI.html) | Key presses, auto-typing. Keys are described by [vc64::C64Key](/api-reference/vc64-C64Key.html). |
| `drive8`, `drive9` | [vc64::DriveAPI](/api-reference/vc64-DriveAPI.html) | The floppy drives. The member `disk` ([vc64::DiskAPI](/api-reference/vc64-DiskAPI.html)) gives access to the inserted disk. |
| `datasette` | [vc64::DatasetteAPI](/api-reference/vc64-DatasetteAPI.html) | The tape recorder. |

## Miscellaneous

| Member | Class | Purpose |
|:-------|:------|:--------|
| `retroShell` | [vc64::RetroShellAPI](/api-reference/vc64-RetroShellAPI.html) | The command console: typing, executing scripts. |
| `remoteManager` | [vc64::RemoteManagerAPI](/api-reference/vc64-RemoteManagerAPI.html) | Remote servers (RetroShell, RPC, DAP, ...). |
| `xray` | [vc64::XRayAPI](/api-reference/vc64-XRayAPI.html) | The X-Ray visualizer. |
| `defaults` (static) | [vc64::DefaultsAPI](/api-reference/vc64-DefaultsAPI.html) | Persistent user defaults. Available without an emulator instance. |

## Data types

Functions exchange data through plain structures and enumerations. The most important ones are:

- Configuration options: [vc64::Opt](/api-reference/grp_config.html#opt)
- Messages from the emulator: [vc64::Message](/api-reference/vc64-Message.html) and [vc64::Msg](/api-reference/grp_messages.html#msg)
- State records: for example [vc64::CPUInfo](/api-reference/vc64-CPUInfo.html), [vc64::VICIIInfo](/api-reference/vc64-VICIIInfo.html), [vc64::CIAInfo](/api-reference/vc64-CIAInfo.html), [vc64::DriveInfo](/api-reference/vc64-DriveInfo.html)
- Execution state: [vc64::ExecState](/api-reference/grp_states.html#execstate)
- Snapshots and media files: [vc64::Snapshot](/api-reference/vc64-Snapshot.html), [vc64::AnyFile](/api-reference/vc64-AnyFile.html)

## Errors

API functions throw [vc64::CoreError](/api-reference/vc64-CoreError.html), [utl::IOError](/api-reference/utl-IOError.html), or [vc64::MediaError](/api-reference/vc64-MediaError.html), all derived from [utl::Error](/api-reference/utl-Error.html). See [Threading and Errors](threading.md).
