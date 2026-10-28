---
title: "VirtualC64"
kind: class
---
# VirtualC64

```cpp
#include <VirtualC64.h>
```

```cpp
class VirtualC64
```

Defined in VirtualC64.h:2053

Public [API](vc64-API.md#api)

This class declares the emulator's public [API](vc64-API.md#api). It consists of functions controlling the execution state, such as running or pausing the emulator, as well as functions configuring the various components. The class contains separate sub-APIs for the subcomponents of the emulator. For example, a VICII [API](vc64-API.md#api) provides additional functions that interact directly with the VICII graphics chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`C64API`](vc64-C64API.md#c64api) | [`c64`](#c64)  | C64 (state, snapshots, media). |
| [`MemoryAPI`](vc64-MemoryAPI.md#memoryapi) | [`mem`](#mem)  | Memory. |
| [`CPUAPI`](vc64-CPUAPI.md#cpuapi) | [`cpu`](#cpu)  | CPU. |
| [`CIAAPI`](vc64-CIAAPI.md#ciaapi) | [`cia1`](#cia1)  | The two CIA chips. |
| [`SIDAPI`](vc64-SIDAPI.md#sidapi) | [`sid`](#sid)  | SID chip(s). |
| [`VICIIAPI`](vc64-VICIIAPI.md#viciiapi) | [`vicii`](#vicii)  | VICII graphics chip. |
| [`AudioPortAPI`](vc64-AudioPortAPI.md#audioportapi) | [`audioPort`](#audioport)  | Audio port. |
| [`ControlPortAPI`](vc64-ControlPortAPI.md#controlportapi) | [`controlPort1`](#controlport1)  | The two control ports. |
| [`ExpansionPortAPI`](vc64-ExpansionPortAPI.md#expansionportapi) | [`expansionPort`](#expansionport)  | Expansion port (cartridges). |
| [`SerialPortAPI`](vc64-SerialPortAPI.md#serialportapi) | [`serialPort`](#serialport)  | Serial port (IEC bus). |
| [`UserPortAPI`](vc64-UserPortAPI.md#userportapi) | [`userPort`](#userport)  | User port. |
| [`VideoPortAPI`](vc64-VideoPortAPI.md#videoportapi) | [`videoPort`](#videoport)  | Video port (textures). |
| [`DatasetteAPI`](vc64-DatasetteAPI.md#datasetteapi) | [`datasette`](#datasette)  | Datasette. |
| [`DriveAPI`](vc64-DriveAPI.md#driveapi) | [`drive8`](#drive8-1)  | Floppy drives 8 and 9. |
| [`KeyboardAPI`](vc64-KeyboardAPI.md#keyboardapi) | [`keyboard`](#keyboard)  | Keyboard. |
| [`XRayAPI`](vc64-XRayAPI.md#xrayapi) | [`xray`](#xray)  | X-Ray visualizer. |
| [`RemoteManagerAPI`](vc64-RemoteManagerAPI.md#remotemanagerapi) | [`remoteManager`](#remotemanager)  | Remote servers. |
| [`RetroShellAPI`](vc64-RetroShellAPI.md#retroshellapi) | [`retroShell`](#retroshell)  | RetroShell. |
| [`DriveAPI`](vc64-DriveAPI.md#driveapi) * | [`drive`](#drive)  | Drives by number. |

---

### c64

```cpp
C64API c64
```

Type: [`C64API`](vc64-C64API.md#c64api)

Defined in VirtualC64.h:2062

C64 (state, snapshots, media).

---

### mem

```cpp
MemoryAPI mem
```

Type: [`MemoryAPI`](vc64-MemoryAPI.md#memoryapi)

Defined in VirtualC64.h:2064

Memory.

---

### cpu

```cpp
CPUAPI cpu
```

Type: [`CPUAPI`](vc64-CPUAPI.md#cpuapi)

Defined in VirtualC64.h:2066

CPU.

---

### cia1

```cpp
CIAAPI cia1
```

Type: [`CIAAPI`](vc64-CIAAPI.md#ciaapi)

Defined in VirtualC64.h:2068

The two CIA chips.

---

### sid

```cpp
SIDAPI sid
```

Type: [`SIDAPI`](vc64-SIDAPI.md#sidapi)

Defined in VirtualC64.h:2070

SID chip(s).

---

### vicii

```cpp
VICIIAPI vicii
```

Type: [`VICIIAPI`](vc64-VICIIAPI.md#viciiapi)

Defined in VirtualC64.h:2072

VICII graphics chip.

---

### audioPort

```cpp
AudioPortAPI audioPort
```

Type: [`AudioPortAPI`](vc64-AudioPortAPI.md#audioportapi)

Defined in VirtualC64.h:2076

Audio port.

---

### controlPort1

```cpp
ControlPortAPI controlPort1
```

Type: [`ControlPortAPI`](vc64-ControlPortAPI.md#controlportapi)

Defined in VirtualC64.h:2078

The two control ports.

---

### expansionPort

```cpp
ExpansionPortAPI expansionPort
```

Type: [`ExpansionPortAPI`](vc64-ExpansionPortAPI.md#expansionportapi)

Defined in VirtualC64.h:2080

Expansion port (cartridges).

---

### serialPort

```cpp
SerialPortAPI serialPort
```

Type: [`SerialPortAPI`](vc64-SerialPortAPI.md#serialportapi)

Defined in VirtualC64.h:2082

Serial port (IEC bus).

---

### userPort

```cpp
UserPortAPI userPort
```

Type: [`UserPortAPI`](vc64-UserPortAPI.md#userportapi)

Defined in VirtualC64.h:2084

User port.

---

### videoPort

```cpp
VideoPortAPI videoPort
```

Type: [`VideoPortAPI`](vc64-VideoPortAPI.md#videoportapi)

Defined in VirtualC64.h:2086

Video port (textures).

---

### datasette

```cpp
DatasetteAPI datasette
```

Type: [`DatasetteAPI`](vc64-DatasetteAPI.md#datasetteapi)

Defined in VirtualC64.h:2090

Datasette.

---

### drive8

```cpp
DriveAPI drive8
```

Type: [`DriveAPI`](vc64-DriveAPI.md#driveapi)

Defined in VirtualC64.h:2092

Floppy drives 8 and 9.

---

### keyboard

```cpp
KeyboardAPI keyboard
```

Type: [`KeyboardAPI`](vc64-KeyboardAPI.md#keyboardapi)

Defined in VirtualC64.h:2094

Keyboard.

---

### xray

```cpp
XRayAPI xray
```

Type: [`XRayAPI`](vc64-XRayAPI.md#xrayapi)

Defined in VirtualC64.h:2098

X-Ray visualizer.

---

### remoteManager

```cpp
RemoteManagerAPI remoteManager
```

Type: [`RemoteManagerAPI`](vc64-RemoteManagerAPI.md#remotemanagerapi)

Defined in VirtualC64.h:2100

Remote servers.

---

### retroShell

```cpp
RetroShellAPI retroShell
```

Type: [`RetroShellAPI`](vc64-RetroShellAPI.md#retroshellapi)

Defined in VirtualC64.h:2102

RetroShell.

---

### drive

```cpp
DriveAPI * drive[4]
```

Defined in VirtualC64.h:2106

Drives by number.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`isPoweredOn`](#ispoweredon) `const` | Returns true iff the emulator if the emulator is powered on. |
| `bool` | [`isPoweredOff`](#ispoweredoff) `const` | Returns true iff the emulator if the emulator is powered off. |
| `bool` | [`isPaused`](#ispaused) `const` | Returns true iff the emulator is in paused state. |
| `bool` | [`isRunning`](#isrunning) `const` | Returns true iff the emulator is running. |
| `bool` | [`isSuspended`](#issuspended) `const` | Returns true iff the emulator has been suspended. |
| `bool` | [`isHalted`](#ishalted) `const` | Returns true iff the emulator has shut down. |
| `bool` | [`isWarping`](#iswarping) `const` | Returns true iff warp mode is active. |
| `bool` | [`isTracking`](#istracking) `const` | Returns true iff the emulator runs in track mode. |
| `void` | [`checkReady`](#checkready) `const` | Checks if the emulator is runnable. The function checks if the necessary ROMs are installed to launch the emulator. On success, the functions returns. Otherwise, an exception is thrown. |
| `void` | [`powerOn`](#poweron)  | Switches the emulator on. |
| `void` | [`powerOnAsync`](#poweronasync)  | Switches the emulator on without waiting for the result. |
| `void` | [`powerOff`](#poweroff)  | Switches the emulator off. |
| `void` | [`powerOffAsync`](#poweroffasync)  | Switches the emulator off without waiting for the result. |
| `void` | [`run`](#run)  | Starts emulation. |
| `void` | [`runAsync`](#runasync)  | Starts emulation without waiting for the result. |
| `void` | [`pause`](#pause)  | Pauses emulation. |
| `void` | [`pauseAsync`](#pauseasync)  | Pauses emulation without waiting for the result. |
| `void` | [`hardReset`](#hardreset)  | Performs a hard reset. |
| `void` | [`hardResetAsync`](#hardresetasync)  | Performs a hard reset without waiting for the result. |
| `void` | [`softReset`](#softreset)  | Performs a soft reset. |
| `void` | [`softResetAsync`](#softresetasync)  | Performs a soft reset without waiting for the result. |
| `void` | [`halt`](#halt)  | Terminates the emulator thread. |
| `void` | [`warpOn`](#warpon)  | Enables warp mode. |
| `void` | [`warpOnAsync`](#warponasync)  | Enables warp mode without waiting for the result. |
| `void` | [`warpOff`](#warpoff)  | Disables warp mode. |
| `void` | [`warpOffAsync`](#warpoffasync)  | Disables warp mode without waiting for the result. |
| `void` | [`trackOn`](#trackon)  | Enables track mode. |
| `void` | [`trackOnAsync`](#trackonasync)  | Enables track mode without waiting for the result. |
| `void` | [`trackOff`](#trackoff)  | Disables track mode. |
| `void` | [`trackOffAsync`](#trackoffasync)  | Disables track mode without waiting for the result. |
| `void` | [`stepInto`](#stepinto)  | Steps a single instruction. |
| `void` | [`stepOver`](#stepover)  | Steps over the current instruction. |
| `void` | [`stepCycle`](#stepcycle)  | Steps one cycle. |
| `void` | [`finishLine`](#finishline)  | Finishes the current rasterline. |
| `void` | [`finishFrame`](#finishframe)  | Finishes the current frame. |
| `void` | [`wakeUp`](#wakeup)  | Sends a wakeup signal to the emulator thread. |
| `void` | [`sync`](#sync)  | Waits until every command sent so far has been carried out. |
| `void` | [`launch`](#launch)  | Launches the emulator thread. |
| `void` | [`removeListener`](#removelistener)  | Stops delivering messages to the registered listener. |
| `bool` | [`isLaunched`](#islaunched) `const` | Returns true if the emulator has been launched. |
| `i64` | [`get`](#get-3) `const` | Queries a configuration option. |
| `i64` | [`get`](#get-4) `const` | Queries a configuration option. |
| `void` | [`set`](#set-5)  | Configures a component. |
| `void` | [`set`](#set-6)  | Configures a component. |
| `void` | [`set`](#set-7)  | Configures the emulator to match a specific C64 model |
| `void` | [`setAsync`](#setasync)  | Configures a component without waiting for the result. |
| `void` | [`setAsync`](#setasync-1)  | Configures a component without waiting for the result. |
| `void` | [`setAsync`](#setasync-2)  | Asynchronous version of [set(ConfigScheme)](#set-7): applies a predefined configuration scheme. |
| `void` | [`check`](#check)  | Checks if a configuration option would be accepted. |
| `void` | [`check`](#check-1)  | Checks if a configuration option would be accepted. |
| `void` | [`exportConfig`](#exportconfig) `const` | Exports the current configuration. |
| `void` | [`exportConfig`](#exportconfig-1) `const` | Exports the configuration to an output stream (see above). |
| `void` | [`call`](#call)  | Runs a function on the emulator thread and waits until it is done. |
| `void` | [`callAsync`](#callasync)  | Queues a function for the emulator thread without waiting for it. |
| `void` | [`put`](#put)  | Feeds a command into the command queue and waits for the result. |
| `void` | [`put`](#put-1) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-2) `inline` | Shorthand for put(Command(type, payload, payload2)). |
| `void` | [`put`](#put-3) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-4) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-5) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-6) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-7) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`put`](#put-8) `inline` | Shorthand for put(Command(type, payload)). |
| `void` | [`putAsync`](#putasync)  | Feeds a command into the command queue without waiting for the result. |
| `void` | [`putAsync`](#putasync-1) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-2) `inline` | Shorthand for putAsync(Command(type, payload, payload2)). |
| `void` | [`putAsync`](#putasync-3) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-4) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-5) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-6) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-7) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `void` | [`putAsync`](#putasync-8) `inline` | Shorthand for putAsync(Command(type, payload)). |
| `class Emulator &` | [`getEmulator`](#getemulator)  | Gives access to the internal components. |
| `class C64 &` | [`getC64`](#getc64)  | Returns the C64. |
| `class Memory &` | [`getMemory`](#getmemory)  | Returns the memory. |
| `class CPU &` | [`getCPU`](#getcpu)  | Returns the CPU. |
| `class CIA &` | [`getCIA`](#getcia)  | Returns a CIA chip (nr = 0 or 1). |
| `class VICII &` | [`getVICII`](#getvicii)  | Returns the VICII chip. |
| `class SIDBridge &` | [`getSID`](#getsid)  | Returns the SID bridge, which hosts all SID chips. |
| `class AudioPort &` | [`getAudioPort`](#getaudioport)  | Returns the audio port. |
| `class VideoPort &` | [`getVideoPort`](#getvideoport)  | Returns the video port. |
| `class XRay &` | [`getXRay`](#getxray)  | Returns the X-Ray visualizer. |
| `class Keyboard &` | [`getKeyboard`](#getkeyboard)  | Returns the keyboard. |
| `class Datasette &` | [`getDatasette`](#getdatasette)  | Returns the datasette. |
| `class ControlPort &` | [`getControlPort`](#getcontrolport)  | Returns a control port (nr = 0 or 1). |
| `class Mouse &` | [`getMouse`](#getmouse)  | Returns the mouse of a control port (port = 0 or 1). |
| `class Joystick &` | [`getJoystick`](#getjoystick)  | Returns the joystick of a control port (port = 0 or 1). |
| `class Paddle &` | [`getPaddle`](#getpaddle)  | Returns the paddle of a control port (port = 0 or 1). |
| `class UserPort &` | [`getUserPort`](#getuserport)  | Returns the user port. |
| `class RS232 &` | [`getRS232`](#getrs232)  | Returns the RS232 adapter. |
| `class ExpansionPort &` | [`getExpansionPort`](#getexpansionport)  | Returns the expansion port. |
| `class SerialPort &` | [`getSerialPort`](#getserialport)  | Returns the serial port. |
| `class Drive &` | [`getDrive`](#getdrive)  | Returns a floppy drive (nr = 0 for drive 8, 1 for drive 9). |
| `class RemoteManager &` | [`getRemoteManager`](#getremotemanager)  | Returns the remote server manager. |
| `class RetroShell &` | [`getRetroShell`](#getretroshell)  | Returns RetroShell. |
| `class Defaults &` | [`getDefaults`](#getdefaults)  | Returns the user defaults storage. |
|  | [`~VirtualC64`](#virtualc64-1)  | Halts the emulator thread and destroys the emulator. |
| const [`EmulatorInfo`](vc64-EmulatorInfo.md#emulatorinfo) & | [`getInfo`](#getinfo-15) `const` | Returns the component's current state. |
| const [`EmulatorInfo`](vc64-EmulatorInfo.md#emulatorinfo) & | [`getCachedInfo`](#getcachedinfo-15) `const` | Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-15)). |
| const [`EmulatorMetrics`](vc64-EmulatorMetrics.md#emulatormetrics) & | [`getMetrics`](#getmetrics-2) `const` | Returns statistical information about the components. |
| const [`EmulatorMetrics`](vc64-EmulatorMetrics.md#emulatormetrics) & | [`getCachedMetrics`](#getcachedmetrics-2) `const` | Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics-2)). |

---

### isPoweredOn

`const`

```cpp
bool isPoweredOn() const
```

Defined in VirtualC64.h:2180

Returns true iff the emulator if the emulator is powered on.

---

### isPoweredOff

`const`

```cpp
bool isPoweredOff() const
```

Defined in VirtualC64.h:2184

Returns true iff the emulator if the emulator is powered off.

---

### isPaused

`const`

```cpp
bool isPaused() const
```

Defined in VirtualC64.h:2188

Returns true iff the emulator is in paused state.

---

### isRunning

`const`

```cpp
bool isRunning() const
```

Defined in VirtualC64.h:2192

Returns true iff the emulator is running.

---

### isSuspended

`const`

```cpp
bool isSuspended() const
```

Defined in VirtualC64.h:2196

Returns true iff the emulator has been suspended.

---

### isHalted

`const`

```cpp
bool isHalted() const
```

Defined in VirtualC64.h:2200

Returns true iff the emulator has shut down.

---

### isWarping

`const`

```cpp
bool isWarping() const
```

Defined in VirtualC64.h:2204

Returns true iff warp mode is active.

---

### isTracking

`const`

```cpp
bool isTracking() const
```

Defined in VirtualC64.h:2208

Returns true iff the emulator runs in track mode.

---

### checkReady

`const`

```cpp
void checkReady() const
```

Defined in VirtualC64.h:2217

Checks if the emulator is runnable. The function checks if the necessary ROMs are installed to launch the emulator. On success, the functions returns. Otherwise, an exception is thrown.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | if the emulator cannot run, e.g., because a ROM is missing. |

---

### powerOn

```cpp
void powerOn()
```

Defined in VirtualC64.h:2238

Switches the emulator on.

Powering on the emulator changes the internal state to [ExecState::PAUSED](enum-ExecState.md), the same state that is entered when the user hits the pause button. Calling this function on an already powered-on emulator has no effect.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [powerOnAsync()](#poweronasync) in these situations. |

---

### powerOnAsync

```cpp
void powerOnAsync()
```

Defined in VirtualC64.h:2245

Switches the emulator on without waiting for the result.

This is the asynchronous counterpart of [powerOn()](#poweron). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

---

### powerOff

```cpp
void powerOff()
```

Defined in VirtualC64.h:2260

Switches the emulator off.

Powering off the emulator changes the internal state to [ExecState::OFF](enum-ExecState.md). Calling this function on an already powered-off emulator has no effect.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [powerOffAsync()](#poweroffasync) in these situations. |

---

### powerOffAsync

```cpp
void powerOffAsync()
```

Defined in VirtualC64.h:2267

Switches the emulator off without waiting for the result.

This is the asynchronous counterpart of [powerOff()](#poweroff). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

---

### run

```cpp
void run()
```

Defined in VirtualC64.h:2286

Starts emulation.

Running the emulator changes the internal state to [ExecState::RUNNING](enum-ExecState.md), which is the normal operation state. Frames are computed at normal pace, if warp mode is switched off, or as fast as possible, if warp mode is switched on. If this function is called for a powered-off emulator, an implicit call to [powerOn()](#poweron) will be performed.

The function returns when the emulator has been started.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [runAsync()](#runasync) in these situations. |

---

### runAsync

```cpp
void runAsync()
```

Defined in VirtualC64.h:2295

Starts emulation without waiting for the result.

This is the asynchronous counterpart of [run()](#run). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |

---

### pause

```cpp
void pause()
```

Defined in VirtualC64.h:2311

Pauses emulation.

Pausing the emulator changes the internal state from [ExecState::RUNNING](enum-ExecState.md) to [ExecState::PAUSED](enum-ExecState.md) after completing the current frame. The emulator enters a frozen state where no more frames are computed.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [pauseAsync()](#pauseasync) in these situations. |

---

### pauseAsync

```cpp
void pauseAsync()
```

Defined in VirtualC64.h:2318

Pauses emulation without waiting for the result.

This is the asynchronous counterpart of [pause()](#pause). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

---

### hardReset

```cpp
void hardReset()
```

Defined in VirtualC64.h:2337

Performs a hard reset.

A hard reset affects all components. The effect is similar to switching power off and on.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [hardResetAsync()](#hardresetasync) in these situations. |

---

### hardResetAsync

```cpp
void hardResetAsync()
```

Defined in VirtualC64.h:2344

Performs a hard reset without waiting for the result.

This is the asynchronous counterpart of [hardReset()](#hardreset). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

---

### softReset

```cpp
void softReset()
```

Defined in VirtualC64.h:2359

Performs a soft reset.

A soft reset emulates a real reset of the C64 which can be initiated via the reset line on the expansion port.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [softResetAsync()](#softresetasync) in these situations. |

---

### softResetAsync

```cpp
void softResetAsync()
```

Defined in VirtualC64.h:2366

Performs a soft reset without waiting for the result.

This is the asynchronous counterpart of [softReset()](#softreset). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

---

### halt

```cpp
void halt()
```

Defined in VirtualC64.h:2380

Terminates the emulator thread.

Halting the emulator changes the internal state to [ExecState::HALTED](enum-ExecState.md). This state is part of the shutdown procedure and never entered during normal operation.

> [!NOTE]
> The function blocks until the emulator thread has terminated. It does nothing if there is no running emulator thread (e.g., if the thread has already been halted), so it is safe to call it more than once. It throws a [CoreError](vc64-CoreError.md#coreerror) if the calling thread has suspended the emulator thread, because a suspended thread cannot terminate.

---

### warpOn

```cpp
void warpOn(isize source)
```

Defined in VirtualC64.h:2399

Enables warp mode.

Several clients can request the mode independently. Each client uses its own source, and the mode stays on as long as at least one source has requested it. Valid sources are 0 to 6.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `isize` | The client requesting the change. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [warpOnAsync()](#warponasync) in these situations. |

---

### warpOnAsync

```cpp
void warpOnAsync(isize source)
```

Defined in VirtualC64.h:2408

Enables warp mode without waiting for the result.

This is the asynchronous counterpart of [warpOn()](#warpon). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |

---

### warpOff

```cpp
void warpOff(isize source)
```

Defined in VirtualC64.h:2427

Disables warp mode.

Withdraws the request made by the given source. The mode is switched off when no other source requests it (the emulator itself may keep warp mode on to implement the warp mode option). Valid sources are 0 to 6.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `isize` | The client requesting the change. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [warpOffAsync()](#warpoffasync) in these situations. |

---

### warpOffAsync

```cpp
void warpOffAsync(isize source)
```

Defined in VirtualC64.h:2436

Disables warp mode without waiting for the result.

This is the asynchronous counterpart of [warpOff()](#warpoff). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |

---

### trackOn

```cpp
void trackOn(isize source)
```

Defined in VirtualC64.h:2455

Enables track mode.

Several clients can request the mode independently. Each client uses its own source, and the mode stays on as long as at least one source has requested it. Valid sources are 0 to 6.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `isize` | The client requesting the change. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [trackOnAsync()](#trackonasync) in these situations. |

---

### trackOnAsync

```cpp
void trackOnAsync(isize source)
```

Defined in VirtualC64.h:2464

Enables track mode without waiting for the result.

This is the asynchronous counterpart of [trackOn()](#trackon). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |

---

### trackOff

```cpp
void trackOff(isize source)
```

Defined in VirtualC64.h:2482

Disables track mode.

Withdraws the request made by the given source. The mode is switched off when no other source requests it. Valid sources are 0 to 6.

The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `isize` | The client requesting the change. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [trackOffAsync()](#trackoffasync) in these situations. |

---

### trackOffAsync

```cpp
void trackOffAsync(isize source)
```

Defined in VirtualC64.h:2491

Disables track mode without waiting for the result.

This is the asynchronous counterpart of [trackOff()](#trackoff). The request is queued and carried out by the emulator thread at some later time (see [sync()](#sync)).

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the source is out of range. |

---

### stepInto

```cpp
void stepInto()
```

Defined in VirtualC64.h:2512

Steps a single instruction.

Steps the CPU for a single instruction. The emulator must not be running. The CPU debugger utilizes this function to implement single stepping.

The function returns when the emulator has been started for the step. It is paused again when the step is done.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::RUNNING](vc64-CoreError.md#running-1)) if the emulator is already running. |
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### stepOver

```cpp
void stepOver()
```

Defined in VirtualC64.h:2539

Steps over the current instruction.

Runs the emulator until the instruction following the current instruction has been reached. The emulator must not be running. The CPU debugger utilizes this function to implement single stepping.

[stepOver()](#stepover) acts like [stepInto()](#stepinto) except for branching instructions. For instance, if the current instruction is a `JSR` instruction, the subroutine will be executed in a single chunk.

> [!NOTE]
> If the current instruction is a branch instruction which never returns, the emulator will remain in running state. Calling this function will then be equivalent to a calling [run()](#run), with the exception of a small runtime overhead since the emulator will check after the each instruction if the program counter has reached its target location.

The function returns when the emulator has been started for the step. It is paused again when the step is done.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::RUNNING](vc64-CoreError.md#running-1)) if the emulator is already running. |
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### stepCycle

```cpp
void stepCycle()
```

Defined in VirtualC64.h:2553

Steps one cycle.

Calling this function runs the CPU for a single cycle.

The function returns when the emulator has been started for the step. It is paused again when the step is done.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::RUNNING](vc64-CoreError.md#running-1)) if the emulator is already running. |
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### finishLine

```cpp
void finishLine()
```

Defined in VirtualC64.h:2567

Finishes the current rasterline.

Calling this function runs the CPU until the end of the current rasterline.

The function returns when the emulator has been started for the step. It is paused again when the step is done.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::RUNNING](vc64-CoreError.md#running-1)) if the emulator is already running. |
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### finishFrame

```cpp
void finishFrame()
```

Defined in VirtualC64.h:2581

Finishes the current frame.

Calling this function runs the CPU until the end of the current frame.

The function returns when the emulator has been started for the step. It is paused again when the step is done.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::RUNNING](vc64-CoreError.md#running-1)) if the emulator is already running. |
| `CoreError` | if the emulator is not ready to run (see [checkReady()](#checkready)). |
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### wakeUp

```cpp
void wakeUp()
```

Defined in VirtualC64.h:2596

Sends a wakeup signal to the emulator thread.

To compute frames at the proper pace, the emulator core expects the GUI to send a wakeup signal on each VSYNC pulse. Once this signal is received, the emulator thread starts computing all missing frames. To minimize jitter, the wakeup signal should be sent right after the current texture has been handed over to the GPU.

---

### sync

```cpp
void sync()
```

Defined in VirtualC64.h:2610

Waits until every command sent so far has been carried out.

Most calls on this [API](vc64-API.md#api) hand the emulator thread a command and return at once, so a caller that reads back what it just changed, or exports the configuration, can still see the old state. Calling this afterwards closes that gap.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out commands, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. |

---

### launch

```cpp
void launch(const void * listener = nullptr, Callback * func = nullptr)
```

Defined in VirtualC64.h:2641

Launches the emulator thread.

This function must be called in the initialization procedure of the emulator. It launches the emulator thread and registers a callback function to the message queue. The callback function is a message receiver processing all incoming messages from the emulator.

> [!NOTE]
> All functions of this [API](vc64-API.md#api) must be called from a user thread, i.e., from any thread except the emulator thread. In particular, the callback registered here is invoked on the emulator thread and must not call into the [API](vc64-API.md#api). Forward the message to a user thread instead.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `listener` | `const void *` | An arbitrary pointer which will be passed to the registered callback function as first argument. You can use the argument to pass the address of an object into the callback function. Inside the callback you can utilize the pointer to invoke a message processing function on that object. |
| `func` | [`Callback`](vc64.md#callback) * | The callback function. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::ALREADY_LAUNCHED](vc64-CoreError.md#already_launched)) if the emulator thread has already been launched. |
| `CoreError` | ([CoreError::LAUNCH_FAILED](vc64-CoreError.md#launch_failed)) if the emulator thread could not be launched. |

---

### removeListener

```cpp
void removeListener()
```

Defined in VirtualC64.h:2651

Stops delivering messages to the registered listener.

The emulator outlives its client whenever the client is a stack or static object torn down at exit: the core keeps running, keeps posting, and calls into whatever the listener pointer used to be. Call this before the client becomes unusable. It is synchronised against message delivery, so it is safe while the emulator runs.

---

### isLaunched

`const`

```cpp
bool isLaunched() const
```

Defined in VirtualC64.h:2655

Returns true if the emulator has been launched.

---

### get

`const`

```cpp
i64 get(Opt opt) const
```

Defined in VirtualC64.h:2671

Queries a configuration option.

This is the main function to query a configuration option.

> [!NOTE]
> Some options require an additional parameter to uniquely determine the configured component. For those options, this function must not be called.

> [!NOTE]
> The value is read without synchronizing with the emulator thread. A change requested by [setAsync()](#setasync) shows up once the emulator thread has carried it out (see [sync()](#sync)).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to query |

---

### get

`const`

```cpp
i64 get(Opt opt, isize id) const
```

Defined in VirtualC64.h:2687

Queries a configuration option.

This is the main function to query a configuration option.

> [!NOTE]
> This function must only be called for those options that require an additional parameter to uniquely determine the configured component.

> [!NOTE]
> The value is read without synchronizing with the emulator thread. A change requested by [setAsync()](#setasync) shows up once the emulator thread has carried it out (see [sync()](#sync)).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to query |
| `id` | `isize` | The component to query |

---

### set

```cpp
void set(Opt opt, i64 value)
```

Defined in VirtualC64.h:2709

Configures a component.

This is the main function to set a configuration option. It returns when the change has been made, so the new value is visible to [get()](#get-3) right away. Because the command queue is processed in order, all requests issued earlier (e.g., via [setAsync()](#setasync)) have been carried out as well. A rejected value throws the corresponding exception.

> [!NOTE]
> If this function is called for an options that applies to multiple components, all components are configured with the specified value.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to set |
| `value` | `i64` | The option's value |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [setAsync()](#setasync) in these situations (it can also be called before the emulator thread is launched, in which case the request is carried out at launch). |

---

### set

```cpp
void set(Opt opt, i64 value, isize id)
```

Defined in VirtualC64.h:2721

Configures a component.

> [!NOTE]
> This function must only be called for those options that require an additional parameter to uniquely determine the configured component.

**See also**: [set(Opt, i64)](#set-5)

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to set |
| `value` | `i64` | The option's value |
| `id` | `isize` | The component to configure |

---

### set

```cpp
void set(ConfigScheme model)
```

Defined in VirtualC64.h:2728

Configures the emulator to match a specific C64 model

**See also**: [set(Opt, i64)](#set-5)

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `model` | [`ConfigScheme`](enum-ConfigScheme.md) | The C64 model to emulate |

---

### setAsync

```cpp
void setAsync(Opt opt, i64 value)
```

Defined in VirtualC64.h:2746

Configures a component without waiting for the result.

These functions hand the request to the emulator thread and return at once. They are the asynchronous counterparts of [set()](#set-5). Use them if waiting is impossible or undesirable, e.g., when many options are changed in a row (follow up with [sync()](#sync) if needed), before the emulator thread has been launched (the request is carried out at launch), or while the emulator thread is suspended (it is carried out when the thread resumes).

The value is validated when the emulator thread carries out the command. A rejected value is reported by [Msg::CONFIG](enum-Msg.md), which then carries the exception.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to set |
| `value` | `i64` | The option's value |

---

### setAsync

```cpp
void setAsync(Opt opt, i64 value, isize id)
```

Defined in VirtualC64.h:2757

Configures a component without waiting for the result.

Same as above, for options that require an additional parameter to determine the component.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to set |
| `value` | `i64` | The option's value |
| `id` | `isize` | The component to configure |

---

### setAsync

```cpp
void setAsync(ConfigScheme model)
```

Defined in VirtualC64.h:2759

Asynchronous version of [set(ConfigScheme)](#set-7): applies a predefined configuration scheme.

---

### check

```cpp
void check(Opt opt, i64 value)
```

Defined in VirtualC64.h:2771

Checks if a configuration option would be accepted.

The check is performed in the calling thread and does not change the configuration. It is not needed before [set()](#set-5), which validates the value itself, but it lets a caller skip a request that is known to fail.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to check |
| `value` | `i64` | The option's value |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | if the value would be rejected |

---

### check

```cpp
void check(Opt opt, i64 value, isize id)
```

Defined in VirtualC64.h:2779

Checks if a configuration option would be accepted.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `opt` | [`Opt`](enum-Opt.md) | The option to check |
| `value` | `i64` | The option's value |
| `id` | `isize` | The component to configure |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | if the value would be rejected |

---

### exportConfig

`const`

```cpp
void exportConfig(const std::filesystem::path & path, bool diff = false) const
```

Defined in VirtualC64.h:2796

Exports the current configuration.

The current configuration is exported in form of a RetroShell script. Reading in the script at a later point will restore the configuration.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::filesystem::path &` | The file the script is written to (or use the overload taking an output stream). |
| `diff` | `bool` | When set to true, the script will only contains settings for items that differ from the default configuration. This feature is useful for debugging to compare two virtual machine configurations. |

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `Error` | if the file cannot be written. |

---

### exportConfig

`const`

```cpp
void exportConfig(std::ostream & stream, bool diff = false) const
```

Defined in VirtualC64.h:2798

Exports the configuration to an output stream (see above).

---

### call

```cpp
void call(const std::function< void()> & fn)
```

Defined in VirtualC64.h:2836

Runs a function on the emulator thread and waits until it is done.

> [!WARNING]
> This is an advanced interface. It bypasses the dedicated [API](vc64-API.md#api) functions, and what it touches is not covered by any compatibility promise.

The function is carried out in the order of the commands that have been queued before, and never while a frame is computed or while another thread has suspended the emulator. An exception thrown by the function is passed on to the caller.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the request (see [put()](#put)). |

---

### callAsync

```cpp
void callAsync(std::function< void()> fn, std::function< void(std::exception_ptr)> done = nullptr)
```

Defined in VirtualC64.h:2851

Queues a function for the emulator thread without waiting for it.

> [!WARNING]
> This is an advanced interface. It bypasses the dedicated [API](vc64-API.md#api) functions, and what it touches is not covered by any compatibility promise.

This is the asynchronous counterpart of [call()](#call). The function is carried out by the emulator thread at some later time, so everything it refers to must stay valid until then. A failure of the function cannot be passed on to the caller. If 'done' is given, it is called on the emulator thread when the function has been carried out, with the exception it threw (or null). It is also called, on the calling thread, if the command queue is full and the request is dropped. Nothing is carried out as long as the emulator thread has not been launched.

---

### put

```cpp
void put(const Command & cmd, const std::string & str = "")
```

Defined in VirtualC64.h:2874

Feeds a command into the command queue and waits for the result.

> [!WARNING]
> This is an advanced interface. It bypasses the dedicated [API](vc64-API.md#api) functions, and what it touches is not covered by any compatibility promise.

The function returns when the emulator thread has carried out the command. A command that fails throws the corresponding exception.

Because the queue is processed in order, all commands issued earlier (e.g., via [putAsync()](#putasync)) have been carried out when the call returns. Commands that carry a pointer to an object of the caller can rely on that object being alive for the whole duration of the command.

#### Exceptions

| Exception | Description |
|-----------|-------------|
| `CoreError` | ([CoreError::NOT_LAUNCHED](vc64-CoreError.md#not_launched)) if the emulator thread cannot carry out the command, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use [putAsync()](#putasync) in these situations (it can also be called before the emulator thread is launched, in which case the command is carried out at launch), and for events that occur at a high rate (e.g., key presses or mouse movements). |

---

### put

`inline`

```cpp
inline void put(Cmd type, const std::string & str)
```

Defined in VirtualC64.h:2876

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, i64 payload = 0, i64 payload2 = 0)
```

Defined in VirtualC64.h:2878

Shorthand for put(Command(type, payload, payload2)).

---

### put

`inline`

```cpp
inline void put(Cmd type, ConfigCmd payload)
```

Defined in VirtualC64.h:2880

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, KeyCmd payload)
```

Defined in VirtualC64.h:2882

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, CoordCmd payload)
```

Defined in VirtualC64.h:2884

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, GamePadCmd payload)
```

Defined in VirtualC64.h:2886

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, TapeCmd payload)
```

Defined in VirtualC64.h:2888

Shorthand for put(Command(type, payload)).

---

### put

`inline`

```cpp
inline void put(Cmd type, AlarmCmd payload)
```

Defined in VirtualC64.h:2890

Shorthand for put(Command(type, payload)).

---

### putAsync

```cpp
void putAsync(const Command & cmd, const std::string & str = "")
```

Defined in VirtualC64.h:2900

Feeds a command into the command queue without waiting for the result.

> [!WARNING]
> This is an advanced interface, see [put()](#put).

This is the asynchronous counterpart of [put()](#put). The command is carried out by the emulator thread at some later time. The caller must keep objects referenced by the command alive until it has been carried out (see [sync()](#sync)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, const std::string & str)
```

Defined in VirtualC64.h:2902

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, i64 payload = 0, i64 payload2 = 0)
```

Defined in VirtualC64.h:2904

Shorthand for putAsync(Command(type, payload, payload2)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, ConfigCmd payload)
```

Defined in VirtualC64.h:2906

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, KeyCmd payload)
```

Defined in VirtualC64.h:2908

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, CoordCmd payload)
```

Defined in VirtualC64.h:2910

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, GamePadCmd payload)
```

Defined in VirtualC64.h:2912

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, TapeCmd payload)
```

Defined in VirtualC64.h:2914

Shorthand for putAsync(Command(type, payload)).

---

### putAsync

`inline`

```cpp
inline void putAsync(Cmd type, AlarmCmd payload)
```

Defined in VirtualC64.h:2916

Shorthand for putAsync(Command(type, payload)).

---

### getEmulator

```cpp
class Emulator & getEmulator()
```

Defined in VirtualC64.h:2930

Gives access to the internal components.

> [!WARNING]
> These are advanced interfaces. They return the internal objects the APIs operate on, bypassing all synchronization the dedicated [API](vc64-API.md#api) functions provide. Use them only while the emulator is suspended or from within a function passed to [call()](#call). To make use of the objects, include the corresponding internal headers. What they offer is not covered by any compatibility promise.

The getters that take a number throw a [CoreError](vc64-CoreError.md#coreerror) ([CoreError::OPT_INV_ARG](vc64-CoreError.md#opt_inv_arg)) if the number is out of range.

---

### getC64

```cpp
class C64 & getC64()
```

Defined in VirtualC64.h:2932

Returns the C64.

---

### getMemory

```cpp
class Memory & getMemory()
```

Defined in VirtualC64.h:2934

Returns the memory.

---

### getCPU

```cpp
class CPU & getCPU()
```

Defined in VirtualC64.h:2936

Returns the CPU.

---

### getCIA

```cpp
class CIA & getCIA(isize nr)
```

Defined in VirtualC64.h:2938

Returns a CIA chip (nr = 0 or 1).

---

### getVICII

```cpp
class VICII & getVICII()
```

Defined in VirtualC64.h:2940

Returns the VICII chip.

---

### getSID

```cpp
class SIDBridge & getSID()
```

Defined in VirtualC64.h:2942

Returns the SID bridge, which hosts all SID chips.

---

### getAudioPort

```cpp
class AudioPort & getAudioPort()
```

Defined in VirtualC64.h:2944

Returns the audio port.

---

### getVideoPort

```cpp
class VideoPort & getVideoPort()
```

Defined in VirtualC64.h:2946

Returns the video port.

---

### getXRay

```cpp
class XRay & getXRay()
```

Defined in VirtualC64.h:2948

Returns the X-Ray visualizer.

---

### getKeyboard

```cpp
class Keyboard & getKeyboard()
```

Defined in VirtualC64.h:2950

Returns the keyboard.

---

### getDatasette

```cpp
class Datasette & getDatasette()
```

Defined in VirtualC64.h:2952

Returns the datasette.

---

### getControlPort

```cpp
class ControlPort & getControlPort(isize nr)
```

Defined in VirtualC64.h:2954

Returns a control port (nr = 0 or 1).

---

### getMouse

```cpp
class Mouse & getMouse(isize port)
```

Defined in VirtualC64.h:2956

Returns the mouse of a control port (port = 0 or 1).

---

### getJoystick

```cpp
class Joystick & getJoystick(isize port)
```

Defined in VirtualC64.h:2958

Returns the joystick of a control port (port = 0 or 1).

---

### getPaddle

```cpp
class Paddle & getPaddle(isize port)
```

Defined in VirtualC64.h:2960

Returns the paddle of a control port (port = 0 or 1).

---

### getUserPort

```cpp
class UserPort & getUserPort()
```

Defined in VirtualC64.h:2962

Returns the user port.

---

### getRS232

```cpp
class RS232 & getRS232()
```

Defined in VirtualC64.h:2964

Returns the RS232 adapter.

---

### getExpansionPort

```cpp
class ExpansionPort & getExpansionPort()
```

Defined in VirtualC64.h:2966

Returns the expansion port.

---

### getSerialPort

```cpp
class SerialPort & getSerialPort()
```

Defined in VirtualC64.h:2968

Returns the serial port.

---

### getDrive

```cpp
class Drive & getDrive(isize nr)
```

Defined in VirtualC64.h:2970

Returns a floppy drive (nr = 0 for drive 8, 1 for drive 9).

---

### getRemoteManager

```cpp
class RemoteManager & getRemoteManager()
```

Defined in VirtualC64.h:2972

Returns the remote server manager.

---

### getRetroShell

```cpp
class RetroShell & getRetroShell()
```

Defined in VirtualC64.h:2974

Returns RetroShell.

---

### getDefaults

```cpp
class Defaults & getDefaults()
```

Defined in VirtualC64.h:2976

Returns the user defaults storage.

---

### ~VirtualC64

```cpp
~VirtualC64()
```

Defined in VirtualC64.h:2136

Halts the emulator thread and destroys the emulator.

A suspension that the calling thread still holds is released first, because a suspended emulator thread cannot be halted.

---

### getInfo

`const`

```cpp
const EmulatorInfo & getInfo() const
```

Defined in VirtualC64.h:2152

Returns the component's current state.

[getInfo()](#getinfo-15) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedInfo()](#getcachedinfo-15) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedInfo

`const`

```cpp
const EmulatorInfo & getCachedInfo() const
```

Defined in VirtualC64.h:2154

Returns the most recent state snapshot recorded by the emulator thread (see [getInfo()](#getinfo-15)).

---

### getMetrics

`const`

```cpp
const EmulatorMetrics & getMetrics() const
```

Defined in VirtualC64.h:2165

Returns statistical information about the components.

[getMetrics()](#getmetrics-2) reads the live state on the calling thread. The result is only consistent if the emulator is paused or suspended. [getCachedMetrics()](#getcachedmetrics-2) returns the snapshot the emulator thread has recorded most recently, which is the one to use while the emulator runs. Both return references that can be overwritten by a later call or by the emulator thread, so copy the data if you need to keep it.

---

### getCachedMetrics

`const`

```cpp
const EmulatorMetrics & getCachedMetrics() const
```

Defined in VirtualC64.h:2167

Returns the most recent metrics snapshot recorded by the emulator thread (see [getMetrics()](#getmetrics-2)).

## Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`DefaultsAPI`](vc64-DefaultsAPI.md#defaultsapi) | [`defaults`](#defaults) `static` | User defaults storage. |

---

### defaults

`static`

```cpp
DefaultsAPI defaults
```

Type: [`DefaultsAPI`](vc64-DefaultsAPI.md#defaultsapi)

Defined in VirtualC64.h:2058

User defaults storage.

## Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `string` | [`version`](#version) `static` | Returns a version string for this release. |
| `string` | [`snapshotVersion`](#snapshotversion) `static` | Returns a version string for the supported snapshot format. |
| `string` | [`build`](#build) `static` | Returns a build-number string for this release. |

---

### version

`static`

```cpp
static string version()
```

Defined in VirtualC64.h:2115

Returns a version string for this release.

---

### snapshotVersion

`static`

```cpp
static string snapshotVersion()
```

Defined in VirtualC64.h:2119

Returns a version string for the supported snapshot format.

---

### build

`static`

```cpp
static string build()
```

Defined in VirtualC64.h:2123

Returns a build-number string for this release.

