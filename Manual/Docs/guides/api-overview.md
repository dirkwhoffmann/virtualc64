# API Overview {#guide_api_overview}


This page is the entry point for looking up an API function. It explains how the public API is organized and links to all classes that make it up.

## Structure

The whole public API hangs off a single object of class [vc64::VirtualC64](@ref vc64::VirtualC64). The class has two kinds of members:

- **Functions** that act on the emulator as a whole: querying and controlling the execution state, resetting, single-stepping, configuring, and synchronizing with the emulator thread. They are grouped into sections (*Controlling the emulator state*, *Configuring the emulator*, *Danger zone*, ...) on the class page.
- **Sub-APIs**, one per hardware component or subsystem. They are public data members, so a call reads like `c64.cpu.getInfo()` or `c64.drive8.insert(...)`. Sub-APIs may contain further sub-APIs, for example `c64.cpu.breakpoints` or `c64.controlPort1.joystick`.

All API classes are derived from [vc64::API](@ref vc64::API), which provides [suspend()](@ref vc64::API::suspend) and [resume()](@ref vc64::API::resume). They all follow the same conventions for [threading and error handling](threading.md), which is why those rules are not repeated for each function.

Three naming patterns recur throughout the API:

| Pattern | Meaning |
|:--------|:--------|
| `getConfig()`, `get(Opt)` / `set(Opt, ...)` | Read and change the configuration. Options are identified by the enumeration [vc64::Opt](@ref vc64::Opt). |
| `getInfo()`, `getCachedInfo()`, `getMetrics()`, ... | Read the state of a component (see [Threading and Errors](threading.md)). |
| `foo()` / `fooAsync()` | Blocking and asynchronous flavor of a state-changing function. |

To be notified about events, register a message callback with [launch()](@ref vc64::VirtualC64::launch). The messages are listed in the enumeration [vc64::Msg](@ref vc64::Msg).

## The main class

| Class | Purpose |
|:------|:--------|
| [vc64::VirtualC64](@ref vc64::VirtualC64) | Entry point. Execution state, resetting, configuration, synchronization, and the sub-APIs listed below. |
| [vc64::API](@ref vc64::API) | Base class of all APIs (suspend and resume). |

## Components

| Member | Class | Purpose |
|:-------|:------|:--------|
| `c64` | [vc64::C64API](@ref vc64::C64API) | The machine as a whole: snapshots, workspaces, ROMs, media files, auto-inspection. |
| `mem` | [vc64::MemoryAPI](@ref vc64::MemoryAPI) | Memory configuration, state, heat map. Its member `mem.debugger` ([vc64::MemoryDebuggerAPI](@ref vc64::MemoryDebuggerAPI)) dumps memory. |
| `cpu` | [vc64::CPUAPI](@ref vc64::CPUAPI) | CPU state. Its members `cpu.debugger` ([vc64::CPUDebuggerAPI](@ref vc64::CPUDebuggerAPI)) disassembles code, `cpu.breakpoints` and `cpu.watchpoints` ([vc64::GuardsAPI](@ref vc64::GuardsAPI)) manage guards. |
| `cia1`, `cia2` | [vc64::CIAAPI](@ref vc64::CIAAPI) | The two CIA chips. |
| `sid` | [vc64::SIDAPI](@ref vc64::SIDAPI) | The SID chip(s). |
| `vicii` | [vc64::VICIIAPI](@ref vc64::VICIIAPI) | The VICII graphics chip. |

## Ports

| Member | Class | Purpose |
|:-------|:------|:--------|
| `audioPort` | [vc64::AudioPortAPI](@ref vc64::AudioPortAPI) | Audio output: sample retrieval, volume, waveform drawing. |
| `videoPort` | [vc64::VideoPortAPI](@ref vc64::VideoPortAPI) | Video output: textures, screenshots. |
| `controlPort1`, `controlPort2` | [vc64::ControlPortAPI](@ref vc64::ControlPortAPI) | The control ports. Their members `joystick` ([vc64::JoystickAPI](@ref vc64::JoystickAPI)), `mouse` ([vc64::MouseAPI](@ref vc64::MouseAPI)), and `paddle` ([vc64::PaddleAPI](@ref vc64::PaddleAPI)) control the connected device. |
| `expansionPort` | [vc64::ExpansionPortAPI](@ref vc64::ExpansionPortAPI) | Cartridges, REU, GeoRAM. |
| `serialPort` | [vc64::SerialPortAPI](@ref vc64::SerialPortAPI) | The serial (IEC) port. |
| `userPort` | [vc64::UserPortAPI](@ref vc64::UserPortAPI) | The user port. Its member `rs232` ([vc64::RS232API](@ref vc64::RS232API)) is the RS232 adapter. |

## Peripherals

| Member | Class | Purpose |
|:-------|:------|:--------|
| `keyboard` | [vc64::KeyboardAPI](@ref vc64::KeyboardAPI) | Key presses, auto-typing. Keys are described by [vc64::C64Key](@ref vc64::C64Key). |
| `drive8`, `drive9` | [vc64::DriveAPI](@ref vc64::DriveAPI) | The floppy drives. The member `disk` ([vc64::DiskAPI](@ref vc64::DiskAPI)) gives access to the inserted disk. |
| `datasette` | [vc64::DatasetteAPI](@ref vc64::DatasetteAPI) | The tape recorder. |

## Miscellaneous

| Member | Class | Purpose |
|:-------|:------|:--------|
| `retroShell` | [vc64::RetroShellAPI](@ref vc64::RetroShellAPI) | The command console: typing, executing scripts. |
| `remoteManager` | [vc64::RemoteManagerAPI](@ref vc64::RemoteManagerAPI) | Remote servers (RetroShell, RPC, DAP, ...). |
| `xray` | [vc64::XRayAPI](@ref vc64::XRayAPI) | The X-Ray visualizer. |
| `defaults` (static) | [vc64::DefaultsAPI](@ref vc64::DefaultsAPI) | Persistent user defaults. Available without an emulator instance. |

## Data types

Functions exchange data through plain structures and enumerations. The most important ones are:

- Configuration options: [vc64::Opt](@ref vc64::Opt)
- Messages from the emulator: [vc64::Message](@ref vc64::Message) and [vc64::Msg](@ref vc64::Msg)
- State records: for example [vc64::CPUInfo](@ref vc64::CPUInfo), [vc64::VICIIInfo](@ref vc64::VICIIInfo), [vc64::CIAInfo](@ref vc64::CIAInfo), [vc64::DriveInfo](@ref vc64::DriveInfo)
- Execution state: [vc64::ExecState](@ref vc64::ExecState)
- Snapshots and media files: [vc64::Snapshot](@ref vc64::Snapshot), [vc64::AnyFile](@ref vc64::AnyFile)

## Errors

API functions throw [vc64::CoreError](@ref vc64::CoreError), [utl::IOError](@ref utl::IOError), or [vc64::MediaError](@ref vc64::MediaError), all derived from [utl::Error](@ref utl::Error). See [Threading and Errors](threading.md).
