---
generator: doxide
---


# vc64




:material-package: [NTSC](NTSC/)
:   NTSC related constants 

:material-package: [PAL](PAL/)
:   PAL related constants 

:material-package: [RL](RL/)
:   

:material-package: [Tex](Tex/)
:   Emulator texture parameters 

:material-package: [VICIIColorReg](VICIIColorReg/)
:   Indices of the VICII color registers 

:material-package: [peddle](peddle/)
:   

## Types

| Name | Description |
| ---- | ----------- |
| [API](API/) | File system format of a disk (see retro::vault::cbm::FSFormat) Base class of all APIs Provides the suspend / resume mechanism that every API class inherits.  |
| [AnyFile](AnyFile/) | Base class of all media files Provides basic functionality for reading and writing files, streams, and buffers. |
| [AudioPortAPI](AudioPortAPI/) | @} Audio Port Public API  |
| [AudioPortInfo](AudioPortInfo/) | State of the audio port (currently carries no data)  |
| [Block](Block/) | Block number  |
| [C64API](C64API/) | @} C64 API  |
| [C64Key](C64Key/) | Representation of a key on the C64 keyboard  |
| [CIAAPI](CIAAPI/) | CIA API  |
| [CIAReg](CIAReg/) | Registers of a CIA chip  |
| [CIARev](CIARev/) | Chip revision  |
| [CPUAPI](CPUAPI/) | CPU API  |
| [CPUDebuggerAPI](CPUDebuggerAPI/) | CPU Debugger API  |
| [CRTMode](CRTMode/) | Cartridge mode  |
| [CRTModeEnum](CRTModeEnum/) | No cartridge  |
| [CartridgeType](CartridgeType/) | Supported cartridge types  |
| [Cmd](Cmd/) | Emulator command  |
| [ColorSource](ColorSource/) | Source of a pixel color in the graphics synthesizer  |
| [Command](Command/) | A command that is fed into the command queue  |
| [CommandSet](CommandSet/) | Command sets of RetroShell  |
| [CommunicationDevice](CommunicationDevice/) | Devices that can be connected to the RS232 adapter  |
| [ConfigScheme](ConfigScheme/) | C64 model  |
| [ControlPortAPI](ControlPortAPI/) | Control Port API  |
| [ControlPortDevice](ControlPortDevice/) | Control port device  |
| [CoreError](CoreError/) |  Exception thrown if a request does not match the state of the emulator A CoreError is thrown, for example, if a configuration option is set to an invalid value, if a ROM is missing, or if the emulator thread cannot carry out a request. |
| [Cycle](Cycle/) | Emulator time in master clock cycles  |
| [Cylinder](Cylinder/) | Cylinder number  |
| [DatasetteAPI](DatasetteAPI/) | Datasette Public API  |
| [DatasetteModel](DatasetteModel/) | Datasette model  |
| [DebugFlag](DebugFlag/) | Flags for enabling debug output  |
| [DefaultsAPI](DefaultsAPI/) | @} The user's defaults storage The defaults storage manages all configuration settings that persist across multiple application launches. |
| [DiskAPI](DiskAPI/) | Disk Public API  |
| [DiskType](DiskType/) | Disk type  |
| [DisplayMode](DisplayMode/) | C64 display mode  |
| [DmaDisplayMode](DmaDisplayMode/) | DMA debugger display mode  |
| [Dotmask](Dotmask/) | Dotmask  |
| [DriveAPI](DriveAPI/) | Drive Public API  |
| [DriveRam](DriveRam/) | Memory expansion of a drive  |
| [DriveType](DriveType/) | Drive model  |
| [DrvMemType](DrvMemType/) | Memory types of a drive  |
| [EventID](EventID/) | Events that can be scheduled in the event slots  |
| [EventSlot](EventSlot/) | Slots of the event scheduler  |
| [ExecState](ExecState/) | Execution state  |
| [ExpansionPortAPI](ExpansionPortAPI/) | Expansion Port Public API  |
| [FileError](FileError/) | Error codes for file operations  |
| [FileType](FileType/) | Supported media file types  |
| [FlashState](FlashState/) | State of a flash ROM chip  |
| [GamePadAction](GamePadAction/) | Gamepad / Joystick event  |
| [GlueLogic](GlueLogic/) | Glue logic implementation  |
| [GuardsAPI](GuardsAPI/) | Guards API Gives access to one list of guards (the breakpoints or the watchpoints of the CPU). |
| [Halftrack](Halftrack/) | Halftrack number  |
| [Head](Head/) | Head number  |
| [HeadPos](HeadPos/) | Position of the drive head within a halftrack (in bits)  |
| [InputLine](InputLine/) | A line of input for RetroShell  |
| [InsertionStatus](InsertionStatus/) | Insertion state of a disk  |
| [JoystickAPI](JoystickAPI/) | Joystick Public API  |
| [KeyboardAPI](KeyboardAPI/) | Keyboard Public API  |
| [MediaError](MediaError/) |  Exception thrown if a media file is invalid or not supported Media files are ROMs, snapshots, cartridges, disk images, and similar files. |
| [MemAccess](MemAccess/) | Types of VICII memory accesses  |
| [MemType](MemType/) | ROM types (see retro::vault::RomType) ROM vendors (see retro::vault::RomVendor) Converts a ROM date into a string ROM platforms (see retro::vault::RomPlatform) Memory type  |
| [MemoryAPI](MemoryAPI/) | @} Memory API  |
| [MemoryDebuggerAPI](MemoryDebuggerAPI/) | Memory Debugger API  |
| [MouseAPI](MouseAPI/) | Mouse Public API  |
| [MouseModel](MouseModel/) | Mouse model  |
| [Msg](Msg/) | Emulator message  |
| [Opt](Opt/) | Configuration option  |
| [PaddleAPI](PaddleAPI/) | Paddle Public API  |
| [PaddleInfo](PaddleInfo/) | State of a paddle (currently carries no data)  |
| [PaddleOrientation](PaddleOrientation/) | Value mapping scheme  |
| [Palette](Palette/) | Color palette  |
| [ParCableType](ParCableType/) | Parallel cable type  |
| [PowerGrid](PowerGrid/) | Power grip characteristics  |
| [RS232API](RS232API/) | RS232 Public API  |
| [RSKey](RSKey/) | RetroShell special key  |
| [RamPattern](RamPattern/) | Ram startup pattern  |
| [RemoteManagerAPI](RemoteManagerAPI/) | RemoteManager Public API  |
| [RetroShellAPI](RetroShellAPI/) | @} RetroShell Public API  |
| [RunLoopFlags](RunLoopFlags/) | Flags that are set to request actions of the run loop  |
| [SIDAPI](SIDAPI/) | SID Public API  |
| [SIDEngine](SIDEngine/) | SID backend  |
| [SIDRevision](SIDRevision/) | Chip revision  |
| [SampleStream](SampleStream/) | Buffer for audio samples  |
| [SamplingMethod](SamplingMethod/) | Sampling method of the reSID engine  |
| [Scanlines](Scanlines/) | Scanlines  |
| [ScreenGeometry](ScreenGeometry/) | C64 canvas size  |
| [Sector](Sector/) | Sector number  |
| [SerialPortAPI](SerialPortAPI/) | @} Serial Port API  |
| [Snapshot](Snapshot/) | A snapshot of the emulator state A snapshot contains the complete internal state of the emulator together with a preview image. |
| [SnapshotHeader](SnapshotHeader/) | Header of a snapshot  |
| [TAPVersion](TAPVersion/) | Version of the TAP file format  |
| [TexFormat](TexFormat/) | Byte layout of a 32-bit host pixel  |
| [Thumbnail](Thumbnail/) | Preview image of a snapshot  |
| [Track](Track/) | Track number  |
| [TrafficDirection](TrafficDirection/) | Direction of a packet in the traffic log  |
| [Upscaler](Upscaler/) | Pixel upscaler  |
| [UserPortAPI](UserPortAPI/) | User Port API  |
| [UserPortInfo](UserPortInfo/) | State of the user port (currently carries no data)  |
| [VICIIAPI](VICIIAPI/) | VICII API  |
| [VICIIColor](VICIIColor/) | The 16 colors of the VICII chip  |
| [VICIIRev](VICIIRev/) | @endcond VICII chip revision  |
| [VideoPortAPI](VideoPortAPI/) | @} Video Port Public API  |
| [VirtualC64](VirtualC64/) | Public API This class declares the emulator's public API. |
| [Warp](Warp/) | Warp mode selection  |
| [XRayAPI](XRayAPI/) | DMA Debugger Public API  |
| [XRayCompositor](XRayCompositor/) | Who merges the X-Ray texture into the emulator texture  |
| [XRayMode](XRayMode/) | X-Ray mode  |
| [vspace](vspace/) | Vertical spacing, used in operator overloads  |

## Variables

| Name | Description |
| ---- | ----------- |
| [DRIVE8](#DRIVE8) | Number of drive 8  |
| [DRIVE9](#DRIVE9) | Number of drive 9  |
| [PORT_1](#PORT_1) | Control port 1  |
| [PORT_2](#PORT_2) | Control port 2  |
| [SPR0](#SPR0) | @cond INTERNAL  |
| [XRAY_COLOR_COUNT](#XRAY_COLOR_COUNT) | X-Ray mode Number of persisted Opt::XRAY_COLORn slots  |
| [a](#a) | Accumulator  |
| [action](#action) | The joystick action  |
| [addr](#addr) | Target of a guard  |
| [addr](#addr) | Description of a breakpoint or watchpoint Observed address  |
| [address](#address) | Location of the SID registers in memory  |
| [alarm](#alarm) | Alarm time  |
| [asr](#asr) | Adaptive Sample Rate enable switch  |
| [attackRate](#attackRate) | Envelope attack rate  |
| [autoConfig](#autoConfig) | Configuration of a drive General  |
| [autofire](#autofire) | Configuration of a joystick Autofire  |
| [autofireBullets](#autofireBullets) | Number of bullets per burst  |
| [autofireBursts](#autofireBursts) | Burst mode  |
| [autofireDelay](#autofireDelay) | Autofire delay (in frames)  |
| [awaiting](#awaiting) | Chip revision that takes effect on the next reset  |
| [axisX](#axisX) | Horizontal axis (-1 left, 0 neutral, 1 right)  |
| [axisY](#axisY) | Vertical axis (-1 up, 0 neutral, 1 down)  |
| [badLine](#badLine) | The current line is a bad line  |
| [bankMap](#bankMap) | Current memory bank configuration  |
| [battery](#battery) | The cartridge has battery-buffered memory  |
| [baud](#baud) | Transmission rate (baud)  |
| [bgColor0](#bgColor0) | Background color 0  |
| [bgColor1](#bgColor1) | Background color 1  |
| [bgColor2](#bgColor2) | Background color 2  |
| [bgColor3](#bgColor3) | Background color 3  |
| [bloom](#bloom) | Bloom  |
| [bloomBrightness](#bloomBrightness) | Bloom brightness  |
| [bloomRadius](#bloomRadius) | Bloom radius  |
| [bloomWeight](#bloomWeight) | Bloom weight  |
| [blur](#blur) | Blur  |
| [blurRadius](#blurRadius) | Blur radius  |
| [borderColor](#borderColor) | Border color  |
| [brightness](#brightness) | Monitor brightness  |
| [bufferOverflows](#bufferOverflows) | Number of buffer overflows since power up  |
| [bufferSize](#bufferSize) | Sample buffer capacity  |
| [bufferUnderflows](#bufferUnderflows) | Number of buffer underflows since power up  |
| [button](#button) | State of a joystick The fire button is pressed  |
| [button1](#button1) | Label of button 1  |
| [button2](#button2) | Label of button 2  |
| [buttons](#buttons) | Number of buttons  |
| [canvasFastPath](#canvasFastPath) | Performance statistics of the graphics synthesizer Number of canvas drawings that took the fast path  |
| [canvasSlowPath](#canvasSlowPath) | Number of canvas drawings that took the slow path  |
| [charMemoryAddr](#charMemoryAddr) | Address of the character memory  |
| [charen](#charen) | CHAREN bit of the processor port  |
| [checkSBCollisions](#checkSBCollisions) | Detect sprite-background collisions  |
| [checkSSCollisions](#checkSSCollisions) | Detect sprite-sprite collisions  |
| [cia1Progress](#cia1Progress) | Clock of CIA 1 (in cycles)  |
| [cia2Progress](#cia2Progress) | Clock of CIA 2 (in cycles)  |
| [clones](#clones) | Number of created run-ahead instances  |
| [color](#color) | Sprite color  |
| [compositor](#compositor) | Compositor used to merge the overlay with the picture  |
| [config](#config) | Configuration payload  |
| [connected](#connected) | Connection status  |
| [connected](#connected) | The drive is connected to the serial port  |
| [console](#console) | State of RetroShell Active console  |
| [consumedSamples](#consumedSamples) | Total number of sampels grabbed by the audio backend  |
| [contrast](#contrast) | Monitor contrast  |
| [count](#count) | State of a CIA timer Current counter value  |
| [counter](#counter) | Progress counter  |
| [cpu](#cpu) | CPU payload  |
| [cpuLoad](#cpuLoad) | Collected run-time data Measured CPU load  |
| [cpuProgress](#cpuProgress) | State of the C64 component CPU clock (in cycles)  |
| [crc32](#crc32) | Description of an installed ROM CRC32 checksum  |
| [ctrl1](#ctrl1) | Control register 1  |
| [ctrl2](#ctrl2) | Control register 2  |
| [cursorRel](#cursorRel) | Relative position of the cursor  |
| [cutLayers](#cutLayers) | Layers cut out in XRayMode::XRAY_LAYERS Bits 0 - 7: sprites, 9: background, 10: foreground, 11: border  |
| [cycle](#cycle) | Payload of alarm commands Cycle at which the alarm fires  |
| [cycle](#cycle) | State of the CPU Cycle counter  |
| [cyclesPerFrame](#cyclesPerFrame) | Number of cycles per frame  |
| [cyclesPerLine](#cyclesPerLine) | Number of cycles per raster line  |
| [dapInfo](#dapInfo) | State of the DAP server  |
| [dasmNumbers](#dasmNumbers) | Configuration of the CPU Disassembler number format  |
| [dataBegin](#dataBegin) | Start of the data block  |
| [dataEnd](#dataEnd) | End of the data block  |
| [decayRate](#decayRate) | Envelope decay rate  |
| [delay](#delay) | Delay in seconds  |
| [delayedLpIrqs](#delayedLpIrqs) | Light pen interrupts are delayed  |
| [denBit](#denBit) | Display enable bit  |
| [device](#device) | Configuration of the user port User port device  |
| [device](#device) | Configuration of the RS232 adapter Connected RS232 device  |
| [dir](#dir) | Data direction register (DDRA or DDRB)  |
| [direction](#direction) | Tells whether the packet was received or sent  |
| [disalignment](#disalignment) | Cathode ray disalignment  |
| [disalignmentH](#disalignmentH) | Horizontal cathode ray shift  |
| [disalignmentV](#disalignmentV) | Vertical cathode ray shift  |
| [displayMode](#displayMode) | Display mode  |
| [displayState](#displayState) | Display state (display or idle)  |
| [dmaChannel](#dmaChannel) | Visualize the DMA channel (one flag per channel)  |
| [dmaColor](#dmaColor) | Colors of the DMA channels (0 - 5) and of the cut-out layers (0 - 10). |
| [dmaDisplayMode](#dmaDisplayMode) | DMA display mode  |
| [dmaOpacity](#dmaOpacity) | Opacity of the DMA overlay  |
| [dmaOverlay](#dmaOverlay) | Enable the DMA overlay  |
| [dotMaskBrightness](#dotMaskBrightness) | Dotmask brightness  |
| [dotmask](#dotmask) | Dotmask  |
| [drive](#drive) | Drive payload  |
| [droppedFrames](#droppedFrames) | Statistics of the video port Number of frames that were not grabbed in time  |
| [dx](#dx) | Horizontal scroll value  |
| [dy](#dy) | Vertical scroll value  |
| [ejectDelay](#ejectDelay) | Disk ejection delay  |
| [ejectVolume](#ejectVolume) | Disk ejection volume  |
| [enable](#enable) | Configuration of a remote server Enable status  |
| [enabled](#enabled) | The guard is enabled  |
| [enabled](#enabled) | Configuration of a SID chip Indicates if this SID is plugged in  |
| [enabled](#enabled) | The sprite is enabled  |
| [engine](#engine) | Emulation engine  |
| [eventId](#eventId) | Identifier of the scheduled event (0 if the slot is empty)  |
| [eventName](#eventName) | Name of the scheduled event  |
| [expandX](#expandX) | Horizontal expansion  |
| [expandY](#expandY) | Vertical expansion  |
| [exrom](#exrom) | State of the memory State of the EXROM line  |
| [exromLineInCrtFile](#exromLineInCrtFile) | Value of the EXROM line stored in the CRT file  |
| [extraColor1](#extraColor1) | Extra color 1 (multicolor mode)  |
| [extraColor2](#extraColor2) | Extra color 2 (multicolor mode)  |
| [fillLevel](#fillLevel) | Audio statistics Current ring buffer fill level  |
| [filter](#filter) | Use filter  |
| [filterCutoff](#filterCutoff) | Filter cutoff frequency  |
| [filterEnableBit](#filterEnableBit) | The voice passes through the filter  |
| [filterEnableBits](#filterEnableBits) | Voices routed through the filter  |
| [filterModeBits](#filterModeBits) | Filter mode bits  |
| [filterResonance](#filterResonance) | Filter resonance  |
| [filterType](#filterType) | Filter type  |
| [firstSectorNr](#firstSectorNr) | Logical number of first sector in track  |
| [fnv64](#fnv64) | FNV-64 checksum  |
| [fps](#fps) | Measured frames per seconds  |
| [fps](#fps) | Frames per second  |
| [frame](#frame) | Frame number  |
| [frameBufferHeight](#frameBufferHeight) | Height of the host frame buffer (in pixels)  |
| [frameBufferWidth](#frameBufferWidth) | Width of the host frame buffer (in pixels)  |
| [frameFF](#frameFF) | Frame flip-flops  |
| [frameRel](#frameRel) | Trigger frame relative to the current frame  |
| [frequency](#frequency) | Frequency value  |
| [frequency](#frequency) | Clock frequency (in Hz)  |
| [game](#game) | State of the GAME line  |
| [gameLineInCrtFile](#gameLineInCrtFile) | Value of the GAME line stored in the CRT file  |
| [gateBit](#gateBit) | Gate bit  |
| [glueLogic](#glueLogic) | Glue logic type  |
| [grayCodeBug](#grayCodeBug) | The chip has the gray-code bug  |
| [grayDotBug](#grayDotBug) | Emulate the gray-dot bug  |
| [greenLED](#greenLED) | The green LED is on  |
| [hCenter](#hCenter) | Horizontal center  |
| [hZoom](#hZoom) | Horizontal zoom  |
| [halftrack](#halftrack) | Current head position (halftrack)  |
| [hardSync](#hardSync) | Hard sync is on  |
| [hasDisk](#hasDisk) | A disk is inserted  |
| [hasModifiedDisk](#hasModifiedDisk) | The inserted disk has unsaved changes  |
| [hasProtectedDisk](#hasProtectedDisk) | The inserted disk is write-protected  |
| [hasRom](#hasRom) | A ROM is installed  |
| [hasTape](#hasTape) | Datasette current state Is a tape inserted? |
| [hasUnmodifiedDisk](#hasUnmodifiedDisk) | The inserted disk has no unsaved changes  |
| [hasUnprotectedDisk](#hasUnprotectedDisk) | The inserted disk is not write-protected  |
| [headerBegin](#headerBegin) | Position of a sector on a track (in bits) Start of the sector header  |
| [headerEnd](#headerEnd) | End of the sector header  |
| [heatmap](#heatmap) | Count memory accesses for the heatmap  |
| [hideSprites](#hideSprites) | Sprites  |
| [highestSector](#highestSector) | Highest sector number (numbering starts with 0)  |
| [hiram](#hiram) | HIRAM bit of the processor port  |
| [hits](#hits) | Number of times the guard was hit  |
| [hour](#hour) | Hours  |
| [hpos](#hpos) | Raster cycle at which the event triggers  |
| [hpos](#hpos) | Horizontal beam position (raster cycle)  |
| [icr](#icr) | Interrupt control register (pending interrupts)  |
| [id](#id) | The component the option belongs to  |
| [id](#id) | State of a drive Number of the drive  |
| [idleCycles](#idleCycles) | Number of cycles spent in the idle state  |
| [idlePercentage](#idlePercentage) | Share of idle cycles  |
| [idleSamples](#idleSamples) | Total number of samples produced without executing SID  |
| [idleSince](#idleSince) | Cycle at which the idle state started  |
| [idleSleep](#idleSleep) | Enter the idle state while the chip is not in use  |
| [ignore](#ignore) | Number of hits that are ignored before the guard triggers  |
| [ignores](#ignores) | Ignore count  |
| [imr](#imr) | Interrupt mask register  |
| [imr](#imr) | Interrupt mask register  |
| [insertDelay](#insertDelay) | Disk insertion delay  |
| [insertVolume](#insertVolume) | Disk insertion volume  |
| [intLine](#intLine) | The interrupt line is active  |
| [irq](#irq) | State of the IRQ lines  |
| [irqLine](#irqLine) | Raster line that triggers a raster interrupt  |
| [irr](#irr) | Interrupt request register  |
| [isCommodoreRom](#isCommodoreRom) | The ROM is an original Commodore ROM  |
| [isMega65Rom](#isMega65Rom) | The ROM is an OpenROM of the MEGA65 project  |
| [isPatchedRom](#isPatchedRom) | The ROM is a patched variant  |
| [jammed](#jammed) | The CPU is jammed  |
| [keycode](#keycode) | Payload of keyboard commands Number of the key  |
| [latch](#latch) | Latch value the counter is reloaded from  |
| [latch](#latch) | Latched time  |
| [latchedLPX](#latchedLPX) | Latched light pen X coordinate  |
| [latchedLPY](#latchedLPY) | Latched light pen Y coordinate  |
| [latestGrabbedFrame](#latestGrabbedFrame) | State of the video port Number of the most recent frame that was grabbed by the host  |
| [led](#led) | The LED is on  |
| [leds](#leds) | Number of LEDs  |
| [length](#length) | Layout of a track Length of the track (in bits)  |
| [lengthInBits](#lengthInBits) | Typical track size in bits  |
| [lengthInBytes](#lengthInBytes) | Typical track size in bits  |
| [line](#line) | Payload of script messages  |
| [linesPerFrame](#linesPerFrame) | Number of raster lines per frame  |
| [list](#list) | Payload of breakpoint and watchpoint commands The guard list to modify  |
| [loadAddress](#loadAddress) | Address the ROM chip is mapped to  |
| [loram](#loram) | LORAM bit of the processor port  |
| [lpIrqHasOccurred](#lpIrqHasOccurred) | A light pen interrupt has occurred in this frame  |
| [lpLine](#lpLine) | State of the light pen line  |
| [main](#main) | Main frame flip-flop  |
| [memSelect](#memSelect) | Memory control register  |
| [memory](#memory) | Memory size (in KB)  |
| [memoryBankAddr](#memoryBankAddr) | Start address of the VIC bank  |
| [min](#min) | Minutes  |
| [mode](#mode) | Configuration of the X-Ray visualizer X-Ray mode (off, DMA debugger, layer inspector)  |
| [model](#model) | Configuration of a mouse Mouse model  |
| [model](#model) | Datasette configuration Datasette model  |
| [motor](#motor) | Is the motor on? |
| [multicolor](#multicolor) | Multicolor mode  |
| [needsExecution](#needsExecution) | The cartridge needs to be executed every cycle  |
| [next](#next) | Next micro instruction to be executed  |
| [nmi](#nmi) | State of the NMI lines  |
| [nr](#nr) | Payload of drive messages  |
| [nr](#nr) | Number of a guard in the list  |
| [nr](#nr) | An entry of the traffic log Sequence number (unique, monotonically increasing)  |
| [numPackets](#numPackets) | Number of chip packets  |
| [offset](#offset) | Head offset within the track  |
| [oneShot](#oneShot) | One-shot mode is on  |
| [option](#option) | Payload of configuration messages  |
| [option](#option) | Payload of configuration commands The option to change  |
| [orientation](#orientation) | Configuration of a paddle Paddle value mapping scheme  |
| [pal](#pal) | Properties of a VICII chip revision The chip is a PAL chip  |
| [palette](#palette) | Configuration of the monitor emulation Color palette  |
| [pan](#pan) | Channel pan factors  |
| [pan](#pan) | Stereo pan of the drive sounds  |
| [parCable](#parCable) | Parallel cable  |
| [paused](#paused) | Indicates if emulation is paused  |
| [payload](#payload) | Packet contents  |
| [pbout](#pbout) | The timer output appears on port B  |
| [pc](#pc) | Payload of CPU messages (the program counter)  |
| [pc](#pc) | Program counter  |
| [pc0](#pc0) | Address of the current instruction  |
| [peekSrc](#peekSrc) | Memory type read by the CPU from each 4 KB bank  |
| [playKey](#playKey) | Is the play key pressed? |
| [port](#port) | Payload of mouse commands Control port number  |
| [port](#port) | Payload of joystick commands Control port number  |
| [port](#port) | Payload of joystick state commands Control port number  |
| [port](#port) | The socket port of this server  |
| [port](#port) | State of a CIA port State of the port pins  |
| [portA](#portA) | State of a CIA chip Port A  |
| [portB](#portB) | Port B  |
| [potX](#potX) | Paddle value X (POTX)  |
| [potY](#potY) | Paddle value Y (POTY)  |
| [powerGrid](#powerGrid) | Configuration of the power port Power grid stability  |
| [powerSave](#powerSave) | Performance  |
| [powerSave](#powerSave) | Take fast paths  |
| [powerSave](#powerSave) | Take fast paths  |
| [powerVolume](#powerVolume) | Power switch volume  |
| [powered](#powered) | Indicates if the emulator is powered on  |
| [priority](#priority) | The sprite is drawn behind the background  |
| [processorPort](#processorPort) | Processor port register  |
| [processorPortDir](#processorPortDir) | Processor port data direction register  |
| [producedSamples](#producedSamples) | Total number of generated sampleds  |
| [promInfo](#promInfo) | State of the Prometheus server  |
| [pulseWidth](#pulseWidth) | Pulse width value  |
| [quickExitHit](#quickExitHit) | Number of successful quick-exit checks  |
| [quickExitMiss](#quickExitMiss) | Number of failed quick-exit checks  |
| [ram](#ram) | Drive RAM  |
| [ramPattern](#ramPattern) | Configuration of the memory RAM start-up pattern  |
| [rasterCycle](#rasterCycle) | Current raster cycle  |
| [rc](#rc) | Row counter (RC)  |
| [rdy](#rdy) | State of the RDY line  |
| [reads](#reads) | Memory access statistics Number of read accesses per address  |
| [redLED](#redLED) | The red LED is on  |
| [refreshRate](#refreshRate) | Properties of the host machine Refresh rate of the host display  |
| [reg](#reg) | Port register (PRA or PRB)  |
| [reg](#reg) | State of a SID voice Raw voice registers  |
| [releaseRate](#releaseRate) | Envelope release rate  |
| [resyncs](#resyncs) | Number of out-of-sync conditions  |
| [reuSpeed](#reuSpeed) | Configuration of the expansion port Speed of the REU  |
| [revision](#revision) | Configuration of a CIA chip Chip revision  |
| [revision](#revision) | Chip revision (MOS 6581 or MOS 8580)  |
| [revision](#revision) | Configuration of the VICII chip Chip revision  |
| [revision](#revision) | Revision of the ROM  |
| [ringMod](#ringMod) | Ring modulation is on  |
| [rpcInfo](#rpcInfo) | State of the RPC server  |
| [rshInfo](#rshInfo) | State of the remote servers State of the RetroShell server  |
| [runAhead](#runAhead) | Number of run-ahead frames (0 = run-ahead is disabled)  |
| [running](#running) | Indicates if the emulator is running  |
| [running](#running) | The timer is running  |
| [sampleRate](#sampleRate) | Audio sample rate of the host computer  |
| [sampling](#sampling) | Sampling method  |
| [saturation](#saturation) | Monitor saturation  |
| [saveRoms](#saveRoms) | Include ROMs in snapshots  |
| [saveRoms](#saveRoms) | Save Roms  |
| [sbCollision](#sbCollision) | A sprite-background collision was detected  |
| [scanline](#scanline) | State of the VICII chip Current raster line  |
| [scanlineBrightness](#scanlineBrightness) | Scanline brightness  |
| [scanlineWeight](#scanlineWeight) | Scanline weight  |
| [scanlines](#scanlines) | Scanlines  |
| [screenGeometry](#screenGeometry) | Screen geometry  |
| [screenMemoryAddr](#screenMemoryAddr) | Address of the screen memory  |
| [script](#script) | Script payload  |
| [sdr](#sdr) | Serial data register (SDR)  |
| [sec](#sec) | Seconds  |
| [sectorLayout](#sectorLayout) | Layout of the sectors  |
| [sectors](#sectors) | Default properties of a track Typical number of sectors in this track  |
| [server](#server) | The server that received or sent the packet  |
| [shakeDetection](#shakeDetection) | Detect a shaken mouse  |
| [size](#size) | Description of a cartridge ROM chip Size of the ROM chip (in bytes)  |
| [slot](#slot) | State of a single event slot The event slot  |
| [slotInfo](#slotInfo) | Events  |
| [snapshot](#snapshot) | Payload of snapshot messages  |
| [snapshot](#snapshot) | Snapshot payload  |
| [sp](#sp) | Stack pointer  |
| [speedBoost](#speedBoost) | Emulator speed in percent (100 is native speed)  |
| [speedZone](#speedZone) | Default speed zone for this track  |
| [spinning](#spinning) | The disk is spinning  |
| [spriteFastPath](#spriteFastPath) | Number of sprite drawings that took the fast path  |
| [spriteSlowPath](#spriteSlowPath) | Number of sprite drawings that took the slow path  |
| [sr](#sr) | Status register  |
| [ssCollision](#ssCollision) | A sprite-sprite collision was detected  |
| [ssr](#ssr) | Serial shift register  |
| [stagger](#stagger) | Relative position of first bit (from Hoxs64)  |
| [state](#state) | Bits 0-4: current state, bits 8-12: previous state (up, down, left, right, fire)  |
| [state](#state) | The current emulator state The current emulator state  |
| [state](#state) | State of a remote server State of the server  |
| [stepVolume](#stepVolume) | Head step volume  |
| [str](#str) | Optional text (may be null)  |
| [subtitle](#subtitle) | Subtitle of the ROM  |
| [supported](#supported) | The cartridge type is supported  |
| [suspended](#suspended) | Indicates if the emulator is in suspended state  |
| [sustainRate](#sustainRate) | Envelope sustain level  |
| [swapDelay](#swapDelay) | Disk swap delay  |
| [switchLeft](#switchLeft) | Label of the left switch position  |
| [switchNeutral](#switchNeutral) | Label of the neutral switch position  |
| [switchPos](#switchPos) | Position of the switch  |
| [switchRight](#switchRight) | Label of the right switch position  |
| [switchedOn](#switchedOn) | Power switch  |
| [switches](#switches) | Number of switches  |
| [tape](#tape) | Payload of tape commands Pointer to the tape file  |
| [tenth](#tenth) | Time-of-day value in BCD format Tenths of a second  |
| [testBit](#testBit) | Test bit  |
| [texFormat](#texFormat) | Texture format  |
| [time](#time) | State of the time-of-day clock Current time  |
| [time](#time) | Host time stamp (milliseconds since epoch)  |
| [timerA](#timerA) | Timer A  |
| [timerB](#timerB) | Timer B  |
| [timerBBug](#timerBBug) | Emulate the Timer-B bug  |
| [title](#title) | Title of the cartridge  |
| [title](#title) | Title of the ROM  |
| [tod](#tod) | Time-of-day clock  |
| [todIntEnable](#todIntEnable) | The TOD alarm interrupt is enabled  |
| [toggle](#toggle) | Toggle mode is on  |
| [totalCycles](#totalCycles) | Usage statistics of a CIA chip Total number of executed cycles  |
| [trackLayout](#trackLayout) | Layout of a disk Layout of the tracks  |
| [tracking](#tracking) | Indicates if track mode is enabled  |
| [transport](#transport) | The transport protocol of this server  |
| [trigger](#trigger) | Trigger cycle of the event  |
| [triggerRel](#triggerRel) | Time until the event triggers (in cycles)  |
| [type](#type) | Properties of a cartridge type Cartridge type  |
| [type](#type) | State of an attached cartridge DEPRECATED  |
| [type](#type) | A message sent from the emulator to the GUI Type of the message  |
| [type](#type) | TAP file format  |
| [type](#type) | Drive model  |
| [ultimax](#ultimax) | Ultimax memory mode is active  |
| [upscaler](#upscaler) | Effects  |
| [vCenter](#vCenter) | Vertical center  |
| [vZoom](#vZoom) | Vertical zoom  |
| [value](#value) | Message payload  |
| [value](#value) | The new value  |
| [value](#value) | Value reported by the alarm  |
| [value](#value) | The time as a single 32-bit value  |
| [value2](#value2) | Generic payload  |
| [vblank](#vblank) | Vertical blanking is active  |
| [vc](#vc) | Video counter (VC)  |
| [vcBase](#vcBase) | Video counter base (VCBASE)  |
| [velocity](#velocity) | Mouse velocity  |
| [verbose](#verbose) | If true, transmitted packets are shown in RetroShell  |
| [vertical](#vertical) | State of the frame flip-flops Vertical frame flip-flop  |
| [vicPeekSrc](#vicPeekSrc) | Memory type read by the VICII from each 4 KB bank  |
| [visibleLines](#visibleLines) | Number of visible raster lines  |
| [vmli](#vmli) | Video matrix line index (VMLI)  |
| [voice](#voice) | State of the three voices  |
| [vol](#vol) | Configuration of the audio port Channel volumes  |
| [volL](#volL) | Master volume (left)  |
| [volR](#volR) | Master volume (right)  |
| [volume](#volume) | State of a SID chip Master volume  |
| [vpos](#vpos) | Raster line at which the event triggers  |
| [vpos](#vpos) | Vertical beam position (raster line)  |
| [vsync](#vsync) | Vertical Synchronization  |
| [warpBoot](#warpBoot) | Configuration of the C64 component After a reset, the emulator runs in warp mode for this amout of seconds  |
| [warpMode](#warpMode) | Warp mode  |
| [warping](#warping) | Indicates if warp mode is currently on  |
| [waveform](#waveform) | Waveform selection bits  |
| [whiteNoise](#whiteNoise) | Configuration of the video port Show white noise while the machine is off  |
| [writes](#writes) | Number of write accesses per address  |
| [writing](#writing) | The drive is writing  |
| [x](#x) | X coordinate  |
| [x](#x) | X register  |
| [x](#x) | State of a sprite Horizontal position  |
| [xCounter](#xCounter) | Horizontal position counter  |
| [y](#y) | Y coordinate  |
| [y](#y) | Y register  |
| [y](#y) | Vertical position  |
| [yCounter](#yCounter) | Raster line counter  |

## Variable Details

### DRIVE8<a name="DRIVE8"></a>

!!! variable "static const isize DRIVE8"
    
    Number of drive 8
    

### DRIVE9<a name="DRIVE9"></a>

!!! variable "static const isize DRIVE9"
    
    Number of drive 9
    

### PORT_1<a name="PORT_1"></a>

!!! variable "static constexpr auto PORT_1"
    
    Control port 1
    

### PORT_2<a name="PORT_2"></a>

!!! variable "static constexpr auto PORT_2"
    
    Control port 2
    

### SPR0<a name="SPR0"></a>

!!! variable "inline constexpr int SPR0"
    
    @cond INTERNAL
    

### XRAY_COLOR_COUNT<a name="XRAY_COLOR_COUNT"></a>

!!! variable "constexpr isize XRAY_COLOR_COUNT"
    
    X-Ray mode
    Number of persisted Opt::XRAY_COLORn slots
    

### a<a name="a"></a>

!!! variable "u8 a"
    
    Accumulator
    

### action<a name="action"></a>

!!! variable "GamePadAction action"
    
    The joystick action
    

### addr<a name="addr"></a>

!!! variable "u32 addr"
    
    Target of a guard
    

### addr<a name="addr"></a>

!!! variable "u32 addr"
    
    Description of a breakpoint or watchpoint
    Observed address
    

### address<a name="address"></a>

!!! variable "u16 address"
    
    Location of the SID registers in memory
    

### alarm<a name="alarm"></a>

!!! variable "TimeOfDay alarm"
    
    Alarm time
    

### asr<a name="asr"></a>

!!! variable "bool asr"
    
    Adaptive Sample Rate enable switch
    

### attackRate<a name="attackRate"></a>

!!! variable "u8 attackRate"
    
    Envelope attack rate
    

### autoConfig<a name="autoConfig"></a>

!!! variable "bool autoConfig"
    
    Configuration of a drive
    General
    

### autofire<a name="autofire"></a>

!!! variable "bool autofire"
    
    Configuration of a joystick
    Autofire
    

### autofireBullets<a name="autofireBullets"></a>

!!! variable "isize autofireBullets"
    
    Number of bullets per burst
    

### autofireBursts<a name="autofireBursts"></a>

!!! variable "bool autofireBursts"
    
    Burst mode
    

### autofireDelay<a name="autofireDelay"></a>

!!! variable "isize autofireDelay"
    
    Autofire delay (in frames)
    

### awaiting<a name="awaiting"></a>

!!! variable "VICIIRev awaiting"
    
    Chip revision that takes effect on the next reset
    

### axisX<a name="axisX"></a>

!!! variable "int axisX"
    
    Horizontal axis (-1 left, 0 neutral, 1 right)
    

### axisY<a name="axisY"></a>

!!! variable "int axisY"
    
    Vertical axis (-1 up, 0 neutral, 1 down)
    

### badLine<a name="badLine"></a>

!!! variable "bool badLine"
    
    The current line is a bad line
    

### bankMap<a name="bankMap"></a>

!!! variable "u8   bankMap"
    
    Current memory bank configuration
    

### battery<a name="battery"></a>

!!! variable "bool  battery"
    
    The cartridge has battery-buffered memory
    

### baud<a name="baud"></a>

!!! variable "isize baud"
    
    Transmission rate (baud)
    

### bgColor0<a name="bgColor0"></a>

!!! variable "u8 bgColor0"
    
    Background color 0
    

### bgColor1<a name="bgColor1"></a>

!!! variable "u8 bgColor1"
    
    Background color 1
    

### bgColor2<a name="bgColor2"></a>

!!! variable "u8 bgColor2"
    
    Background color 2
    

### bgColor3<a name="bgColor3"></a>

!!! variable "u8 bgColor3"
    
    Background color 3
    

### bloom<a name="bloom"></a>

!!! variable "bool bloom"
    
    Bloom
    

### bloomBrightness<a name="bloomBrightness"></a>

!!! variable "isize bloomBrightness"
    
    Bloom brightness
    

### bloomRadius<a name="bloomRadius"></a>

!!! variable "isize bloomRadius"
    
    Bloom radius
    

### bloomWeight<a name="bloomWeight"></a>

!!! variable "isize bloomWeight"
    
    Bloom weight
    

### blur<a name="blur"></a>

!!! variable "bool blur"
    
    Blur
    

### blurRadius<a name="blurRadius"></a>

!!! variable "isize blurRadius"
    
    Blur radius
    

### borderColor<a name="borderColor"></a>

!!! variable "u8 borderColor"
    
    Border color
    

### brightness<a name="brightness"></a>

!!! variable "isize brightness"
    
    Monitor brightness
    

### bufferOverflows<a name="bufferOverflows"></a>

!!! variable "u64 bufferOverflows"
    
    Number of buffer overflows since power up
    

### bufferSize<a name="bufferSize"></a>

!!! variable "isize bufferSize"
    
    Sample buffer capacity
    

### bufferUnderflows<a name="bufferUnderflows"></a>

!!! variable "u64 bufferUnderflows"
    
    Number of buffer underflows since power up
    

### button<a name="button"></a>

!!! variable "bool button"
    
    State of a joystick
    The fire button is pressed
    

### button1<a name="button1"></a>

!!! variable "const char &#42;button1"
    
    Label of button 1
    

### button2<a name="button2"></a>

!!! variable "const char &#42;button2"
    
    Label of button 2
    

### buttons<a name="buttons"></a>

!!! variable "isize buttons"
    
    Number of buttons
    

### canvasFastPath<a name="canvasFastPath"></a>

!!! variable "isize canvasFastPath"
    
    Performance statistics of the graphics synthesizer
    Number of canvas drawings that took the fast path
    

### canvasSlowPath<a name="canvasSlowPath"></a>

!!! variable "isize canvasSlowPath"
    
    Number of canvas drawings that took the slow path
    

### charMemoryAddr<a name="charMemoryAddr"></a>

!!! variable "u16 charMemoryAddr"
    
    Address of the character memory
    

### charen<a name="charen"></a>

!!! variable "bool charen"
    
    CHAREN bit of the processor port
    

### checkSBCollisions<a name="checkSBCollisions"></a>

!!! variable "bool checkSBCollisions"
    
    Detect sprite-background collisions
    

### checkSSCollisions<a name="checkSSCollisions"></a>

!!! variable "bool checkSSCollisions"
    
    Detect sprite-sprite collisions
    

### cia1Progress<a name="cia1Progress"></a>

!!! variable "Cycle cia1Progress"
    
    Clock of CIA 1 (in cycles)
    

### cia2Progress<a name="cia2Progress"></a>

!!! variable "Cycle cia2Progress"
    
    Clock of CIA 2 (in cycles)
    

### clones<a name="clones"></a>

!!! variable "isize clones"
    
    Number of created run-ahead instances
    

### color<a name="color"></a>

!!! variable "u8 color"
    
    Sprite color
    

### compositor<a name="compositor"></a>

!!! variable "XRayCompositor compositor"
    
    Compositor used to merge the overlay with the picture
    

### config<a name="config"></a>

!!! variable "ConfigMsg config"
    
    Configuration payload
    

### connected<a name="connected"></a>

!!! variable "bool connected"
    
    Connection status
    

### connected<a name="connected"></a>

!!! variable "bool connected"
    
    The drive is connected to the serial port
    

### console<a name="console"></a>

!!! variable "isize console"
    
    State of RetroShell
    Active console
    

### consumedSamples<a name="consumedSamples"></a>

!!! variable "i64 consumedSamples"
    
    Total number of sampels grabbed by the audio backend
    

### contrast<a name="contrast"></a>

!!! variable "isize contrast"
    
    Monitor contrast
    

### count<a name="count"></a>

!!! variable "u16 count"
    
    State of a CIA timer
    Current counter value
    

### counter<a name="counter"></a>

!!! variable "isize counter"
    
    Progress counter
    

### cpu<a name="cpu"></a>

!!! variable "CpuMsg cpu"
    
    CPU payload
    

### cpuLoad<a name="cpuLoad"></a>

!!! variable "double cpuLoad"
    
    Collected run-time data
    Measured CPU load
    

### cpuProgress<a name="cpuProgress"></a>

!!! variable "Cycle cpuProgress"
    
    State of the C64 component
    CPU clock (in cycles)
    

### crc32<a name="crc32"></a>

!!! variable "u32 crc32"
    
    Description of an installed ROM
    CRC32 checksum
    

### ctrl1<a name="ctrl1"></a>

!!! variable "u8 ctrl1"
    
    Control register 1
    

### ctrl2<a name="ctrl2"></a>

!!! variable "u8 ctrl2"
    
    Control register 2
    

### cursorRel<a name="cursorRel"></a>

!!! variable "isize cursorRel"
    
    Relative position of the cursor
    

### cutLayers<a name="cutLayers"></a>

!!! variable "u16 cutLayers"
    
    Layers cut out in XRayMode::XRAY_LAYERS
    Bits 0 - 7: sprites, 9: background, 10: foreground, 11: border
    

### cycle<a name="cycle"></a>

!!! variable "i64 cycle"
    
    Payload of alarm commands
    Cycle at which the alarm fires
    

### cycle<a name="cycle"></a>

!!! variable "i64 cycle"
    
    State of the CPU
    Cycle counter
    

### cyclesPerFrame<a name="cyclesPerFrame"></a>

!!! variable "isize cyclesPerFrame"
    
    Number of cycles per frame
    

### cyclesPerLine<a name="cyclesPerLine"></a>

!!! variable "isize cyclesPerLine"
    
    Number of cycles per raster line
    

### dapInfo<a name="dapInfo"></a>

!!! variable "RemoteServerInfo dapInfo"
    
    State of the DAP server
    

### dasmNumbers<a name="dasmNumbers"></a>

!!! variable "DasmNumbers dasmNumbers"
    
    Configuration of the CPU
    Disassembler number format
    

### dataBegin<a name="dataBegin"></a>

!!! variable "isize dataBegin"
    
    Start of the data block
    

### dataEnd<a name="dataEnd"></a>

!!! variable "isize dataEnd"
    
    End of the data block
    

### decayRate<a name="decayRate"></a>

!!! variable "u8 decayRate"
    
    Envelope decay rate
    

### delay<a name="delay"></a>

!!! variable "double delay"
    
    Delay in seconds
    

### delayedLpIrqs<a name="delayedLpIrqs"></a>

!!! variable "bool delayedLpIrqs"
    
    Light pen interrupts are delayed
    

### denBit<a name="denBit"></a>

!!! variable "bool denBit"
    
    Display enable bit
    

### device<a name="device"></a>

!!! variable "UserPortDevice device"
    
    Configuration of the user port
    User port device
    

### device<a name="device"></a>

!!! variable "CommunicationDevice device"
    
    Configuration of the RS232 adapter
    Connected RS232 device
    

### dir<a name="dir"></a>

!!! variable "u8 dir"
    
    Data direction register (DDRA or DDRB)
    

### direction<a name="direction"></a>

!!! variable "TrafficDirection direction"
    
    Tells whether the packet was received or sent
    

### disalignment<a name="disalignment"></a>

!!! variable "bool disalignment"
    
    Cathode ray disalignment
    

### disalignmentH<a name="disalignmentH"></a>

!!! variable "isize disalignmentH"
    
    Horizontal cathode ray shift
    

### disalignmentV<a name="disalignmentV"></a>

!!! variable "isize disalignmentV"
    
    Vertical cathode ray shift
    

### displayMode<a name="displayMode"></a>

!!! variable "DisplayMode displayMode"
    
    Display mode
    

### displayState<a name="displayState"></a>

!!! variable "bool displayState"
    
    Display state (display or idle)
    

### dmaChannel<a name="dmaChannel"></a>

!!! variable "bool dmaChannel[6]"
    
    Visualize the DMA channel (one flag per channel)
    

### dmaColor<a name="dmaColor"></a>

!!! variable "u32 dmaColor[XRAY_COLOR_COUNT]"
    
    Colors of the DMA channels (0 - 5) and of the cut-out layers (0 - 10).
    The layers use sprite n : color n, border : 8, foreground : 9,
    background : 10. Hence, the colors of the first channels are shared.
    

### dmaDisplayMode<a name="dmaDisplayMode"></a>

!!! variable "DmaDisplayMode dmaDisplayMode"
    
    DMA display mode
    

### dmaOpacity<a name="dmaOpacity"></a>

!!! variable "u8 dmaOpacity"
    
    Opacity of the DMA overlay
    

### dmaOverlay<a name="dmaOverlay"></a>

!!! variable "bool dmaOverlay"
    
    Enable the DMA overlay
    

### dotMaskBrightness<a name="dotMaskBrightness"></a>

!!! variable "isize dotMaskBrightness"
    
    Dotmask brightness
    

### dotmask<a name="dotmask"></a>

!!! variable "Dotmask dotmask"
    
    Dotmask
    

### drive<a name="drive"></a>

!!! variable "DriveMsg drive"
    
    Drive payload
    

### droppedFrames<a name="droppedFrames"></a>

!!! variable "isize droppedFrames"
    
    Statistics of the video port
    Number of frames that were not grabbed in time
    

### dx<a name="dx"></a>

!!! variable "u8 dx"
    
    Horizontal scroll value
    

### dy<a name="dy"></a>

!!! variable "u8 dy"
    
    Vertical scroll value
    

### ejectDelay<a name="ejectDelay"></a>

!!! variable "isize ejectDelay"
    
    Disk ejection delay
    

### ejectVolume<a name="ejectVolume"></a>

!!! variable "u8 ejectVolume"
    
    Disk ejection volume
    

### enable<a name="enable"></a>

!!! variable "bool enable"
    
    Configuration of a remote server
    Enable status
    

### enabled<a name="enabled"></a>

!!! variable "bool enabled"
    
    The guard is enabled
    

### enabled<a name="enabled"></a>

!!! variable "bool enabled"
    
    Configuration of a SID chip
    Indicates if this SID is plugged in
    

### enabled<a name="enabled"></a>

!!! variable "bool enabled"
    
    The sprite is enabled
    

### engine<a name="engine"></a>

!!! variable "SIDEngine engine"
    
    Emulation engine
    

### eventId<a name="eventId"></a>

!!! variable "EventID eventId"
    
    Identifier of the scheduled event (0 if the slot is empty)
    

### eventName<a name="eventName"></a>

!!! variable "const char &#42;eventName"
    
    Name of the scheduled event
    

### expandX<a name="expandX"></a>

!!! variable "bool expandX"
    
    Horizontal expansion
    

### expandY<a name="expandY"></a>

!!! variable "bool expandY"
    
    Vertical expansion
    

### exrom<a name="exrom"></a>

!!! variable "bool exrom"
    
    State of the memory
    State of the EXROM line
    

### exromLineInCrtFile<a name="exromLineInCrtFile"></a>

!!! variable "bool exromLineInCrtFile"
    
    Value of the EXROM line stored in the CRT file
    

### extraColor1<a name="extraColor1"></a>

!!! variable "u8 extraColor1"
    
    Extra color 1 (multicolor mode)
    

### extraColor2<a name="extraColor2"></a>

!!! variable "u8 extraColor2"
    
    Extra color 2 (multicolor mode)
    

### fillLevel<a name="fillLevel"></a>

!!! variable "double fillLevel"
    
    Audio statistics
    Current ring buffer fill level
    

### filter<a name="filter"></a>

!!! variable "bool filter"
    
    Use filter
    

### filterCutoff<a name="filterCutoff"></a>

!!! variable "u16 filterCutoff"
    
    Filter cutoff frequency
    

### filterEnableBit<a name="filterEnableBit"></a>

!!! variable "bool filterEnableBit"
    
    The voice passes through the filter
    

### filterEnableBits<a name="filterEnableBits"></a>

!!! variable "u8 filterEnableBits"
    
    Voices routed through the filter
    

### filterModeBits<a name="filterModeBits"></a>

!!! variable "u8 filterModeBits"
    
    Filter mode bits
    

### filterResonance<a name="filterResonance"></a>

!!! variable "u8 filterResonance"
    
    Filter resonance
    

### filterType<a name="filterType"></a>

!!! variable "u8 filterType"
    
    Filter type
    

### firstSectorNr<a name="firstSectorNr"></a>

!!! variable "Sector firstSectorNr"
    
    Logical number of first sector in track
    

### fnv64<a name="fnv64"></a>

!!! variable "u64 fnv64"
    
    FNV-64 checksum
    

### fps<a name="fps"></a>

!!! variable "double fps"
    
    Measured frames per seconds
    

### fps<a name="fps"></a>

!!! variable "double fps"
    
    Frames per second
    

### frame<a name="frame"></a>

!!! variable "i64 frame"
    
    Frame number
    

### frameBufferHeight<a name="frameBufferHeight"></a>

!!! variable "isize frameBufferHeight"
    
    Height of the host frame buffer (in pixels)
    

### frameBufferWidth<a name="frameBufferWidth"></a>

!!! variable "isize frameBufferWidth"
    
    Width of the host frame buffer (in pixels)
    

### frameFF<a name="frameFF"></a>

!!! variable "FrameFlipflops frameFF"
    
    Frame flip-flops
    

### frameRel<a name="frameRel"></a>

!!! variable "long frameRel"
    
    Trigger frame relative to the current frame
    

### frequency<a name="frequency"></a>

!!! variable "u16 frequency"
    
    Frequency value
    

### frequency<a name="frequency"></a>

!!! variable "isize frequency"
    
    Clock frequency (in Hz)
    

### game<a name="game"></a>

!!! variable "bool game"
    
    State of the GAME line
    

### gameLineInCrtFile<a name="gameLineInCrtFile"></a>

!!! variable "bool gameLineInCrtFile"
    
    Value of the GAME line stored in the CRT file
    

### gateBit<a name="gateBit"></a>

!!! variable "bool gateBit"
    
    Gate bit
    

### glueLogic<a name="glueLogic"></a>

!!! variable "GlueLogic glueLogic"
    
    Glue logic type
    

### grayCodeBug<a name="grayCodeBug"></a>

!!! variable "bool grayCodeBug"
    
    The chip has the gray-code bug
    

### grayDotBug<a name="grayDotBug"></a>

!!! variable "bool grayDotBug"
    
    Emulate the gray-dot bug
    

### greenLED<a name="greenLED"></a>

!!! variable "bool greenLED"
    
    The green LED is on
    

### hCenter<a name="hCenter"></a>

!!! variable "isize hCenter"
    
    Horizontal center
    

### hZoom<a name="hZoom"></a>

!!! variable "isize hZoom"
    
    Horizontal zoom
    

### halftrack<a name="halftrack"></a>

!!! variable "Halftrack halftrack"
    
    Current head position (halftrack)
    

### hardSync<a name="hardSync"></a>

!!! variable "bool hardSync"
    
    Hard sync is on
    

### hasDisk<a name="hasDisk"></a>

!!! variable "bool hasDisk"
    
    A disk is inserted
    

### hasModifiedDisk<a name="hasModifiedDisk"></a>

!!! variable "bool hasModifiedDisk"
    
    The inserted disk has unsaved changes
    

### hasProtectedDisk<a name="hasProtectedDisk"></a>

!!! variable "bool hasProtectedDisk"
    
    The inserted disk is write-protected
    

### hasRom<a name="hasRom"></a>

!!! variable "bool hasRom"
    
    A ROM is installed
    

### hasTape<a name="hasTape"></a>

!!! variable "bool hasTape"
    
    Datasette current state
    Is a tape inserted?
    

### hasUnmodifiedDisk<a name="hasUnmodifiedDisk"></a>

!!! variable "bool hasUnmodifiedDisk"
    
    The inserted disk has no unsaved changes
    

### hasUnprotectedDisk<a name="hasUnprotectedDisk"></a>

!!! variable "bool hasUnprotectedDisk"
    
    The inserted disk is not write-protected
    

### headerBegin<a name="headerBegin"></a>

!!! variable "isize headerBegin"
    
    Position of a sector on a track (in bits)
    Start of the sector header
    

### headerEnd<a name="headerEnd"></a>

!!! variable "isize headerEnd"
    
    End of the sector header
    

### heatmap<a name="heatmap"></a>

!!! variable "bool heatmap"
    
    Count memory accesses for the heatmap
    

### hideSprites<a name="hideSprites"></a>

!!! variable "bool hideSprites"
    
    Sprites
    

### highestSector<a name="highestSector"></a>

!!! variable "static const isize highestSector"
    
    Highest sector number (numbering starts with 0)
    

### hiram<a name="hiram"></a>

!!! variable "bool hiram"
    
    HIRAM bit of the processor port
    

### hits<a name="hits"></a>

!!! variable "isize hits"
    
    Number of times the guard was hit
    

### hour<a name="hour"></a>

!!! variable "u8 hour"
    
    Hours
    

### hpos<a name="hpos"></a>

!!! variable "long hpos"
    
    Raster cycle at which the event triggers
    

### hpos<a name="hpos"></a>

!!! variable "long hpos"
    
    Horizontal beam position (raster cycle)
    

### icr<a name="icr"></a>

!!! variable "u8 icr"
    
    Interrupt control register (pending interrupts)
    

### id<a name="id"></a>

!!! variable "isize id"
    
    The component the option belongs to
    

### id<a name="id"></a>

!!! variable "isize id"
    
    State of a drive
    Number of the drive
    

### idleCycles<a name="idleCycles"></a>

!!! variable "Cycle idleCycles"
    
    Number of cycles spent in the idle state
    

### idlePercentage<a name="idlePercentage"></a>

!!! variable "double idlePercentage"
    
    Share of idle cycles
    

### idleSamples<a name="idleSamples"></a>

!!! variable "i64 idleSamples"
    
    Total number of samples produced without executing SID
    

### idleSince<a name="idleSince"></a>

!!! variable "Cycle idleSince"
    
    Cycle at which the idle state started
    

### idleSleep<a name="idleSleep"></a>

!!! variable "bool idleSleep"
    
    Enter the idle state while the chip is not in use
    

### ignore<a name="ignore"></a>

!!! variable "isize ignore"
    
    Number of hits that are ignored before the guard triggers
    

### ignores<a name="ignores"></a>

!!! variable "isize ignores"
    
    Ignore count
    

### imr<a name="imr"></a>

!!! variable "u8 imr"
    
    Interrupt mask register
    

### imr<a name="imr"></a>

!!! variable "u8 imr"
    
    Interrupt mask register
    

### insertDelay<a name="insertDelay"></a>

!!! variable "isize insertDelay"
    
    Disk insertion delay
    

### insertVolume<a name="insertVolume"></a>

!!! variable "u8 insertVolume"
    
    Disk insertion volume
    

### intLine<a name="intLine"></a>

!!! variable "bool intLine"
    
    The interrupt line is active
    

### irq<a name="irq"></a>

!!! variable "u8 irq"
    
    State of the IRQ lines
    

### irqLine<a name="irqLine"></a>

!!! variable "u16 irqLine"
    
    Raster line that triggers a raster interrupt
    

### irr<a name="irr"></a>

!!! variable "u8 irr"
    
    Interrupt request register
    

### isCommodoreRom<a name="isCommodoreRom"></a>

!!! variable "bool isCommodoreRom"
    
    The ROM is an original Commodore ROM
    

### isMega65Rom<a name="isMega65Rom"></a>

!!! variable "bool isMega65Rom"
    
    The ROM is an OpenROM of the MEGA65 project
    

### isPatchedRom<a name="isPatchedRom"></a>

!!! variable "bool isPatchedRom"
    
    The ROM is a patched variant
    

### jammed<a name="jammed"></a>

!!! variable "bool jammed"
    
    The CPU is jammed
    

### keycode<a name="keycode"></a>

!!! variable "u8 keycode"
    
    Payload of keyboard commands
    Number of the key
    

### latch<a name="latch"></a>

!!! variable "u16 latch"
    
    Latch value the counter is reloaded from
    

### latch<a name="latch"></a>

!!! variable "TimeOfDay latch"
    
    Latched time
    

### latchedLPX<a name="latchedLPX"></a>

!!! variable "u8 latchedLPX"
    
    Latched light pen X coordinate
    

### latchedLPY<a name="latchedLPY"></a>

!!! variable "u8 latchedLPY"
    
    Latched light pen Y coordinate
    

### latestGrabbedFrame<a name="latestGrabbedFrame"></a>

!!! variable "i64 latestGrabbedFrame"
    
    State of the video port
    Number of the most recent frame that was grabbed by the host
    

### led<a name="led"></a>

!!! variable "bool led"
    
    The LED is on
    

### leds<a name="leds"></a>

!!! variable "isize leds"
    
    Number of LEDs
    

### length<a name="length"></a>

!!! variable "isize length"
    
    Layout of a track
    Length of the track (in bits)
    

### lengthInBits<a name="lengthInBits"></a>

!!! variable "u16 lengthInBits"
    
    Typical track size in bits
    

### lengthInBytes<a name="lengthInBytes"></a>

!!! variable "u16 lengthInBytes"
    
    Typical track size in bits
    

### line<a name="line"></a>

!!! variable "isize line"
    
    Payload of script messages
    

### linesPerFrame<a name="linesPerFrame"></a>

!!! variable "isize linesPerFrame"
    
    Number of raster lines per frame
    

### list<a name="list"></a>

!!! variable "GuardType list"
    
    Payload of breakpoint and watchpoint commands
    The guard list to modify
    

### loadAddress<a name="loadAddress"></a>

!!! variable "u16 loadAddress"
    
    Address the ROM chip is mapped to
    

### loram<a name="loram"></a>

!!! variable "bool loram"
    
    LORAM bit of the processor port
    

### lpIrqHasOccurred<a name="lpIrqHasOccurred"></a>

!!! variable "bool lpIrqHasOccurred"
    
    A light pen interrupt has occurred in this frame
    

### lpLine<a name="lpLine"></a>

!!! variable "bool lpLine"
    
    State of the light pen line
    

### main<a name="main"></a>

!!! variable "bool main"
    
    Main frame flip-flop
    

### memSelect<a name="memSelect"></a>

!!! variable "u8 memSelect"
    
    Memory control register
    

### memory<a name="memory"></a>

!!! variable "isize memory"
    
    Memory size (in KB)
    

### memoryBankAddr<a name="memoryBankAddr"></a>

!!! variable "u16 memoryBankAddr"
    
    Start address of the VIC bank
    

### min<a name="min"></a>

!!! variable "u8 min"
    
    Minutes
    

### mode<a name="mode"></a>

!!! variable "XRayMode mode"
    
    Configuration of the X-Ray visualizer
    X-Ray mode (off, DMA debugger, layer inspector)
    

### model<a name="model"></a>

!!! variable "MouseModel model"
    
    Configuration of a mouse
    Mouse model
    

### model<a name="model"></a>

!!! variable "DatasetteModel model"
    
    Datasette configuration
    Datasette model
    

### motor<a name="motor"></a>

!!! variable "bool motor"
    
    Is the motor on?
    

### multicolor<a name="multicolor"></a>

!!! variable "bool multicolor"
    
    Multicolor mode
    

### needsExecution<a name="needsExecution"></a>

!!! variable "bool needsExecution"
    
    The cartridge needs to be executed every cycle
    

### next<a name="next"></a>

!!! variable "long next"
    
    Next micro instruction to be executed
    

### nmi<a name="nmi"></a>

!!! variable "u8 nmi"
    
    State of the NMI lines
    

### nr<a name="nr"></a>

!!! variable "i16 nr"
    
    Payload of drive messages
    

### nr<a name="nr"></a>

!!! variable "isize nr"
    
    Number of a guard in the list
    

### nr<a name="nr"></a>

!!! variable "isize nr"
    
    An entry of the traffic log
    Sequence number (unique, monotonically increasing)
    

### numPackets<a name="numPackets"></a>

!!! variable "isize numPackets"
    
    Number of chip packets
    

### offset<a name="offset"></a>

!!! variable "HeadPos offset"
    
    Head offset within the track
    

### oneShot<a name="oneShot"></a>

!!! variable "bool oneShot"
    
    One-shot mode is on
    

### option<a name="option"></a>

!!! variable "i32 option"
    
    Payload of configuration messages
    

### option<a name="option"></a>

!!! variable "Opt option"
    
    Payload of configuration commands
    The option to change
    

### orientation<a name="orientation"></a>

!!! variable "PaddleOrientation orientation"
    
    Configuration of a paddle
    Paddle value mapping scheme
    

### pal<a name="pal"></a>

!!! variable "bool pal"
    
    Properties of a VICII chip revision
    The chip is a PAL chip
    

### palette<a name="palette"></a>

!!! variable "Palette palette"
    
    Configuration of the monitor emulation
    Color palette
    

### pan<a name="pan"></a>

!!! variable "i64 pan[4]"
    
    Channel pan factors
    

### pan<a name="pan"></a>

!!! variable "i16 pan"
    
    Stereo pan of the drive sounds
    

### parCable<a name="parCable"></a>

!!! variable "ParCableType parCable"
    
    Parallel cable
    

### paused<a name="paused"></a>

!!! variable "bool paused"
    
    Indicates if emulation is paused
    

### payload<a name="payload"></a>

!!! variable "string payload"
    
    Packet contents
    

### pbout<a name="pbout"></a>

!!! variable "bool pbout"
    
    The timer output appears on port B
    

### pc<a name="pc"></a>

!!! variable "u16 pc"
    
    Payload of CPU messages (the program counter)
    

### pc<a name="pc"></a>

!!! variable "u16 pc"
    
    Program counter
    

### pc0<a name="pc0"></a>

!!! variable "u16 pc0"
    
    Address of the current instruction
    

### peekSrc<a name="peekSrc"></a>

!!! variable "MemType peekSrc[16]"
    
    Memory type read by the CPU from each 4 KB bank
    

### playKey<a name="playKey"></a>

!!! variable "bool playKey"
    
    Is the play key pressed?
    

### port<a name="port"></a>

!!! variable "isize port"
    
    Payload of mouse commands
    Control port number
    

### port<a name="port"></a>

!!! variable "isize port"
    
    Payload of joystick commands
    Control port number
    

### port<a name="port"></a>

!!! variable "isize port"
    
    Payload of joystick state commands
    Control port number
    

### port<a name="port"></a>

!!! variable "u16 port"
    
    The socket port of this server
    

### port<a name="port"></a>

!!! variable "u8 port"
    
    State of a CIA port
    State of the port pins
    

### portA<a name="portA"></a>

!!! variable "CIAPortInfo portA"
    
    State of a CIA chip
    Port A
    

### portB<a name="portB"></a>

!!! variable "CIAPortInfo portB"
    
    Port B
    

### potX<a name="potX"></a>

!!! variable "u8 potX"
    
    Paddle value X (POTX)
    

### potY<a name="potY"></a>

!!! variable "u8 potY"
    
    Paddle value Y (POTY)
    

### powerGrid<a name="powerGrid"></a>

!!! variable "PowerGrid powerGrid"
    
    Configuration of the power port
    Power grid stability
    

### powerSave<a name="powerSave"></a>

!!! variable "bool powerSave"
    
    Performance
    

### powerSave<a name="powerSave"></a>

!!! variable "bool powerSave"
    
    Take fast paths
    

### powerSave<a name="powerSave"></a>

!!! variable "bool powerSave"
    
    Take fast paths
    

### powerVolume<a name="powerVolume"></a>

!!! variable "u8 powerVolume"
    
    Power switch volume
    

### powered<a name="powered"></a>

!!! variable "bool powered"
    
    Indicates if the emulator is powered on
    

### priority<a name="priority"></a>

!!! variable "bool priority"
    
    The sprite is drawn behind the background
    

### processorPort<a name="processorPort"></a>

!!! variable "u8 processorPort"
    
    Processor port register
    

### processorPortDir<a name="processorPortDir"></a>

!!! variable "u8 processorPortDir"
    
    Processor port data direction register
    

### producedSamples<a name="producedSamples"></a>

!!! variable "i64 producedSamples"
    
    Total number of generated sampleds
    

### promInfo<a name="promInfo"></a>

!!! variable "RemoteServerInfo promInfo"
    
    State of the Prometheus server
    

### pulseWidth<a name="pulseWidth"></a>

!!! variable "u16 pulseWidth"
    
    Pulse width value
    

### quickExitHit<a name="quickExitHit"></a>

!!! variable "isize quickExitHit"
    
    Number of successful quick-exit checks
    

### quickExitMiss<a name="quickExitMiss"></a>

!!! variable "isize quickExitMiss"
    
    Number of failed quick-exit checks
    

### ram<a name="ram"></a>

!!! variable "DriveRam ram"
    
    Drive RAM
    

### ramPattern<a name="ramPattern"></a>

!!! variable "RamPattern ramPattern"
    
    Configuration of the memory
    RAM start-up pattern
    

### rasterCycle<a name="rasterCycle"></a>

!!! variable "u8 rasterCycle"
    
    Current raster cycle
    

### rc<a name="rc"></a>

!!! variable "u8 rc"
    
    Row counter (RC)
    

### rdy<a name="rdy"></a>

!!! variable "bool rdy"
    
    State of the RDY line
    

### reads<a name="reads"></a>

!!! variable "isize reads[65536]"
    
    Memory access statistics
    Number of read accesses per address
    

### redLED<a name="redLED"></a>

!!! variable "bool redLED"
    
    The red LED is on
    

### refreshRate<a name="refreshRate"></a>

!!! variable "isize refreshRate"
    
    Properties of the host machine
    Refresh rate of the host display
    

### reg<a name="reg"></a>

!!! variable "u8 reg"
    
    Port register (PRA or PRB)
    

### reg<a name="reg"></a>

!!! variable "u8 reg[7]"
    
    State of a SID voice
    Raw voice registers
    

### releaseRate<a name="releaseRate"></a>

!!! variable "u8 releaseRate"
    
    Envelope release rate
    

### resyncs<a name="resyncs"></a>

!!! variable "isize resyncs"
    
    Number of out-of-sync conditions
    

### reuSpeed<a name="reuSpeed"></a>

!!! variable "isize reuSpeed"
    
    Configuration of the expansion port
    Speed of the REU
    

### revision<a name="revision"></a>

!!! variable "CIARev revision"
    
    Configuration of a CIA chip
    Chip revision
    

### revision<a name="revision"></a>

!!! variable "SIDRevision revision"
    
    Chip revision (MOS 6581 or MOS 8580)
    

### revision<a name="revision"></a>

!!! variable "VICIIRev revision"
    
    Configuration of the VICII chip
    Chip revision
    

### revision<a name="revision"></a>

!!! variable "const char &#42;revision"
    
    Revision of the ROM
    

### ringMod<a name="ringMod"></a>

!!! variable "bool ringMod"
    
    Ring modulation is on
    

### rpcInfo<a name="rpcInfo"></a>

!!! variable "RemoteServerInfo rpcInfo"
    
    State of the RPC server
    

### rshInfo<a name="rshInfo"></a>

!!! variable "RemoteServerInfo rshInfo"
    
    State of the remote servers
    State of the RetroShell server
    

### runAhead<a name="runAhead"></a>

!!! variable "isize runAhead"
    
    Number of run-ahead frames (0 = run-ahead is disabled)
    

### running<a name="running"></a>

!!! variable "bool running"
    
    Indicates if the emulator is running
    

### running<a name="running"></a>

!!! variable "bool running"
    
    The timer is running
    

### sampleRate<a name="sampleRate"></a>

!!! variable "isize sampleRate"
    
    Audio sample rate of the host computer
    

### sampling<a name="sampling"></a>

!!! variable "SamplingMethod sampling"
    
    Sampling method
    

### saturation<a name="saturation"></a>

!!! variable "isize saturation"
    
    Monitor saturation
    

### saveRoms<a name="saveRoms"></a>

!!! variable "bool saveRoms"
    
    Include ROMs in snapshots
    

### saveRoms<a name="saveRoms"></a>

!!! variable "bool saveRoms"
    
    Save Roms
    

### sbCollision<a name="sbCollision"></a>

!!! variable "bool sbCollision"
    
    A sprite-background collision was detected
    

### scanline<a name="scanline"></a>

!!! variable "u16 scanline"
    
    State of the VICII chip
    Current raster line
    

### scanlineBrightness<a name="scanlineBrightness"></a>

!!! variable "isize scanlineBrightness"
    
    Scanline brightness
    

### scanlineWeight<a name="scanlineWeight"></a>

!!! variable "isize scanlineWeight"
    
    Scanline weight
    

### scanlines<a name="scanlines"></a>

!!! variable "Scanlines scanlines"
    
    Scanlines
    

### screenGeometry<a name="screenGeometry"></a>

!!! variable "ScreenGeometry screenGeometry"
    
    Screen geometry
    

### screenMemoryAddr<a name="screenMemoryAddr"></a>

!!! variable "u16 screenMemoryAddr"
    
    Address of the screen memory
    

### script<a name="script"></a>

!!! variable "ScriptMsg script"
    
    Script payload
    

### sdr<a name="sdr"></a>

!!! variable "u8 sdr"
    
    Serial data register (SDR)
    

### sec<a name="sec"></a>

!!! variable "u8 sec"
    
    Seconds
    

### sectorLayout<a name="sectorLayout"></a>

!!! variable "SectorLayout sectorLayout[22]"
    
    Layout of the sectors
    

### sectors<a name="sectors"></a>

!!! variable "u8  sectors"
    
    Default properties of a track
    Typical number of sectors in this track
    

### server<a name="server"></a>

!!! variable "ServerType server"
    
    The server that received or sent the packet
    

### shakeDetection<a name="shakeDetection"></a>

!!! variable "bool shakeDetection"
    
    Detect a shaken mouse
    

### size<a name="size"></a>

!!! variable "u16 size"
    
    Description of a cartridge ROM chip
    Size of the ROM chip (in bytes)
    

### slot<a name="slot"></a>

!!! variable "EventSlot slot"
    
    State of a single event slot
    The event slot
    

### slotInfo<a name="slotInfo"></a>

!!! variable "EventSlotInfo slotInfo[SLOT_COUNT]"
    
    Events
    

### snapshot<a name="snapshot"></a>

!!! variable "void &#42;snapshot"
    
    Payload of snapshot messages
    

### snapshot<a name="snapshot"></a>

!!! variable "SnapshotMsg snapshot"
    
    Snapshot payload
    

### sp<a name="sp"></a>

!!! variable "u8 sp"
    
    Stack pointer
    

### speedBoost<a name="speedBoost"></a>

!!! variable "isize speedBoost"
    
    Emulator speed in percent (100 is native speed)
    

### speedZone<a name="speedZone"></a>

!!! variable "u8  speedZone"
    
    Default speed zone for this track
    

### spinning<a name="spinning"></a>

!!! variable "bool spinning"
    
    The disk is spinning
    

### spriteFastPath<a name="spriteFastPath"></a>

!!! variable "isize spriteFastPath"
    
    Number of sprite drawings that took the fast path
    

### spriteSlowPath<a name="spriteSlowPath"></a>

!!! variable "isize spriteSlowPath"
    
    Number of sprite drawings that took the slow path
    

### sr<a name="sr"></a>

!!! variable "u8 sr"
    
    Status register
    

### ssCollision<a name="ssCollision"></a>

!!! variable "bool ssCollision"
    
    A sprite-sprite collision was detected
    

### ssr<a name="ssr"></a>

!!! variable "u8 ssr"
    
    Serial shift register
    

### stagger<a name="stagger"></a>

!!! variable "double stagger"
    
    Relative position of first bit (from Hoxs64)
    

### state<a name="state"></a>

!!! variable "u32 state"
    
    Bits 0-4: current state, bits 8-12: previous state (up, down, left, right, fire)
    

### state<a name="state"></a>

!!! variable "ExecState state"
    
    The current emulator state
    The current emulator state
    

### state<a name="state"></a>

!!! variable "SrvState state"
    
    State of a remote server
    State of the server
    

### stepVolume<a name="stepVolume"></a>

!!! variable "u8 stepVolume"
    
    Head step volume
    

### str<a name="str"></a>

!!! variable "const char &#42;str"
    
    Optional text (may be null)
    

### subtitle<a name="subtitle"></a>

!!! variable "const char &#42;subtitle"
    
    Subtitle of the ROM
    

### supported<a name="supported"></a>

!!! variable "bool supported"
    
    The cartridge type is supported
    

### suspended<a name="suspended"></a>

!!! variable "bool suspended"
    
    Indicates if the emulator is in suspended state
    

### sustainRate<a name="sustainRate"></a>

!!! variable "u8 sustainRate"
    
    Envelope sustain level
    

### swapDelay<a name="swapDelay"></a>

!!! variable "isize swapDelay"
    
    Disk swap delay
    

### switchLeft<a name="switchLeft"></a>

!!! variable "const char &#42;switchLeft"
    
    Label of the left switch position
    

### switchNeutral<a name="switchNeutral"></a>

!!! variable "const char &#42;switchNeutral"
    
    Label of the neutral switch position
    

### switchPos<a name="switchPos"></a>

!!! variable "isize switchPos"
    
    Position of the switch
    

### switchRight<a name="switchRight"></a>

!!! variable "const char &#42;switchRight"
    
    Label of the right switch position
    

### switchedOn<a name="switchedOn"></a>

!!! variable "bool switchedOn"
    
    Power switch
    

### switches<a name="switches"></a>

!!! variable "isize switches"
    
    Number of switches
    

### tape<a name="tape"></a>

!!! variable "void &#42;tape"
    
    Payload of tape commands
    Pointer to the tape file
    

### tenth<a name="tenth"></a>

!!! variable "u8 tenth"
    
    Time-of-day value in BCD format
    Tenths of a second
    

### testBit<a name="testBit"></a>

!!! variable "bool testBit"
    
    Test bit
    

### texFormat<a name="texFormat"></a>

!!! variable "TexFormat texFormat"
    
    Texture format
    

### time<a name="time"></a>

!!! variable "TimeOfDay time"
    
    State of the time-of-day clock
    Current time
    

### time<a name="time"></a>

!!! variable "i64 time"
    
    Host time stamp (milliseconds since epoch)
    

### timerA<a name="timerA"></a>

!!! variable "CIATimerInfo timerA"
    
    Timer A
    

### timerB<a name="timerB"></a>

!!! variable "CIATimerInfo timerB"
    
    Timer B
    

### timerBBug<a name="timerBBug"></a>

!!! variable "bool timerBBug"
    
    Emulate the Timer-B bug
    

### title<a name="title"></a>

!!! variable "const char &#42;title"
    
    Title of the cartridge
    

### title<a name="title"></a>

!!! variable "const char &#42;title"
    
    Title of the ROM
    

### tod<a name="tod"></a>

!!! variable "TODInfo tod"
    
    Time-of-day clock
    

### todIntEnable<a name="todIntEnable"></a>

!!! variable "bool todIntEnable"
    
    The TOD alarm interrupt is enabled
    

### toggle<a name="toggle"></a>

!!! variable "bool toggle"
    
    Toggle mode is on
    

### totalCycles<a name="totalCycles"></a>

!!! variable "Cycle totalCycles"
    
    Usage statistics of a CIA chip
    Total number of executed cycles
    

### trackLayout<a name="trackLayout"></a>

!!! variable "TrackLayout trackLayout[85]"
    
    Layout of a disk
    Layout of the tracks
    

### tracking<a name="tracking"></a>

!!! variable "bool tracking"
    
    Indicates if track mode is enabled
    

### transport<a name="transport"></a>

!!! variable "TransportProtocol transport"
    
    The transport protocol of this server
    

### trigger<a name="trigger"></a>

!!! variable "Cycle trigger"
    
    Trigger cycle of the event
    

### triggerRel<a name="triggerRel"></a>

!!! variable "Cycle triggerRel"
    
    Time until the event triggers (in cycles)
    

### type<a name="type"></a>

!!! variable "CartridgeType type"
    
    Properties of a cartridge type
    Cartridge type
    

### type<a name="type"></a>

!!! variable "CartridgeType type"
    
    State of an attached cartridge
    DEPRECATED
    

### type<a name="type"></a>

!!! variable "Msg type"
    
    A message sent from the emulator to the GUI
    Type of the message
    

### type<a name="type"></a>

!!! variable "u8 type"
    
    TAP file format
    

### type<a name="type"></a>

!!! variable "DriveType type"
    
    Drive model
    

### ultimax<a name="ultimax"></a>

!!! variable "bool ultimax"
    
    Ultimax memory mode is active
    

### upscaler<a name="upscaler"></a>

!!! variable "Upscaler upscaler"
    
    Effects
    

### vCenter<a name="vCenter"></a>

!!! variable "isize vCenter"
    
    Vertical center
    

### vZoom<a name="vZoom"></a>

!!! variable "isize vZoom"
    
    Vertical zoom
    

### value<a name="value"></a>

!!! variable "i64 value"
    
    Message payload
    

### value<a name="value"></a>

!!! variable "i64 value"
    
    The new value
    

### value<a name="value"></a>

!!! variable "i64 value"
    
    Value reported by the alarm
    

### value<a name="value"></a>

!!! variable "u32 value"
    
    The time as a single 32-bit value
    

### value2<a name="value2"></a>

!!! variable "i64 value2"
    
    Generic payload
    

### vblank<a name="vblank"></a>

!!! variable "bool vblank"
    
    Vertical blanking is active
    

### vc<a name="vc"></a>

!!! variable "u16 vc"
    
    Video counter (VC)
    

### vcBase<a name="vcBase"></a>

!!! variable "u16 vcBase"
    
    Video counter base (VCBASE)
    

### velocity<a name="velocity"></a>

!!! variable "isize velocity"
    
    Mouse velocity
    

### verbose<a name="verbose"></a>

!!! variable "bool verbose"
    
    If true, transmitted packets are shown in RetroShell
    

### vertical<a name="vertical"></a>

!!! variable "bool vertical"
    
    State of the frame flip-flops
    Vertical frame flip-flop
    

### vicPeekSrc<a name="vicPeekSrc"></a>

!!! variable "MemType vicPeekSrc[16]"
    
    Memory type read by the VICII from each 4 KB bank
    

### visibleLines<a name="visibleLines"></a>

!!! variable "isize visibleLines"
    
    Number of visible raster lines
    

### vmli<a name="vmli"></a>

!!! variable "u8 vmli"
    
    Video matrix line index (VMLI)
    

### voice<a name="voice"></a>

!!! variable "VoiceInfo voice[3]"
    
    State of the three voices
    

### vol<a name="vol"></a>

!!! variable "i64 vol[4]"
    
    Configuration of the audio port
    Channel volumes
    

### volL<a name="volL"></a>

!!! variable "i64 volL"
    
    Master volume (left)
    

### volR<a name="volR"></a>

!!! variable "i64 volR"
    
    Master volume (right)
    

### volume<a name="volume"></a>

!!! variable "u8 volume"
    
    State of a SID chip
    Master volume
    

### vpos<a name="vpos"></a>

!!! variable "long vpos"
    
    Raster line at which the event triggers
    

### vpos<a name="vpos"></a>

!!! variable "long vpos"
    
    Vertical beam position (raster line)
    

### vsync<a name="vsync"></a>

!!! variable "bool vsync"
    
    Vertical Synchronization
    

### warpBoot<a name="warpBoot"></a>

!!! variable "isize warpBoot"
    
    Configuration of the C64 component
    After a reset, the emulator runs in warp mode for this amout of seconds
    

### warpMode<a name="warpMode"></a>

!!! variable "Warp warpMode"
    
    Warp mode
    

### warping<a name="warping"></a>

!!! variable "bool warping"
    
    Indicates if warp mode is currently on
    

### waveform<a name="waveform"></a>

!!! variable "u8 waveform"
    
    Waveform selection bits
    

### whiteNoise<a name="whiteNoise"></a>

!!! variable "bool whiteNoise"
    
    Configuration of the video port
    Show white noise while the machine is off
    

### writes<a name="writes"></a>

!!! variable "isize writes[65536]"
    
    Number of write accesses per address
    

### writing<a name="writing"></a>

!!! variable "bool writing"
    
    The drive is writing
    

### x<a name="x"></a>

!!! variable "double x"
    
    X coordinate
    

### x<a name="x"></a>

!!! variable "u8 x"
    
    X register
    

### x<a name="x"></a>

!!! variable "u16 x"
    
    State of a sprite
    Horizontal position
    

### xCounter<a name="xCounter"></a>

!!! variable "u16 xCounter"
    
    Horizontal position counter
    

### y<a name="y"></a>

!!! variable "double y"
    
    Y coordinate
    

### y<a name="y"></a>

!!! variable "u8 y"
    
    Y register
    

### y<a name="y"></a>

!!! variable "u8 y"
    
    Vertical position
    

### yCounter<a name="yCounter"></a>

!!! variable "u32 yCounter"
    
    Raster line counter
    

