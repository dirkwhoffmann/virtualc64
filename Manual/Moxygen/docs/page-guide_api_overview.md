---
title: "API Overview"
kind: page
---
# API Overview {#apioverview}

This page is the entry point for looking up an API function. It explains how the public API is organized and links to all classes that make it up.

## Structure
The whole public API hangs off a single object of class [vc64::VirtualC64](vc64-VirtualC64.md#virtualc64). The class has two kinds of members:

* **Functions** that act on the emulator as a whole: querying and controlling the execution state, resetting, single-stepping, configuring, and synchronizing with the emulator thread. They are grouped into sections (*Controlling the emulator state*, *Configuring the emulator*, *Danger zone*, ...) on the class page.
* **Sub-APIs**, one per hardware component or subsystem. They are public data members, so a call reads like `c64.cpu.getInfo()` or `c64.drive8.insert(...)`. Sub-APIs may contain further sub-APIs, for example `c64.cpu.breakpoints` or `c64.controlPort1.joystick`.

All API classes are derived from [vc64::API](vc64-API.md#api), which provides [suspend()](vc64-API.md#suspend) and [resume()](vc64-API.md#resume). They all follow the same conventions for [threading and error handling](page-guide_threading.md#guide_threading_1md__2_users_2hoff_2_retro_2virtualc64_2_manual_2_doxygen_2build_2guides_2threading), which is why those rules are not repeated for each function.

Three naming patterns recur throughout the API:

Pattern  |Meaning
--------- | ---------
`getConfig()`, `get(Opt)` / `set(Opt, ...)`|Read and change the configuration. Options are identified by the enumeration [vc64::Opt](enum-Opt.md).
`getInfo()`, `getCachedInfo()`, `getMetrics()`, ...  |Read the state of a component (see [Threading and Errors](page-guide_threading.md#guide_threading_1md__2_users_2hoff_2_retro_2virtualc64_2_manual_2_doxygen_2build_2guides_2threading)).
`foo()` / `fooAsync()`|Blocking and asynchronous flavor of a state-changing function.

To be notified about events, register a message callback with [launch()](vc64-VirtualC64.md#launch). The messages are listed in the enumeration [vc64::Msg](enum-Msg.md).

## The main class

Class  |Purpose
--------- | ---------
[vc64::VirtualC64](vc64-VirtualC64.md#virtualc64)|Entry point. Execution state, resetting, configuration, synchronization, and the sub-APIs listed below.
[vc64::API](vc64-API.md#api)|Base class of all APIs (suspend and resume).

## Components

Member  |Class  |Purpose
--------- | --------- | ---------
`c64`|[vc64::C64API](vc64-C64API.md#c64api)|The machine as a whole: snapshots, workspaces, ROMs, media files, auto-inspection.
`mem`|[vc64::MemoryAPI](vc64-MemoryAPI.md#memoryapi)|Memory configuration, state, heat map. Its member `mem.debugger` ([vc64::MemoryDebuggerAPI](vc64-MemoryDebuggerAPI.md#memorydebuggerapi)) dumps memory.
`cpu`|[vc64::CPUAPI](vc64-CPUAPI.md#cpuapi)|CPU state. Its members `cpu.debugger` ([vc64::CPUDebuggerAPI](vc64-CPUDebuggerAPI.md#cpudebuggerapi)) disassembles code, `cpu.breakpoints` and `cpu.watchpoints` ([vc64::GuardsAPI](vc64-GuardsAPI.md#guardsapi)) manage guards.
`cia1`, `cia2`|[vc64::CIAAPI](vc64-CIAAPI.md#ciaapi)|The two CIA chips.
`sid`|[vc64::SIDAPI](vc64-SIDAPI.md#sidapi)|The SID chip(s).
`vicii`|[vc64::VICIIAPI](vc64-VICIIAPI.md#viciiapi)|The VICII graphics chip.

## Ports

Member  |Class  |Purpose
--------- | --------- | ---------
`audioPort`|[vc64::AudioPortAPI](vc64-AudioPortAPI.md#audioportapi)|Audio output: sample retrieval, volume, waveform drawing.
`videoPort`|[vc64::VideoPortAPI](vc64-VideoPortAPI.md#videoportapi)|Video output: textures, screenshots.
`controlPort1`, `controlPort2`|[vc64::ControlPortAPI](vc64-ControlPortAPI.md#controlportapi)|The control ports. Their members `joystick` ([vc64::JoystickAPI](vc64-JoystickAPI.md#joystickapi)), `mouse` ([vc64::MouseAPI](vc64-MouseAPI.md#mouseapi)), and `paddle` ([vc64::PaddleAPI](vc64-PaddleAPI.md#paddleapi)) control the connected device.
`expansionPort`|[vc64::ExpansionPortAPI](vc64-ExpansionPortAPI.md#expansionportapi)|Cartridges, REU, GeoRAM.
`serialPort`|[vc64::SerialPortAPI](vc64-SerialPortAPI.md#serialportapi)|The serial (IEC) port.
`userPort`|[vc64::UserPortAPI](vc64-UserPortAPI.md#userportapi)|The user port. Its member `rs232` ([vc64::RS232API](vc64-RS232API.md#rs232api)) is the RS232 adapter.

## Peripherals

Member  |Class  |Purpose
--------- | --------- | ---------
`keyboard`|[vc64::KeyboardAPI](vc64-KeyboardAPI.md#keyboardapi)|Key presses, auto-typing. Keys are described by [vc64::C64Key](vc64-C64Key.md#c64key).
`drive8`, `drive9`|[vc64::DriveAPI](vc64-DriveAPI.md#driveapi)|The floppy drives. The member `disk` ([vc64::DiskAPI](vc64-DiskAPI.md#diskapi)) gives access to the inserted disk.
`datasette`|[vc64::DatasetteAPI](vc64-DatasetteAPI.md#datasetteapi)|The tape recorder.

## Miscellaneous

Member  |Class  |Purpose
--------- | --------- | ---------
`retroShell`|[vc64::RetroShellAPI](vc64-RetroShellAPI.md#retroshellapi)|The command console: typing, executing scripts.
`remoteManager`|[vc64::RemoteManagerAPI](vc64-RemoteManagerAPI.md#remotemanagerapi)|Remote servers (RetroShell, RPC, DAP, ...).
`xray`|[vc64::XRayAPI](vc64-XRayAPI.md#xrayapi)|The X-Ray visualizer.
`defaults` (static)  |[vc64::DefaultsAPI](vc64-DefaultsAPI.md#defaultsapi)|Persistent user defaults. Available without an emulator instance.

## Data types
Functions exchange data through plain structures and enumerations. The most important ones are:

* Configuration options: [vc64::Opt](enum-Opt.md)
* Messages from the emulator: [vc64::Message](vc64-Message.md#message) and [vc64::Msg](enum-Msg.md)
* State records: for example [vc64::CPUInfo](vc64-CPUInfo.md#cpuinfo), [vc64::VICIIInfo](vc64-VICIIInfo.md#viciiinfo), [vc64::CIAInfo](vc64-CIAInfo.md#ciainfo), [vc64::DriveInfo](vc64-DriveInfo.md#driveinfo)
* Execution state: [vc64::ExecState](enum-ExecState.md)
* Snapshots and media files: [vc64::Snapshot](vc64-Snapshot.md#snapshot), [vc64::AnyFile](vc64-AnyFile.md#anyfile)

## Errors
API functions throw [vc64::CoreError](vc64-CoreError.md#coreerror), [utl::IOError](utl-IOError.md#ioerror), or [vc64::MediaError](vc64-MediaError.md#mediaerror), all derived from [utl::Error](utl-Error.md#error). See [Threading and Errors](page-guide_threading.md#guide_threading_1md__2_users_2hoff_2_retro_2virtualc64_2_manual_2_doxygen_2build_2guides_2threading).

