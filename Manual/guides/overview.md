# Overview {#api_overview}

The whole public API hangs off a single object of class [vc64::VirtualC64](@ref vc64::VirtualC64). It has two kinds of members:

- **Functions** that act on the emulator as a whole: controlling the execution state, resetting, configuring, and synchronizing with the emulator thread.
- **Sub-APIs**, one per hardware component or subsystem. They are public data members, so a call reads like `c64.cpu.getInfo()` or `c64.drive8.insert(...)`. Sub-APIs may contain further sub-APIs, for example `c64.cpu.breakpoints` or `c64.controlPort1.joystick`.

All API classes are derived from [vc64::API](@ref vc64::API), which provides [suspend()](@ref vc64::API::suspend) and [resume()](@ref vc64::API::resume).

## Sub-APIs

| Member | Class | Purpose |
|:-------|:------|:--------|
| `c64` | [vc64::C64API](@ref vc64::C64API) | The machine as a whole: snapshots, workspaces, ROMs, media files. |
| `mem` | [vc64::MemoryAPI](@ref vc64::MemoryAPI) | Memory configuration and state. |
| `cpu` | [vc64::CPUAPI](@ref vc64::CPUAPI) | CPU state, disassembler, breakpoints and watchpoints. |
| `cia1`, `cia2` | [vc64::CIAAPI](@ref vc64::CIAAPI) | The two CIA chips. |
| `sid` | [vc64::SIDAPI](@ref vc64::SIDAPI) | The SID chip(s). |
| `vicii` | [vc64::VICIIAPI](@ref vc64::VICIIAPI) | The VICII graphics chip. |
| `audioPort`, `videoPort` | [vc64::AudioPortAPI](@ref vc64::AudioPortAPI), [vc64::VideoPortAPI](@ref vc64::VideoPortAPI) | Audio and video output. |
| `controlPort1`, `controlPort2` | [vc64::ControlPortAPI](@ref vc64::ControlPortAPI) | The control ports with their joystick, mouse, and paddle. |
| `expansionPort` | [vc64::ExpansionPortAPI](@ref vc64::ExpansionPortAPI) | Cartridges, REU, GeoRAM. |
| `serialPort`, `userPort` | [vc64::SerialPortAPI](@ref vc64::SerialPortAPI), [vc64::UserPortAPI](@ref vc64::UserPortAPI) | The serial (IEC) port and the user port with its RS232 adapter. |
| `keyboard` | [vc64::KeyboardAPI](@ref vc64::KeyboardAPI) | Key presses, auto-typing. |
| `drive8`, `drive9` | [vc64::DriveAPI](@ref vc64::DriveAPI) | The floppy drives and their disks. |
| `datasette` | [vc64::DatasetteAPI](@ref vc64::DatasetteAPI) | The tape recorder. |
| `retroShell` | [vc64::RetroShellAPI](@ref vc64::RetroShellAPI) | The command console. |
| `remoteManager` | [vc64::RemoteManagerAPI](@ref vc64::RemoteManagerAPI) | Remote servers (RetroShell, RPC, DAP, ...). |
| `xray` | [vc64::XRayAPI](@ref vc64::XRayAPI) | The X-Ray visualizer. |
| `defaults` (static) | [vc64::DefaultsAPI](@ref vc64::DefaultsAPI) | Persistent user defaults. Available without an emulator instance. |

## Conventions

| Pattern | Meaning |
|:--------|:--------|
| `getConfig()`, `get(Opt)` / `set(Opt, ...)` | Read and change the configuration. Options are identified by the enumeration [vc64::Opt](@ref vc64::Opt). |
| `getInfo()`, `getCachedInfo()`, `getMetrics()`, ... | Read the state of a component. |
| `foo()` / `fooAsync()` | Blocking and asynchronous flavor of a state-changing function. |

To be notified about events, register a message callback with [launch()](@ref vc64::VirtualC64::launch). The messages are listed in the enumeration [vc64::Msg](@ref vc64::Msg). The [Architecture](architecture.md) explains which thread may call what.

## Errors

API functions report failures by throwing exceptions derived from [utl::Error](@ref utl::Error). Each function documents what it can throw with `@throw`.

| Exception | Meaning |
|:----------|:--------|
| [vc64::CoreError](@ref vc64::CoreError) | The request does not match the state of the emulator, e.g., an invalid option value or a missing ROM. |
| [utl::IOError](@ref utl::IOError) | A file or folder could not be accessed. |
| [vc64::MediaError](@ref vc64::MediaError) | A ROM, snapshot, or disk image is invalid or not supported. |

```cpp
try {
    c64.run();
} catch (const CoreError &e) {
    if (e.fault() == CoreError::ROM_KERNAL_MISSING) { /* ask the user for a ROM */ }
}
```
