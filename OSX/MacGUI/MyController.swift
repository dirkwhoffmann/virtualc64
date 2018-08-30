//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

class MyController : NSWindowController {

    /// Proxy object.
    /// Implements a bridge between the emulator which is written in C++ and the
    /// GUI which is written in Swift. Because Swift cannot interact with C++ directly,
    /// the proxy is written in Objective-C.
    var c64: C64Proxy!
    
    /// Game pad manager
    var gamePadManager: GamePadManager!
    
    /// Keyboard controller
    var keyboardcontroller: KeyboardController!
    
    /// Virtual C64 keyboard (opened as a separate window)
    var virtualKeyboard: VirtualKeyboardController? = nil

    /// Virtual C64 keyboard (opened as a sheet)
    var virtualKeyboardSheet: VirtualKeyboardController? = nil

    /// Loop timer
    /// The timer fires 60 times a second and executes all tasks that need to be done
    /// perdiodically (e.g., updating the speedometer and the debug panels)
    var timer: Timer?
    
    // Timer lock
    var timerLock: NSLock!
    
    /// Used inside the timer function to fine tune timed events
    var animationCounter = 0
    
    /// Speedometer to measure clock frequence and frames per second
    var speedometer: Speedometer!
    
    /// Current keyboard modifier flags
    /// These flags tell us if one of the special keysare currently pressed.
    /// The flags are utilized, e.g., to alter behaviour when a key on the
    /// TouchBar is pressed.
    var modifierFlags: NSEvent.ModifierFlags = .init(rawValue: 0)
    
    /// Current mouse coordinate
    var mouseXY = NSPoint(x: 0, y: 0)
    
    /// Indicates if mouse is currently hidden
    var hideMouse = false
    
    /// Indicates if a status bar is shown
    var statusBar = true
    
    /// Selected game pad slot for joystick in port A
    var gamepadSlot1 = 0
    
    /// Selected game pad slot for joystick in port B
    var gamepadSlot2 = 0
    
    /// Default image for USB devices
    var genericDeviceImage: NSImage?

    /// Indicates if the emulator should pause when it looses focus.
    var pauseInBackground = false

    /// Remembers if the emulator was running or paused when it lose focus.
    /// Needed to implement the pauseInBackground feature.
    var pauseInBackgroundSavedState = true

    /// Indicates if the user dialog should be skipped when opening archives.
    var autoMount = false
    

    //
    // Outlets
    //
    
    // Main screen
    @IBOutlet weak var metalScreen: MetalView!
    @IBOutlet weak var debugger: NSDrawer!
    
    // Bottom bar
    @IBOutlet weak var greenLED1: NSButton!
    @IBOutlet weak var greenLED2: NSButton!
    @IBOutlet weak var redLED1: NSButton!
    @IBOutlet weak var redLED2: NSButton!
    @IBOutlet weak var progress1: NSProgressIndicator!
    @IBOutlet weak var progress2: NSProgressIndicator!
    @IBOutlet weak var diskIcon1: NSButton!
    @IBOutlet weak var diskIcon2: NSButton!
    @IBOutlet weak var cartridgeIcon: NSButton!
    @IBOutlet weak var tapeIcon: NSButton!
    @IBOutlet weak var tapeProgress: NSProgressIndicator!
    @IBOutlet weak var clockSpeed: NSTextField!
    @IBOutlet weak var clockSpeedBar: NSLevelIndicator!
    @IBOutlet weak var warpIcon: NSButton!
    
    // Toolbar
    @IBOutlet weak var controlPort1: NSPopUpButton!
    @IBOutlet weak var controlPort2: NSPopUpButton!
    
    // Menu
    // @IBOutlet weak var recentDisksMenu: NSMenu!
    
    // Debug panel (commons)
    var hex = true
    @IBOutlet weak var debugPanel: NSTabView!
    @IBOutlet weak var dezHexSelector: NSMatrix!
    @IBOutlet weak var stopAndGoButton: NSButton!
    @IBOutlet weak var stepIntoButton: NSButton!
    @IBOutlet weak var stepOverButton: NSButton!
    
    // Debug panel (CPU)
    @IBOutlet weak var cpuTableView: CpuTableView!
    @IBOutlet weak var cpuTraceView: CpuTraceView!
    @IBOutlet weak var pc: NSTextField!
    @IBOutlet weak var sp: NSTextField!
    @IBOutlet weak var a: NSTextField!
    @IBOutlet weak var x: NSTextField!
    @IBOutlet weak var y: NSTextField!
    @IBOutlet weak var nflag: NSButton!
    @IBOutlet weak var zflag: NSButton!
    @IBOutlet weak var cflag: NSButton!
    @IBOutlet weak var iflag: NSButton!
    @IBOutlet weak var bflag: NSButton!
    @IBOutlet weak var dflag: NSButton!
    @IBOutlet weak var vflag: NSButton!
    @IBOutlet weak var breakAt: NSTextField!
    
    // Debug panel (Memory)
    @IBOutlet weak var memTableView: MemTableView!
    
    // Debug panel (CIA)
    @IBOutlet weak var ciaSelector: NSSegmentedControl!
    
    @IBOutlet weak var ciaPA: NSTextField!
    @IBOutlet weak var ciaPAbinary: NSTextField!
    @IBOutlet weak var ciaPRA: NSTextField!
    @IBOutlet weak var ciaDDRA: NSTextField!
    
    @IBOutlet weak var ciaPB: NSTextField!
    @IBOutlet weak var ciaPBbinary: NSTextField!
    @IBOutlet weak var ciaPRB: NSTextField!
    @IBOutlet weak var ciaDDRB: NSTextField!
    
    @IBOutlet weak var ciaTimerA: NSTextField!
    @IBOutlet weak var ciaLatchA: NSTextField!
    @IBOutlet weak var ciaRunningA: NSButton!
    @IBOutlet weak var ciaToggleA: NSButton!
    @IBOutlet weak var ciaPBoutA: NSButton!
    @IBOutlet weak var ciaOneShotA: NSButton!
    
    @IBOutlet weak var ciaTimerB: NSTextField!
    @IBOutlet weak var ciaLatchB: NSTextField!
    @IBOutlet weak var ciaRunningB: NSButton!
    @IBOutlet weak var ciaToggleB: NSButton!
    @IBOutlet weak var ciaPBoutB: NSButton!
    @IBOutlet weak var ciaOneShotB: NSButton!
    
    @IBOutlet weak var todHours: NSTextField!
    @IBOutlet weak var todMinutes: NSTextField!
    @IBOutlet weak var todSeconds: NSTextField!
    @IBOutlet weak var todTenth: NSTextField!
    @IBOutlet weak var todIntEnable: NSButton!
    @IBOutlet weak var alarmHours: NSTextField!
    @IBOutlet weak var alarmMinutes: NSTextField!
    @IBOutlet weak var alarmSeconds: NSTextField!
    @IBOutlet weak var alarmTenth: NSTextField!
    
    @IBOutlet weak var ciaIcr: NSTextField!
    @IBOutlet weak var ciaIcrBinary: NSTextField!
    @IBOutlet weak var ciaImr: NSTextField!
    @IBOutlet weak var ciaImrBinary: NSTextField!
    @IBOutlet weak var ciaIntLineLow: NSButton!
    
    // Debug panel (VIC)
    @IBOutlet weak var vicRasterline: NSTextField!
    @IBOutlet weak var vicCycle: NSTextField!
    @IBOutlet weak var vicXCounter: NSTextField!
    @IBOutlet weak var vicBadLine: NSButton!
    @IBOutlet weak var vicDisplayMode: NSPopUpButton!
    @IBOutlet weak var vicBorderColor: NSButton!
    @IBOutlet weak var vicBackgroundColor0: NSButton!
    @IBOutlet weak var vicBackgroundColor1: NSButton!
    @IBOutlet weak var vicBackgroundColor2: NSButton!
    @IBOutlet weak var vicBackgroundColor3: NSButton!
    @IBOutlet weak var vicScreenGeometry: NSPopUpButton!
    @IBOutlet weak var vicDx: NSTextField!
    @IBOutlet weak var vicDXStepper: NSStepper!
    @IBOutlet weak var vicDy: NSTextField!
    @IBOutlet weak var vicDYStepper: NSStepper!
    @IBOutlet weak var vicMemoryBankAddr: NSPopUpButton!
    @IBOutlet weak var vicScreenMemoryAddr: NSPopUpButton!
    @IBOutlet weak var vicCharacterMemoryAddr: NSPopUpButton!
    @IBOutlet weak var vicRasterIrqEnabled: NSButton!
    @IBOutlet weak var vicIrqRasterline: NSTextField!
    @IBOutlet weak var vicIrqLine: NSButton!
    
    @IBOutlet weak var spriteSelector: NSSegmentedControl!
    @IBOutlet weak var spriteEnabled: NSButton!
    @IBOutlet weak var spriteX: NSTextField!
    @IBOutlet weak var spriteY: NSTextField!
    @IBOutlet weak var spriteIsMulticolor: NSPopUpButton!
    @IBOutlet weak var spriteColor: NSButton!
    @IBOutlet weak var spriteExtraColor1: NSButton!
    @IBOutlet weak var spriteExtraColor2: NSButton!
    @IBOutlet weak var spriteExpandX: NSButton!
    @IBOutlet weak var spriteExpandY: NSButton!
    @IBOutlet weak var spritePriority: NSButton!
    @IBOutlet weak var spriteCollidesWithSprite: NSButton!
    @IBOutlet weak var spriteSpriteIrqEnabled: NSButton!
    @IBOutlet weak var spriteCollidesWithBackground: NSButton!
    @IBOutlet weak var spriteBackgroundIrqEnabled: NSButton!
    
    // Debugger (SID panel)
    @IBOutlet weak var volume: NSTextField!
    @IBOutlet weak var potX: NSTextField!
    @IBOutlet weak var potY: NSTextField!
    
    @IBOutlet weak var voiceSelector: NSSegmentedControl!
    @IBOutlet weak var waveform: NSPopUpButton!
    @IBOutlet weak var frequency: NSTextField!
    @IBOutlet weak var pulseWidth: NSTextField!
    @IBOutlet weak var pulseWidthText: NSTextField!
    @IBOutlet weak var attackRate: NSTextField!
    @IBOutlet weak var decayRate: NSTextField!
    @IBOutlet weak var sustainRate: NSTextField!
    @IBOutlet weak var releaseRate: NSTextField!
    @IBOutlet weak var gateBit: NSButton!
    @IBOutlet weak var testBit: NSButton!
    @IBOutlet weak var syncBit: NSButton!
    @IBOutlet weak var ringBit: NSButton!
    
    @IBOutlet weak var filterType: NSPopUpButton!
    @IBOutlet weak var filterCutoff: NSTextField!
    @IBOutlet weak var filterResonance: NSTextField!
    @IBOutlet weak var filter1: NSButton!
    @IBOutlet weak var filter2: NSButton!
    @IBOutlet weak var filter3: NSButton!
    
    @IBOutlet weak var waveformView: WaveformView!
    @IBOutlet weak var audioBufferLevel: NSLevelIndicator!
    @IBOutlet weak var audioBufferLevelText: NSTextField!
    @IBOutlet weak var bufferUnderflows: NSTextField!
    @IBOutlet weak var bufferOverflows: NSTextField!
}

extension MyController {

    // Provides the undo manager
    override open var undoManager: UndoManager? {
        get {
            return metalScreen.undoManager
        }
    }
 
    // Provides the document casted to the correct type
    var mydocument: MyDocument {
        get {
            return document as! MyDocument
        }
    }
    
    //
    // Initialization
    //

    override open func awakeFromNib() {

        track()
        cpuTableView.c = self
        cpuTraceView.c = self
        memTableView.c = self
    }
    
    
    override open func windowDidLoad() {
 
        track()

        // Reset mouse coordinates
        mouseXY = NSZeroPoint
        hideMouse = false
        
        // Create keyboard controller
        keyboardcontroller = KeyboardController(controller: self)
        if (keyboardcontroller == nil) {
            track("Failed to create keyboard controller")
            return
        }

        // Create game pad manager
        gamePadManager = GamePadManager(controller: self)
        if (gamePadManager == nil) {
            track("Failed to create game pad manager")
            return
        }
        gamepadSlot1 = -1 // No gampad assigned
        gamepadSlot2 = -1

        // Setup toolbar, window, and debugger
        configureToolbar()
        configureWindow()
        setupDebugger()
        
        // Get metal running
        metalScreen.setupMetal()
    
        // Load user defaults
        loadUserDefaults()
        
        // Enable message processing (register callback)
        setListener()

        // Create speed monitor and get the timer tunning
        createTimer()
    }
    
    func configureWindow() {
    
        // Add status bar
        window?.autorecalculatesContentBorderThickness(for: .minY)
        window?.setContentBorderThickness(32.0, for: .minY)
        statusBar = true
        
        // Adjust size and enable auto-save for window coordinates
        adjustWindowSize()
        window?.windowController?.shouldCascadeWindows = false // true ?!
        let name = NSWindow.FrameAutosaveName(rawValue: "dirkwhoffmann.de.virtualC64.window")
        window?.setFrameAutosaveName(name)
        
        // Enable fullscreen mode
        window?.collectionBehavior = .fullScreenPrimary
        
        // Disable area tracking
        // trackingArea = nil
    }
    
    func configureToolbar() {
        
        // Get and resize images
        let cutout = NSMakeRect(2, 0, 28, 28)
        
        var none = NSImage(named: NSImage.Name("oxygen_none"))
        none = none?.resizeImage(width: 32, height: 32, cutout: cutout)
        var keyset = NSImage(named: NSImage.Name(rawValue: "oxygen_keys"))
        keyset = keyset?.resizeImage(width: 32, height: 32, cutout: cutout)
        var mouse = NSImage(named: NSImage.Name(rawValue: "oxygen_mouse"))
        mouse = mouse?.resizeImage(width: 32, height: 32, cutout: cutout)
        var gamepad = NSImage(named: NSImage.Name(rawValue: "crystal_gamepad"))
        gamepad = gamepad?.resizeImage(width: 32, height: 32, cutout: cutout)
        genericDeviceImage = gamepad
        
        // Assign images
        controlPort1.item(at: 0)?.image = none
        controlPort1.item(at: 1)?.image = keyset
        controlPort1.item(at: 2)?.image = keyset
        controlPort1.item(at: 3)?.image = mouse
        controlPort1.item(at: 4)?.image = gamepad
        controlPort1.item(at: 5)?.image = gamepad

        // Assign images
        controlPort2.item(at: 0)?.image = none
        controlPort2.item(at: 1)?.image = keyset
        controlPort2.item(at: 2)?.image = keyset
        controlPort2.item(at: 3)?.image = mouse
        controlPort2.item(at: 4)?.image = gamepad
        controlPort2.item(at: 5)?.image = gamepad

        validateJoystickToolbarItems()
    }
    
    func setListener() {
        
        track()
        
        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        c64.setListener(myself) { (ptr, type, data) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                let mType = MessageType(rawValue: UInt32(type))
                myself.processMessage(Message(type: mType, data: data))
                // myself.processMessage(Message(UInt32(msg)))
            }
        }
        
        track("Listener is in place")
    }
    
    func createTimer() {
    
        // Create speed monitor
        speedometer = Speedometer()
        
        // Create timer and speedometer
        timerLock = NSLock()
        timer = Timer.scheduledTimer(timeInterval: 1.0/12, // 12 times a second
                                     target: self,
                                     selector: #selector(timerFunc),
                                     userInfo: nil,
                                     repeats: true)
        
        track("GUI timer is up and running")
    }
 
    
    //
    // Timer and message processing
    //
    
    @objc func timerFunc() {

        precondition(timerLock != nil)
        timerLock.lock()
 
        animationCounter += 1
        
        // Process all pending messages
        /*
        var msg: Message = c64.message()
        while msg != MSG_NONE {
            processMessage(msg)
            msg = c64.message()
        }
        */

        // Do 12 times a second ...
        if (animationCounter % 1) == 0 {
 
            // Refresh debug panel if open
            if c64.isRunning() {
                let state = debugger.state
                if state == NSDrawerState.open || state == NSDrawerState.opening {
                    refresh()
                }
            }
        }
        
        // Do 6 times a second ...
        if (animationCounter % 2) == 0 {
 
            // Update tape progress icon
            // Note: The tape progress icon is not switched on or off by a "push" message,
            // because some games continously switch on and off the datasette motor.
            // This would quickly overflow the message queue.
            if (c64.datasette.motor() && c64.datasette.playKey()) {
                tapeProgress.startAnimation(self)
            } else {
                tapeProgress.stopAnimation(self)
            }

            /* Original code: Why so complicated???
            if ([[c64 datasette] motor] != [c64 tapeBusIsBusy]) {
                if ([[c64 datasette] motor] && [[c64 datasette] playKey]) {
                    [tapeProgress startAnimation:nil];
                    [c64 setTapeBusIsBusy:YES];
                } else {
                    [tapeProgress stopAnimation:nil];
                    [c64 setTapeBusIsBusy:NO];
                }
            }
            */
        }
        
        // Do 3 times a second ...
        if (animationCounter % 4) == 0 {
            speedometer.updateWith(cycle: c64.cycles(), frame: metalScreen.frames)
            let mhz = speedometer.mhz(digits: 2)
            let fps = speedometer.fps(digits: 0)
            clockSpeed.stringValue = String(format:"%.2f MHz %.0f fps", mhz, fps)
            clockSpeedBar.doubleValue = 10 * mhz
        
            // Let the cursor disappear in fullscreen mode
            if metalScreen.fullscreen &&
                CGEventSource.secondsSinceLastEventType(.combinedSessionState,
                                                        eventType: .mouseMoved) > 1.0 {
                NSCursor.setHiddenUntilMouseMoves(true)
            }
        }
        
        timerLock.unlock()
    }
 
    func processMessage(_ msg: Message) {

        func firstDrive() -> Bool {
            precondition(msg.data == 1 || msg.data == 2)
            return msg.data == 1;
        }
        // track("Message \(msg)")
    
        switch (msg.type) {
    
        case MSG_READY_TO_RUN:
    
            // Start emulator
            c64.run()
            metalScreen.blendIn()
            metalScreen.drawC64texture = true
    
            // Open attachment if present
            mydocument.openAttachmentWithDocument()
    
        case MSG_RUN:
            
            disableUserEditing()
            document?.updateChangeCount(.changeDone)
            refresh()
    
        case MSG_HALT:
            
            enableUserEditing()
            refresh()
    
        case MSG_BASIC_ROM_LOADED,
             MSG_CHAR_ROM_LOADED,
             MSG_KERNAL_ROM_LOADED,
             MSG_VC1541_ROM_LOADED:
            break
            
        case MSG_ROM_MISSING:

            track("MSG_ROM_MISSING")
            let nibName = NSNib.Name(rawValue: "RomDialog")
            let dialogController = RomDialogController.init(windowNibName: nibName)
            dialogController.showSheet(withParent: self)
            
        case MSG_SNAPSHOT_TAKEN:
            break
    
        case MSG_CPU_OK,
             MSG_CPU_SOFT_BREAKPOINT_REACHED:
            break
            
        case MSG_CPU_HARD_BREAKPOINT_REACHED,
             MSG_CPU_ILLEGAL_INSTRUCTION:
            self.debugOpenAction(self)
            refresh()
            
        case MSG_WARP_ON,
             MSG_WARP_OFF,
             MSG_ALWAYS_WARP_ON,
             MSG_ALWAYS_WARP_OFF:

            if c64.alwaysWarp() {
                warpIcon.image = NSImage.init(named: NSImage.Name(rawValue: "pin_red"))
            } else if (c64.warp()) {
                warpIcon.image = NSImage.init(named: NSImage.Name(rawValue: "clock_red"))
            } else {
                warpIcon.image = NSImage.init(named: NSImage.Name(rawValue: "clock_green"))
            }
    
        case MSG_PAL,
             MSG_NTSC:

            metalScreen.updateScreenGeometry()
    
        case MSG_KEYMATRIX,
             MSG_CHARSET:
            
            virtualKeyboard?.refresh()
            virtualKeyboardSheet?.refresh()

        case MSG_VC1541_ATTACHED:
            
            let image = NSImage.init(named: NSImage.Name(rawValue:"LEDgreen"))
            
            if firstDrive() {
                greenLED1.image = image
            } else {
                greenLED2.image = image
            }

        case MSG_VC1541_DETACHED:
            
            let image = NSImage.init(named: NSImage.Name(rawValue:"LEDgray"))
            
            if firstDrive() {
                greenLED1.image = image
            } else {
                greenLED2.image = image
            }

        case MSG_VC1541_ATTACHED_SOUND:
            
            // Not sure about the copyright of the following sound:
            // playSound:@"1541_power_on_0" volume:0.2];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            playSound(name: "drive_click", volume: 1.0)
        
        case MSG_VC1541_DETACHED_SOUND:
            
            // Not sure about the copyright of the following sound:
            // playSound:@"1541_track_change_0" volume:0.6];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            playSound(name: "drive_click", volume: 1.0)
    
        case MSG_VC1541_DISK_SOUND:
            
            // playSound:@"1541_door_closed_2" volume:0.2];
            playSound(name: "drive_snatch_uae", volume: 0.1)
            
        case MSG_VC1541_NO_DISK_SOUND:
            
            // playSound:@"1541_door_open_1" volume:0.2];
            playSound(name: "drive_snatch_uae", volume: 0.1)
            
        case MSG_VC1541_HEAD_UP_SOUND:
            
            // Not sure about the copyright of the following sound:
            // playSound:@"1541_track_change_0" volume:0.6];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            playSound(name: "drive_click", volume: 1.0)
            
        case MSG_VC1541_HEAD_DOWN_SOUND:
            
            // Not sure about the copyright of the following sound:
            // playSound:@"1541_track_change_2" volume:1.0];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            playSound(name: "drive_click", volume: 1.0)
            
        case MSG_VC1541_DISK:
            
            if firstDrive() {
                diskIcon1.isHidden = false
            } else {
                diskIcon2.isHidden = false
            }
  
        case MSG_VC1541_NO_DISK:
            
            if firstDrive() {
                diskIcon1.isHidden = true
            } else {
                diskIcon2.isHidden = true
            }
            
        case MSG_DISK_SAVED:
            
            let image = NSImage.init(named: NSImage.Name(rawValue: "disk_saved"))
            if firstDrive() {
                diskIcon1.image = image
            } else {
                diskIcon2.image = image
            }
            
        case MSG_DISK_UNSAVED:
            
            track("Disk is unsaved")
            let image = NSImage.init(named: NSImage.Name(rawValue: "disk_unsaved"))
            if firstDrive() {
                diskIcon1.image = image
            } else {
                diskIcon2.image = image
            }
            
        case MSG_VC1541_RED_LED_ON:
            
            let image = NSImage.init(named: NSImage.Name(rawValue: "LEDred"))
            if firstDrive() {
                redLED1.image = image
                redLED1.setNeedsDisplay()
            } else {
                redLED2.image = image
                redLED2.setNeedsDisplay()
            }
            
        case MSG_VC1541_RED_LED_OFF:
            
            let image = NSImage.init(named: NSImage.Name(rawValue: "LEDgray"))
            if firstDrive() {
                redLED1.image = image
                redLED1.setNeedsDisplay()
            } else {
                redLED2.image = image
                redLED2.setNeedsDisplay()
            }
    
        case MSG_IEC_BUS_BUSY:
            if c64.drive1.isRotating() {
                progress1.startAnimation(self)
            }
            if c64.drive2.isRotating() {
                progress2.startAnimation(self)
            }
    
        case MSG_IEC_BUS_IDLE:
            progress1.stopAnimation(self)
            progress2.stopAnimation(self)
            
        case MSG_VC1541_MOTOR_ON,
             MSG_VC1541_MOTOR_OFF,
             MSG_VC1541_HEAD_UP,
             MSG_VC1541_HEAD_DOWN:
            break
    
        case MSG_VC1530_TAPE:
            
            tapeIcon.isHidden = false

        case MSG_VC1530_NO_TAPE:
            
            tapeIcon.isHidden = true

        //case MSG_VC1530_PLAY:
        //    break
    
        case MSG_VC1530_PROGRESS:
            break
    
        case MSG_CARTRIDGE:
            
            cartridgeIcon.isHidden = false
    
        case MSG_NO_CARTRIDGE:
            
            cartridgeIcon.isHidden = true
            
        default:
            track("Unknown message: \(msg)")
            assert(false)
        }
    }

    //
    // Keyboard events
    //

    // Keyboard events are handled by the emulator window.
    // If they are handled here, some keys such as 'TAB' don't trigger an event.
 
    
    //
    //  Game pad events
    //
    
    /// GamePadManager delegation method
    /// - Returns: true, iff a joystick event has been triggered on port A or B
    @discardableResult
    func joystickEvent(slot: Int, events: [JoystickEvent]) -> Bool {
        
        if (slot == gamepadSlot1) {
            for event in events { c64.port1.trigger(event) }
            return true
        }

        if (slot == gamepadSlot2) {
            for event in events { c64.port2.trigger(event) }
            return true
        }
        
        return false
    }    

    
    //
    // Action methods (main screen)
    //
    
    @IBAction func alwaysWarpAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.alwaysWarpAction(sender)
        }
    
        c64.setAlwaysWarp(!c64.alwaysWarp())
        refresh()
    }
    
    
    //
    // Mounting media files
    //
    
    /// DEPRECATED
    @discardableResult
    func mount(_ item: ContainerProxy?) -> Bool {

        guard let type = item?.type() else { return false }
            
        // We need to take some special care for items that mount as a disk.
        // In that case, the light barrier has to be broken several times.
        
        switch (type) {
            
        case T64_CONTAINER, D64_CONTAINER,
             PRG_CONTAINER, P00_CONTAINER:
            // TODO: Use insertDisk for these attachments in future
            changeDisk(item, drive: 1)
            return true
            
        default:
            break
        }
        
        // Finally, let's mount that thing
        return c64.mount(item)
    }
    
    // Emulates changing a disk including the necessary light barrier breaks
    // If disk is nil, only the ejection is emulated.
    func changeDisk(_ disk: ContainerProxy?, drive nr: Int) {
        
        let drive = c64.drive(nr)!

        DispatchQueue.global().async {
            
            // For a better user experience, we switch on automatically
            // when a disk is inserted.
            if disk != nil {
                self.c64.drive(nr).powerOn()
            }
            
            // Remove old disk if present
            if drive.hasDisk() {
                drive.prepareToEject()
                usleep(300000)
                drive.ejectDisk()
            }
            
            // Insert new disk if provided
            if disk != nil {
                drive.prepareToInsert()
                usleep(300000)
                drive.insertDisk(disk as! ArchiveProxy)
            }
        }
    }

    
    //
    // Misc
    //

    func playSound(name: String, volume: Float) {
        
        if let s = NSSound.init(named: NSSound.Name(rawValue: name)) {
            s.volume = volume
            s.play()
        } else {
            track("ERROR: Cannot create NSSound object.")
        }
    }
}
