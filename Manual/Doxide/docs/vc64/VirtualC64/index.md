---
generator: doxide
---


# VirtualC64

**`class VirtualC64 : public API`**

Public API

This class declares the emulator's public API. It consists of functions
controlling the execution state, such as running or pausing the emulator, as
well as functions configuring the various components. The class contains
separate sub-APIs for the subcomponents of the emulator. For example, a
VICII API provides additional functions that interact directly with the
VICII graphics chip.


## Types

| Name | Description |
| ---- | ----------- |
| [Emulator](Emulator/) |  Gives access to the internal components !!! warning These are advanced interfaces. They return the internal objects the APIs operate on, bypassing all synchronization the dedicated API functions provide. Use them only while the emulator is suspended or from within a function passed to call(). To make use of the objects, include the corresponding internal headers. What they offer is not covered by any compatibility promise. The getters that take a number throw a CoreError (CoreError::OPT_INV_ARG) if the number is out of range.  |
| [C64](C64/) | Returns the C64. |
| [Memory](Memory/) | Returns the memory. |
| [CPU](CPU/) | Returns the CPU. |
| [CIA](CIA/) | Returns a CIA chip (nr = 0 or 1). |
| [VICII](VICII/) | Returns the VICII chip. |
| [SIDBridge](SIDBridge/) | Returns the SID bridge, which hosts all SID chips. |
| [AudioPort](AudioPort/) | Returns the audio port. |
| [VideoPort](VideoPort/) | Returns the video port. |
| [XRay](XRay/) | Returns the X-Ray visualizer. |
| [Keyboard](Keyboard/) | Returns the keyboard. |
| [Datasette](Datasette/) | Returns the datasette. |
| [ControlPort](ControlPort/) | Returns a control port (nr = 0 or 1). |
| [Mouse](Mouse/) | Returns the mouse of a control port (port = 0 or 1). |
| [Joystick](Joystick/) | Returns the joystick of a control port (port = 0 or 1). |
| [Paddle](Paddle/) | Returns the paddle of a control port (port = 0 or 1). |
| [UserPort](UserPort/) | Returns the user port. |
| [RS232](RS232/) | Returns the RS232 adapter. |
| [ExpansionPort](ExpansionPort/) | Returns the expansion port. |
| [SerialPort](SerialPort/) | Returns the serial port. |
| [Drive](Drive/) | Returns a floppy drive (nr = 0 for drive 8, 1 for drive 9). |
| [RemoteManager](RemoteManager/) | Returns the remote server manager. |
| [RetroShell](RetroShell/) | Returns RetroShell. |
| [Defaults](Defaults/) | Returns the user defaults storage. |

## Variables

| Name | Description |
| ---- | ----------- |
| [defaults](#defaults) | User defaults storage  |
| [c64](#c64) | C64 (state, snapshots, media)  |
| [mem](#mem) | Memory  |
| [cpu](#cpu) | CPU  |
| [cia1](#cia1) | The two CIA chips  |
| [sid](#sid) | SID chip(s)  |
| [vicii](#vicii) | VICII graphics chip  |
| [audioPort](#audioPort) | Audio port  |
| [controlPort1](#controlPort1) | The two control ports  |
| [expansionPort](#expansionPort) | Expansion port (cartridges)  |
| [serialPort](#serialPort) | Serial port (IEC bus)  |
| [userPort](#userPort) | User port  |
| [videoPort](#videoPort) | Video port (textures)  |
| [datasette](#datasette) | Datasette  |
| [drive8](#drive8) | Floppy drives 8 and 9  |
| [keyboard](#keyboard) | Keyboard  |
| [xray](#xray) | X-Ray visualizer  |
| [remoteManager](#remoteManager) | Remote servers  |
| [retroShell](#retroShell) | RetroShell  |

## Functions

| Name | Description |
| ---- | ----------- |
| [version](#version) | Drives by number Returns a version string for this release. |
| [snapshotVersion](#snapshotVersion) |  Returns a version string for the supported snapshot format. |
| [build](#build) |  Returns a build-number string for this release. |
| [~VirtualC64](#_u007eVirtualC64) |  Halts the emulator thread and destroys the emulator A suspension that the calling thread still holds is released first, because a suspended emulator thread cannot be halted. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [getMetrics](#getMetrics) |  Returns statistical information about the components. |
| [getCachedMetrics](#getCachedMetrics) | Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()). |
| [isPoweredOn](#isPoweredOn) | @} @name Querying the emulator state @{ Returns true iff the emulator if the emulator is powered on. |
| [isPoweredOff](#isPoweredOff) |  Returns true iff the emulator if the emulator is powered off. |
| [isPaused](#isPaused) |  Returns true iff the emulator is in paused state. |
| [isRunning](#isRunning) |  Returns true iff the emulator is running. |
| [isSuspended](#isSuspended) |  Returns true iff the emulator has been suspended. |
| [isHalted](#isHalted) |  Returns true iff the emulator has shut down. |
| [isWarping](#isWarping) |  Returns true iff warp mode is active. |
| [isTracking](#isTracking) |  Returns true iff the emulator runs in track mode. |
| [checkReady](#checkReady) |  Checks if the emulator is runnable. |
| [powerOn](#powerOn) | @} @name Controlling the emulator state @{ Switches the emulator on Powering on the emulator changes the internal state to ExecState::PAUSED, the same state that is entered when the user hits the pause button. Calling this function on an already powered-on emulator has no effect. The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception. :material-alert-circle-outline: **Throw** :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use powerOnAsync() in these situations.  |
| [powerOnAsync](#powerOnAsync) |  Switches the emulator on without waiting for the result This is the asynchronous counterpart of powerOn(). |
| [powerOff](#powerOff) |  Switches the emulator off Powering off the emulator changes the internal state to ExecState::OFF. Calling this function on an already powered-off emulator has no effect. The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception. :material-alert-circle-outline: **Throw** :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use powerOffAsync() in these situations.  |
| [powerOffAsync](#powerOffAsync) |  Switches the emulator off without waiting for the result This is the asynchronous counterpart of powerOff(). |
| [run](#run) |  Starts emulation Running the emulator changes the internal state to ExecState::RUNNING, which is the normal operation state. Frames are computed at normal pace, if warp mode is switched off, or as fast as possible, if warp mode is switched on. If this function is called for a powered-off emulator, an implicit call to powerOn() will be performed. :material-alert-circle-outline: **Throw** :    CoreError if the emulator is not ready to run (see checkReady()). The function returns when the emulator has been started. :material-alert-circle-outline: **Throw** :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use runAsync() in these situations.  |
| [runAsync](#runAsync) |  Starts emulation without waiting for the result This is the asynchronous counterpart of run(). |
| [pause](#pause) |   Pauses emulation Pausing the emulator changes the internal state from ExecState::RUNNING to ExecState::PAUSED after completing the current frame. The emulator enters a frozen state where no more frames are computed. The function returns when the emulator thread has carried out the request. A failure throws the corresponding exception. :material-alert-circle-outline: **Throw** :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot carry out the request, because it has not been launched yet, is halted or suspended. The same applies if the function is called from the emulator thread. Use pauseAsync() in these situations.  |
| [pauseAsync](#pauseAsync) |  Pauses emulation without waiting for the result This is the asynchronous counterpart of pause(). |
| [hardReset](#hardReset) | @} @name Resetting the C64 @{ Performs a hard reset A hard reset affects all components. |
| [hardResetAsync](#hardResetAsync) |  Performs a hard reset without waiting for the result This is the asynchronous counterpart of hardReset(). |
| [softReset](#softReset) |  Performs a soft reset A soft reset emulates a real reset of the C64 which can be initiated via the reset line on the expansion port. |
| [softResetAsync](#softResetAsync) |  Performs a soft reset without waiting for the result This is the asynchronous counterpart of softReset(). |
| [halt](#halt) |   Terminates the emulator thread Halting the emulator changes the internal state to ExecState::HALTED. This state is part of the shutdown procedure and never entered during normal operation. !!! note The function blocks until the emulator thread has terminated. It does nothing if there is no running emulator thread (e.g., if the thread has already been halted), so it is safe to call it more than once. It throws a CoreError if the calling thread has suspended the emulator thread, because a suspended thread cannot terminate.  |
| [warpOn](#warpOn) |  Enables warp mode. |
| [warpOnAsync](#warpOnAsync) |  Enables warp mode without waiting for the result This is the asynchronous counterpart of warpOn(). |
| [warpOff](#warpOff) |  Disables warp mode. |
| [warpOffAsync](#warpOffAsync) |  Disables warp mode without waiting for the result This is the asynchronous counterpart of warpOff(). |
| [trackOn](#trackOn) |  Enables track mode. |
| [trackOnAsync](#trackOnAsync) |  Enables track mode without waiting for the result This is the asynchronous counterpart of trackOn(). |
| [trackOff](#trackOff) |  Disables track mode. |
| [trackOffAsync](#trackOffAsync) |  Disables track mode without waiting for the result This is the asynchronous counterpart of trackOff(). |
| [stepInto](#stepInto) | @} @name Single-stepping @{ Steps a single instruction Steps the CPU for a single instruction. |
| [stepOver](#stepOver) |  Steps over the current instruction Runs the emulator until the instruction following the current instruction has been reached. |
| [stepCycle](#stepCycle) |  Steps one cycle Calling this function runs the CPU for a single cycle. |
| [finishLine](#finishLine) |  Finishes the current rasterline Calling this function runs the CPU until the end of the current rasterline. |
| [finishFrame](#finishFrame) |  Finishes the current frame Calling this function runs the CPU until the end of the current frame. |
| [wakeUp](#wakeUp) | @} @name Synchronizing the emulator thread @{ Sends a wakeup signal to the emulator thread. |
| [sync](#sync) |  Waits until every command sent so far has been carried out. |
| [launch](#launch) | @} @name Configuring the emulator @{ Launches the emulator thread. |
| [removeListener](#removeListener) |  Stops delivering messages to the registered listener The emulator outlives its client whenever the client is a stack or static object torn down at exit: the core keeps running, keeps posting, and calls into whatever the listener pointer used to be. Call this before the client becomes unusable. It is synchronised against message delivery, so it is safe while the emulator runs.  |
| [isLaunched](#isLaunched) |  Returns true if the emulator has been launched. |
| [get](#get) |  Queries a configuration option. |
| [get](#get) |  Queries a configuration option. |
| [set](#set) |  Configures a component. |
| [set](#set) |  Configures a component. |
| [set](#set) | Configures the emulator to match a specific C64 model :material-location-enter: `model` :       The C64 model to emulate :material-eye-outline: **See** :    set(Opt, i64)  |
| [setAsync](#setAsync) |  Configures a component without waiting for the result. |
| [setAsync](#setAsync) |  Configures a component without waiting for the result. |
| [setAsync](#setAsync) | Asynchronous version of set(ConfigScheme): applies a predefined configuration scheme.  |
| [check](#check) |  Checks if a configuration option would be accepted. |
| [check](#check) |  Checks if a configuration option would be accepted. |
| [exportConfig](#exportConfig) |  Exports the current configuration. |
| [exportConfig](#exportConfig) | Exports the configuration to an output stream (see above). |
| [call](#call) | @} @name Danger zone @{ Runs a function on the emulator thread and waits until it is done. |
| [callAsync](#callAsync) |  Queues a function for the emulator thread without waiting for it. |
| [put](#put) |  Feeds a command into the command queue and waits for the result. |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload, payload2)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [put](#put) | Shorthand for put(Command(type, payload)). |
| [putAsync](#putAsync) |  Feeds a command into the command queue without waiting for the result. |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload, payload2)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |
| [putAsync](#putAsync) | Shorthand for putAsync(Command(type, payload)). |

## Variable Details

### audioPort<a name="audioPort"></a>

!!! variable "AudioPortAPI audioPort"
    
    Audio port
    

### c64<a name="c64"></a>

!!! variable "C64API c64"
    
    C64 (state, snapshots, media)
    

### cia1<a name="cia1"></a>

!!! variable "CIAAPI cia1, cia2"
    
    The two CIA chips
    

### controlPort1<a name="controlPort1"></a>

!!! variable "ControlPortAPI controlPort1, controlPort2"
    
    The two control ports
    

### cpu<a name="cpu"></a>

!!! variable "CPUAPI cpu"
    
    CPU
    

### datasette<a name="datasette"></a>

!!! variable "DatasetteAPI datasette"
    
    Datasette
    

### defaults<a name="defaults"></a>

!!! variable "static DefaultsAPI defaults"
    
    User defaults storage
    

### drive8<a name="drive8"></a>

!!! variable "DriveAPI drive8, drive9"
    
    Floppy drives 8 and 9
    

### expansionPort<a name="expansionPort"></a>

!!! variable "ExpansionPortAPI expansionPort"
    
    Expansion port (cartridges)
    

### keyboard<a name="keyboard"></a>

!!! variable "KeyboardAPI keyboard"
    
    Keyboard
    

### mem<a name="mem"></a>

!!! variable "MemoryAPI mem"
    
    Memory
    

### remoteManager<a name="remoteManager"></a>

!!! variable "RemoteManagerAPI remoteManager"
    
    Remote servers
    

### retroShell<a name="retroShell"></a>

!!! variable "RetroShellAPI retroShell"
    
    RetroShell
    

### serialPort<a name="serialPort"></a>

!!! variable "SerialPortAPI serialPort"
    
    Serial port (IEC bus)
    

### sid<a name="sid"></a>

!!! variable "SIDAPI sid"
    
    SID chip(s)
    

### userPort<a name="userPort"></a>

!!! variable "UserPortAPI userPort"
    
    User port
    

### vicii<a name="vicii"></a>

!!! variable "VICIIAPI vicii"
    
    VICII graphics chip
    

### videoPort<a name="videoPort"></a>

!!! variable "VideoPortAPI videoPort"
    
    Video port (textures)
    

### xray<a name="xray"></a>

!!! variable "XRayAPI xray"
    
    X-Ray visualizer
    

## Function Details

### build<a name="build"></a>
!!! function "static string build()"
    
     Returns a build-number string for this release.
    

### call<a name="call"></a>
!!! function "void call(const std::function&lt;void()&gt; &amp;fn)"
    
    @}
    @name Danger zone
    @{
     Runs a function on the emulator thread and waits until it is done.
    
     
    !!! warning
     This is an advanced interface. It bypasses the dedicated API functions, and what it touches is not covered by any compatibility promise.
    
     The function is carried out in the order of the commands that have been
     queued before, and never while a frame is computed or while another
     thread has suspended the emulator. An exception thrown by the function
     is passed on to the caller.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### callAsync<a name="callAsync"></a>
!!! function "void callAsync(std::function&lt;void()&gt; fn, std::function&lt;void(std::exception_ptr)&gt; done = nullptr)"
    
     Queues a function for the emulator thread without waiting for it.
    
     
    !!! warning
     This is an advanced interface. It bypasses the dedicated API functions, and what it touches is not covered by any compatibility promise.
    
     This is the asynchronous counterpart of call(). The function is carried
     out by the emulator thread at some later time, so everything it refers
     to must stay valid until then. A failure of the function cannot be
     passed on to the caller. If 'done' is given, it is called on the
     emulator thread when the function has been carried out, with the
     exception it threw (or null). It is also called, on the calling thread,
     if the command queue is full and the request is dropped. Nothing is
     carried out as long as the emulator thread has not been launched.
    

### check<a name="check"></a>
!!! function "void check(Opt opt, i64 value)"
    
     Checks if a configuration option would be accepted.
    
     The check is performed in the calling thread and does not change the
     configuration. It is not needed before set(), which validates the value
     itself, but it lets a caller skip a request that is known to fail.
    
     
    :material-location-enter: `opt`
    :         The option to check
     
    :material-location-enter: `value`
    :       The option's value
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the value would be rejected
    

!!! function "void check(Opt opt, i64 value, isize id)"
    
     Checks if a configuration option would be accepted.
     
    :material-location-enter: `opt`
    :         The option to check
     
    :material-location-enter: `value`
    :       The option's value
     
    :material-location-enter: `id`
    :          The component to configure
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the value would be rejected
    

### checkReady<a name="checkReady"></a>
!!! function "void checkReady() const"
    
     Checks if the emulator is runnable.
     The function checks if the necessary ROMs are installed to launch the
     emulator. On success, the functions returns. Otherwise, an exception
     is thrown.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator cannot run, e.g., because a ROM is missing.
    

### exportConfig<a name="exportConfig"></a>
!!! function "void exportConfig(const std::filesystem::path &amp;path, bool diff = false) const"
    
     Exports the current configuration.
    
     The current configuration is exported in form of a RetroShell script.
     Reading in the script at a later point will restore the configuration.
    
     
    :material-location-enter: `path`
    :    The file the script is written to (or use the overload
                 taking an output stream).
     
    :material-location-enter: `diff`
    :    When set to true, the script will only contains settings
                 for items that differ from the default configuration.
                 This feature is useful for debugging to compare two virtual
                 machine configurations.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void exportConfig(std::ostream &amp;stream, bool diff = false) const"
    
    Exports the configuration to an output stream (see above).
    

### finishFrame<a name="finishFrame"></a>
!!! function "void finishFrame()"
    
     Finishes the current frame
    
     Calling this function runs the CPU until the end of the current frame.
    
     The function returns when the emulator has been started for the step. It
     is paused again when the step is done.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::RUNNING) if the emulator is already running.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### finishLine<a name="finishLine"></a>
!!! function "void finishLine()"
    
     Finishes the current rasterline
    
     Calling this function runs the CPU until the end of the current rasterline.
    
     The function returns when the emulator has been started for the step. It
     is paused again when the step is done.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::RUNNING) if the emulator is already running.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### get<a name="get"></a>
!!! function "i64 get(Opt opt) const"
    
     Queries a configuration option.
    
     This is the main function to query a configuration option.
    
     
    :material-location-enter: `opt`
    :         The option to query
    
     
    !!! note
     Some options require an additional parameter to uniquely
         determine the configured component. For those options, this function
         must not be called.
    
     
    !!! note
     The value is read without synchronizing with the emulator thread.
         A change requested by setAsync() shows up once the emulator thread has
         carried it out (see sync()).
    

!!! function "i64 get(Opt opt, isize id) const"
    
     Queries a configuration option.
    
     This is the main function to query a configuration option.
    
     
    :material-location-enter: `opt`
    :         The option to query
     
    :material-location-enter: `id`
    :          The component to query
    
     
    !!! note
     This function must only be called for those options that require
         an additional parameter to uniquely determine the configured component.
    
     
    !!! note
     The value is read without synchronizing with the emulator thread.
         A change requested by setAsync() shows up once the emulator thread has
         carried it out (see sync()).
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const EmulatorInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getCachedMetrics<a name="getCachedMetrics"></a>
!!! function "const EmulatorMetrics &amp;getCachedMetrics() const"
    
    Returns the most recent metrics snapshot recorded by the emulator thread (see getMetrics()).
    

### getInfo<a name="getInfo"></a>
!!! function "const EmulatorInfo &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getMetrics<a name="getMetrics"></a>
!!! function "const EmulatorMetrics &amp;getMetrics() const"
    
     Returns statistical information about the components.
    
     getMetrics() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedMetrics()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### halt<a name="halt"></a>
!!! function "void halt()"
    
      Terminates the emulator thread
    
     Halting the emulator changes the internal state to ExecState::HALTED.
     This state is part of the shutdown procedure and never entered during
     normal operation.
    
     
    !!! note
     The function blocks until the emulator thread has terminated. It
         does nothing if there is no running emulator thread (e.g., if the
         thread has already been halted), so it is safe to call it more than
         once. It throws a CoreError if the calling thread has suspended the
         emulator thread, because a suspended thread cannot terminate.
    

### hardReset<a name="hardReset"></a>
!!! function "void hardReset()"
    
    @}
    @name Resetting the C64
    @{
     Performs a hard reset
    
     A hard reset affects all components. The effect is similar to
     switching power off and on.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use hardResetAsync() in these situations.
    

### hardResetAsync<a name="hardResetAsync"></a>
!!! function "void hardResetAsync()"
    
     Performs a hard reset without waiting for the result
    
     This is the asynchronous counterpart of hardReset(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    

### isHalted<a name="isHalted"></a>
!!! function "bool isHalted() const"
    
     Returns true iff the emulator has shut down.
    

### isLaunched<a name="isLaunched"></a>
!!! function "bool isLaunched() const"
    
     Returns true if the emulator has been launched.
    

### isPaused<a name="isPaused"></a>
!!! function "bool isPaused() const"
    
     Returns true iff the emulator is in paused state.
    

### isPoweredOff<a name="isPoweredOff"></a>
!!! function "bool isPoweredOff() const"
    
     Returns true iff the emulator if the emulator is powered off.
    

### isPoweredOn<a name="isPoweredOn"></a>
!!! function "bool isPoweredOn() const"
    
    @}
    @name Querying the emulator state
    @{
     Returns true iff the emulator if the emulator is powered on.
    

### isRunning<a name="isRunning"></a>
!!! function "bool isRunning() const"
    
     Returns true iff the emulator is running.
    

### isSuspended<a name="isSuspended"></a>
!!! function "bool isSuspended() const"
    
     Returns true iff the emulator has been suspended.
    

### isTracking<a name="isTracking"></a>
!!! function "bool isTracking() const"
    
     Returns true iff the emulator runs in track mode.
    

### isWarping<a name="isWarping"></a>
!!! function "bool isWarping() const"
    
     Returns true iff warp mode is active.
    

### launch<a name="launch"></a>
!!! function "void launch(const void &#42;listener = nullptr, Callback &#42;func = nullptr)"
    
    @}
    @name Configuring the emulator
    @{
     Launches the emulator thread.
    
     This function must be called in the initialization procedure of the
     emulator. It launches the emulator thread and registers a callback
     function to the message queue. The callback function is a message
     receiver processing all incoming messages from the emulator.
    
     
    :material-location-enter: `listener`
    :       An arbitrary pointer which will be passed to the
     registered callback function as first argument. You can use the argument
     to pass the address of an object into the callback function. Inside the
     callback you can utilize the pointer to invoke a message processing
     function on that object.
     
    :material-location-enter: `func`
    :           The callback function.
    
     
    !!! note
     All functions of this API must be called from a user thread, i.e.,
         from any thread except the emulator thread. In particular, the callback
         registered here is invoked on the emulator thread and must not call into
         the API. Forward the message to a user thread instead.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::ALREADY_LAUNCHED) if the emulator thread has
     already been launched.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::LAUNCH_FAILED) if the emulator thread could not
     be launched.
    

### pause<a name="pause"></a>
!!! function "void pause()"
    
      Pauses emulation
    
    Pausing the emulator changes the internal state from ExecState::RUNNING
    to ExecState::PAUSED after completing the current frame. The emulator
    enters a frozen state where no more frames are computed.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use pauseAsync() in these situations.
    

### pauseAsync<a name="pauseAsync"></a>
!!! function "void pauseAsync()"
    
     Pauses emulation without waiting for the result
    
     This is the asynchronous counterpart of pause(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    

### powerOff<a name="powerOff"></a>
!!! function "void powerOff()"
    
     Switches the emulator off
    
     Powering off the emulator changes the internal state to ExecState::OFF.
     Calling this function on an already powered-off emulator has no effect.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use powerOffAsync() in these situations.
    

### powerOffAsync<a name="powerOffAsync"></a>
!!! function "void powerOffAsync()"
    
     Switches the emulator off without waiting for the result
    
     This is the asynchronous counterpart of powerOff(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    

### powerOn<a name="powerOn"></a>
!!! function "void powerOn()"
    
    @}
    @name Controlling the emulator state
    @{
     Switches the emulator on
    
     Powering on the emulator changes the internal state to ExecState::PAUSED,
     the same state that is entered when the user hits the pause button.
     Calling this function on an already powered-on emulator has no effect.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use powerOnAsync() in these situations.
    

### powerOnAsync<a name="powerOnAsync"></a>
!!! function "void powerOnAsync()"
    
     Switches the emulator on without waiting for the result
    
     This is the asynchronous counterpart of powerOn(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    

### put<a name="put"></a>
!!! function "void put(const Command &amp;cmd, const std::string &amp;str = &quot;&quot;)"
    
     Feeds a command into the command queue and waits for the result.
    
     
    !!! warning
     This is an advanced interface. It bypasses the dedicated API functions, and what it touches is not covered by any compatibility promise.
    
     The function returns when the emulator thread has carried out the
     command. A command that fails throws the corresponding exception.
    
     Because the queue is processed in order, all commands issued earlier
     (e.g., via putAsync()) have been carried out when the call returns.
     Commands that carry a pointer to an object of the caller can rely on
     that object being alive for the whole duration of the command.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the command, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use putAsync() in these situations (it can also be called before
     the emulator thread is launched, in which case the command is carried out
     at launch), and for events that occur at a high rate (e.g., key presses
     or mouse movements).
    

!!! function "void put(Cmd type, const std::string &amp;str)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, i64 payload = 0, i64 payload2 = 0)"
    
    Shorthand for put(Command(type, payload, payload2)).
    

!!! function "void put(Cmd type, ConfigCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, KeyCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, CoordCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, GamePadCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, TapeCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

!!! function "void put(Cmd type, AlarmCmd payload)"
    
    Shorthand for put(Command(type, payload)).
    

### putAsync<a name="putAsync"></a>
!!! function "void putAsync(const Command &amp;cmd, const std::string &amp;str = &quot;&quot;)"
    
     Feeds a command into the command queue without waiting for the result.
    
     
    !!! warning
     This is an advanced interface, see put().
    
     This is the asynchronous counterpart of put(). The command is carried out
     by the emulator thread at some later time. The caller must keep objects
     referenced by the command alive until it has been carried out (see sync()).
    

!!! function "void putAsync(Cmd type, const std::string &amp;str)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, i64 payload = 0, i64 payload2 = 0)"
    
    Shorthand for putAsync(Command(type, payload, payload2)).
    

!!! function "void putAsync(Cmd type, ConfigCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, KeyCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, CoordCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, GamePadCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, TapeCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

!!! function "void putAsync(Cmd type, AlarmCmd payload)"
    
    Shorthand for putAsync(Command(type, payload)).
    

### removeListener<a name="removeListener"></a>
!!! function "void removeListener()"
    
     Stops delivering messages to the registered listener
    
     The emulator outlives its client whenever the client is a stack or
     static object torn down at exit: the core keeps running, keeps
     posting, and calls into whatever the listener pointer used to be.
     Call this before the client becomes unusable. It is synchronised
     against message delivery, so it is safe while the emulator runs.
    

### run<a name="run"></a>
!!! function "void run()"
    
     Starts emulation
    
     Running the emulator changes the internal state to ExecState::RUNNING,
     which is the normal operation state. Frames are computed at normal
     pace, if warp mode is switched off, or as fast as possible, if warp
     mode is switched on. If this function is called for a powered-off
     emulator, an implicit call to powerOn() will be performed.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
    
     The function returns when the emulator has been started.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use runAsync() in these situations.
    

### runAsync<a name="runAsync"></a>
!!! function "void runAsync()"
    
     Starts emulation without waiting for the result
    
     This is the asynchronous counterpart of run(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
    

### set<a name="set"></a>
!!! function "void set(Opt opt, i64 value)"
    
     Configures a component.
    
     This is the main function to set a configuration option. It returns
     when the change has been made, so the new value is visible to get()
     right away. Because the command queue is processed in order, all
     requests issued earlier (e.g., via setAsync()) have been carried out as
     well. A rejected value throws the corresponding exception.
    
     
    :material-location-enter: `opt`
    :         The option to set
     
    :material-location-enter: `value`
    :       The option's value
    
     
    !!! note
     If this function is called for an options that applies to multiple
         components, all components are configured with the specified value.
         
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
         carry out the request, because it has not been launched yet, is halted or
         suspended. The same applies if the function is called from the emulator
         thread. Use setAsync() in these situations (it can also be called before
         the emulator thread is launched, in which case the request is carried out
         at launch).
    

!!! function "void set(Opt opt, i64 value, isize id)"
    
     Configures a component.
    
     
    :material-location-enter: `opt`
    :         The option to set
     
    :material-location-enter: `value`
    :       The option's value
     
    :material-location-enter: `id`
    :          The component to configure
    
     
    !!! note
     This function must only be called for those options that require
         an additional parameter to uniquely determine the configured component.
         
    :material-eye-outline: **See**
    :    set(Opt, i64)
    

!!! function "void set(ConfigScheme model)"
    
    Configures the emulator to match a specific C64 model
    
     
    :material-location-enter: `model`
    :       The C64 model to emulate
     
    :material-eye-outline: **See**
    :    set(Opt, i64)
    

### setAsync<a name="setAsync"></a>
!!! function "void setAsync(Opt opt, i64 value)"
    
     Configures a component without waiting for the result.
    
     These functions hand the request to the emulator thread and return at
     once. They are the asynchronous counterparts of set(). Use them if
     waiting is impossible or undesirable, e.g., when many options are
     changed in a row (follow up with sync() if needed), before the emulator
     thread has been launched (the request is carried out at launch), or
     while the emulator thread is suspended (it is carried out when the thread
     resumes).
    
     The value is validated when the emulator thread carries out the
     command. A rejected value is reported by Msg::CONFIG, which then carries the exception.
    
     
    :material-location-enter: `opt`
    :         The option to set
     
    :material-location-enter: `value`
    :       The option's value
    

!!! function "void setAsync(Opt opt, i64 value, isize id)"
    
     Configures a component without waiting for the result.
    
     Same as above, for options that require an additional parameter to
     determine the component.
    
     
    :material-location-enter: `opt`
    :         The option to set
     
    :material-location-enter: `value`
    :       The option's value
     
    :material-location-enter: `id`
    :          The component to configure
    

!!! function "void setAsync(ConfigScheme model)"
    
    Asynchronous version of set(ConfigScheme): applies a predefined configuration scheme.
    

### snapshotVersion<a name="snapshotVersion"></a>
!!! function "static string snapshotVersion()"
    
     Returns a version string for the supported snapshot format.
    

### softReset<a name="softReset"></a>
!!! function "void softReset()"
    
     Performs a soft reset
    
     A soft reset emulates a real reset of the C64 which can be initiated
     via the reset line on the expansion port.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use softResetAsync() in these situations.
    

### softResetAsync<a name="softResetAsync"></a>
!!! function "void softResetAsync()"
    
     Performs a soft reset without waiting for the result
    
     This is the asynchronous counterpart of softReset(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    

### stepCycle<a name="stepCycle"></a>
!!! function "void stepCycle()"
    
     Steps one cycle
    
     Calling this function runs the CPU for a single cycle.
    
     The function returns when the emulator has been started for the step. It
     is paused again when the step is done.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::RUNNING) if the emulator is already running.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### stepInto<a name="stepInto"></a>
!!! function "void stepInto()"
    
    @}
    @name Single-stepping
    @{
     Steps a single instruction
    
     Steps the CPU for a single instruction. The emulator must not be
     running. The CPU debugger utilizes this function to implement single
     stepping.
    
     The function returns when the emulator has been started for the step. It
     is paused again when the step is done.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::RUNNING) if the emulator is already running.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### stepOver<a name="stepOver"></a>
!!! function "void stepOver()"
    
     Steps over the current instruction
    
     Runs the emulator until the instruction following the current
     instruction has been reached. The emulator must not be running. The CPU
     debugger utilizes this function to implement single stepping.
    
     stepOver() acts like stepInto() except for branching instructions.
     For instance, if the current instruction is a `JSR` instruction, the
     subroutine will be executed in a single chunk.
    
     
    !!! note
     If the current instruction is a branch instruction which never
         returns, the emulator will remain in running state. Calling this
         function will then be equivalent to a calling run(), with the exception
         of a small runtime overhead since the emulator will check after the
         each instruction if the program counter has reached its target
         location.
    
     The function returns when the emulator has been started for the step. It
     is paused again when the step is done.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::RUNNING) if the emulator is already running.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError if the emulator is not ready to run (see checkReady()).
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request (see put()).
    

### sync<a name="sync"></a>
!!! function "void sync()"
    
     Waits until every command sent so far has been carried out.
    
     Most calls on this API hand the emulator thread a command and return
     at once, so a caller that reads back what it just changed, or exports
     the configuration, can still see the old state. Calling this
     afterwards closes that gap.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out commands, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread.
    

### trackOff<a name="trackOff"></a>
!!! function "void trackOff(isize source)"
    
     Disables track mode.
    
     Withdraws the request made by the given source. The mode is switched off
     when no other source requests it. Valid sources are 0 to 6.
    
     
    :material-location-enter: `source`
    :         The client requesting the change.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use trackOffAsync() in these situations.
    

### trackOffAsync<a name="trackOffAsync"></a>
!!! function "void trackOffAsync(isize source)"
    
     Disables track mode without waiting for the result
    
     This is the asynchronous counterpart of trackOff(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    

### trackOn<a name="trackOn"></a>
!!! function "void trackOn(isize source)"
    
     Enables track mode.
    
     Several clients can request the mode independently. Each client uses its
     own source, and the mode stays on as long as at least one source has
     requested it. Valid sources are 0 to 6.
    
     
    :material-location-enter: `source`
    :         The client requesting the change.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use trackOnAsync() in these situations.
    

### trackOnAsync<a name="trackOnAsync"></a>
!!! function "void trackOnAsync(isize source)"
    
     Enables track mode without waiting for the result
    
     This is the asynchronous counterpart of trackOn(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    

### version<a name="version"></a>
!!! function "static string version()"
    
    Drives by number
     Returns a version string for this release.
    

### wakeUp<a name="wakeUp"></a>
!!! function "void wakeUp()"
    
    @}
    @name Synchronizing the emulator thread
    @{
     Sends a wakeup signal to the emulator thread.
    
     To compute frames at the proper pace, the emulator core expects the GUI
     to send a wakeup signal on each VSYNC pulse. Once this signal is
     received, the emulator thread starts computing all missing frames. To
     minimize jitter, the wakeup signal should be sent right after the
     current texture has been handed over to the GPU.
    

### warpOff<a name="warpOff"></a>
!!! function "void warpOff(isize source)"
    
     Disables warp mode.
    
     Withdraws the request made by the given source. The mode is switched off
     when no other source requests it (the emulator itself may keep warp mode
     on to implement the warp mode option). Valid sources are 0 to 6.
    
     
    :material-location-enter: `source`
    :         The client requesting the change.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use warpOffAsync() in these situations.
    

### warpOffAsync<a name="warpOffAsync"></a>
!!! function "void warpOffAsync(isize source)"
    
     Disables warp mode without waiting for the result
    
     This is the asynchronous counterpart of warpOff(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    

### warpOn<a name="warpOn"></a>
!!! function "void warpOn(isize source)"
    
     Enables warp mode.
    
     Several clients can request the mode independently. Each client uses its
     own source, and the mode stays on as long as at least one source has
     requested it. Valid sources are 0 to 6.
    
     
    :material-location-enter: `source`
    :         The client requesting the change.
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    
     The function returns when the emulator thread has carried out the
     request. A failure throws the corresponding exception.
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::NOT_LAUNCHED) if the emulator thread cannot
     carry out the request, because it has not been launched yet, is halted or
     suspended. The same applies if the function is called from the emulator
     thread. Use warpOnAsync() in these situations.
    

### warpOnAsync<a name="warpOnAsync"></a>
!!! function "void warpOnAsync(isize source)"
    
     Enables warp mode without waiting for the result
    
     This is the asynchronous counterpart of warpOn(). The request is queued and
     carried out by the emulator thread at some later time (see sync()).
    
     
    :material-alert-circle-outline: **Throw**
    :    CoreError (CoreError::OPT_INV_ARG) if the source is out of range.
    

### ~VirtualC64<a name="_u007eVirtualC64"></a>
!!! function "~VirtualC64()"
    
     Halts the emulator thread and destroys the emulator
    
     A suspension that the calling thread still holds is released first,
     because a suspended emulator thread cannot be halted.
    

