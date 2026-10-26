# API Overview {#api}

This page is the entry point for looking up an API function. It explains how the public API is organized and links to all classes that make it up.

## Structure

The whole public API hangs off a single object of class \ref vc64::VirtualC64. The class has two kinds of members:

- **Functions** that act on the emulator as a whole: querying and controlling the execution state, resetting, single-stepping, configuring, and synchronizing with the emulator thread. They are grouped into sections (*Controlling the emulator state*, *Configuring the emulator*, *Danger zone*, ...) on the class page.
- **Sub-APIs**, one per hardware component or subsystem. They are public data members, so a call reads like `c64.cpu.getInfo()` or `c64.drive8.insert(...)`. Sub-APIs may contain further sub-APIs, for example `c64.cpu.breakpoints` or `c64.controlPort1.joystick`.

All API classes are derived from \ref vc64::API, which provides \ref vc64::API::suspend "suspend()" and \ref vc64::API::resume "resume()". They all follow the same conventions for [threading and error handling](threading.md), which is why those rules are not repeated for each function.

Three naming patterns recur throughout the API:

| Pattern | Meaning |
|:--------|:--------|
| `getConfig()`, `get(Opt)` / `set(Opt, ...)` | Read and change the configuration. Options are identified by the enumeration \ref vc64::Opt. |
| `getInfo()`, `getCachedInfo()`, `getMetrics()`, ... | Read the state of a component (see [Threading and Errors](threading.md)). |
| `foo()` / `fooAsync()` | Blocking and asynchronous flavor of a state-changing function. |

To be notified about events, register a message callback with \ref vc64::VirtualC64::launch "launch()". The messages are listed in the enumeration \ref vc64::Msg.

## The main class

| Class | Purpose |
|:------|:--------|
| \ref vc64::VirtualC64 | Entry point. Execution state, resetting, configuration, synchronization, and the sub-APIs listed below. |
| \ref vc64::API | Base class of all APIs (suspend and resume). |

## Components

| Member | Class | Purpose |
|:-------|:------|:--------|
| `c64` | \ref vc64::C64API | The machine as a whole: snapshots, workspaces, ROMs, media files, auto-inspection. |
| `mem` | \ref vc64::MemoryAPI | Memory configuration, state, heat map. Its member `mem.debugger` (\ref vc64::MemoryDebuggerAPI) dumps memory. |
| `cpu` | \ref vc64::CPUAPI | CPU state. Its members `cpu.debugger` (\ref vc64::CPUDebuggerAPI) disassembles code, `cpu.breakpoints` and `cpu.watchpoints` (\ref vc64::GuardsAPI) manage guards. |
| `cia1`, `cia2` | \ref vc64::CIAAPI | The two CIA chips. |
| `sid` | \ref vc64::SIDAPI | The SID chip(s). |
| `vicii` | \ref vc64::VICIIAPI | The VICII graphics chip. |

## Ports

| Member | Class | Purpose |
|:-------|:------|:--------|
| `audioPort` | \ref vc64::AudioPortAPI | Audio output: sample retrieval, volume, waveform drawing. |
| `videoPort` | \ref vc64::VideoPortAPI | Video output: textures, screenshots. |
| `controlPort1`, `controlPort2` | \ref vc64::ControlPortAPI | The control ports. Their members `joystick` (\ref vc64::JoystickAPI), `mouse` (\ref vc64::MouseAPI), and `paddle` (\ref vc64::PaddleAPI) control the connected device. |
| `expansionPort` | \ref vc64::ExpansionPortAPI | Cartridges, REU, GeoRAM. |
| `serialPort` | \ref vc64::SerialPortAPI | The serial (IEC) port. |
| `userPort` | \ref vc64::UserPortAPI | The user port. Its member `rs232` (\ref vc64::RS232API) is the RS232 adapter. |

## Peripherals

| Member | Class | Purpose |
|:-------|:------|:--------|
| `keyboard` | \ref vc64::KeyboardAPI | Key presses, auto-typing. Keys are described by \ref vc64::C64Key. |
| `drive8`, `drive9` | \ref vc64::DriveAPI | The floppy drives. The member `disk` (\ref vc64::DiskAPI) gives access to the inserted disk. |
| `datasette` | \ref vc64::DatasetteAPI | The tape recorder. |

## Miscellaneous

| Member | Class | Purpose |
|:-------|:------|:--------|
| `retroShell` | \ref vc64::RetroShellAPI | The command console: typing, executing scripts. |
| `remoteManager` | \ref vc64::RemoteManagerAPI | Remote servers (RetroShell, RPC, DAP, ...). |
| `xray` | \ref vc64::XRayAPI | The X-Ray visualizer. |
| `defaults` (static) | \ref vc64::DefaultsAPI | Persistent user defaults. Available without an emulator instance. |

## Data types

Functions exchange data through plain structures and enumerations. The most important ones are:

- Configuration options: \ref vc64::Opt
- Messages from the emulator: \ref vc64::Message and \ref vc64::Msg
- State records: for example \ref vc64::CPUInfo, \ref vc64::VICIIInfo, \ref vc64::CIAInfo, \ref vc64::DriveInfo
- Execution state: \ref vc64::ExecState
- Snapshots and media files: \ref vc64::Snapshot, \ref vc64::AnyFile

## Errors

API functions throw \ref vc64::CoreError, \ref utl::IOError, or \ref vc64::MediaError, all derived from \ref utl::Error. See [Threading and Errors](threading.md).

<div class="section_buttons">

| Previous                    |                       Next |
|:----------------------------|---------------------------:|
| [Home](home.md)             | [Threading and Errors](threading.md) |

</div>
