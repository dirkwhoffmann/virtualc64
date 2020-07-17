// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

enum WarpMode: Int {

    case auto
    case off
    case on
}

enum AutoMountAction: Int, Codable {
    
    case openBrowser = 0
    case flashFirstFile = 1
    case insertIntoDrive8 = 2
    case insertIntoDrive9 = 3
    case insertIntoDatasette = 4
    case attachToExpansionPort = 5
}

protocol MessageReceiver {
    func processMessage(_ msg: Message)
}

class MyController: NSWindowController, MessageReceiver {

    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    var pref: Preferences { return myAppDelegate.pref }

    // Reference to the connected document
    var mydocument: MyDocument!
    
    // Amiga proxy
    // Implements a bridge between the emulator written in C++ and the
    // GUI written in Swift. Because Swift cannot interact with C++ directly,
    // the proxy is written in Objective-C.
    var c64: C64Proxy!
    
    // Inspector panel of this emulator instance
    // var inspector: Inspector?
    
    // Configuration panel of this emulator instance
    var configurator: ConfigurationController?
    
    // The current emulator configuration
    var config: Configuration!

    // Audio Engine
    var macAudio: MacAudio!
    
    // Game pad manager
    var gamePadManager: GamePadManager!
    /*
     var gamePad1: GamePad? { return gamePadManager.gamePads[config.gameDevice1] }
     var gamePad2: GamePad? { return gamePadManager.gamePads[config.gameDevice2] }
     */
    
    // Keyboard controller
    var keyboard: KeyboardController!

    // Virtual keyboard
    var virtualKeyboard: VirtualKeyboardController?
    
    /// Preferences controller
    // var preferencesController: ConfigurationController?

    /// Loop timer
    /// The timer fires 60 times a second and executes all tasks that need to be
    //  done perdiodically (e.g., updating the speedometer and the debug panels)
    var timer: Timer?
    
    /// Lock to prevent reentrance into the timer function
    var timerLock = NSLock()
    
    // Snapshot timers
    var snapshotTimer: Timer?

    /// Lock for protecting the C64 proxy
    var proxyLock = NSLock()
    
    /// Speedometer to measure clock frequence and frames per second
    var speedometer: Speedometer!
    
    /// Used inside the timer function to fine tune timed events
    var animationCounter = 0
    
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
    var inputDevice1 = Defaults.inputDevice1
    
    /// Selected game pad slot for joystick in port B
    var inputDevice2 = Defaults.inputDevice2
    
    /*
    /// Screenshot resolution (0 = low, 1 = high)
    var screenshotSource = GeneralDefaults.std.screenshotSource
    
    /// Screenshot image format
    var screenshotTarget = GeneralDefaults.std.screenshotTarget
    var screenshotTargetIntValue: Int {
        get { return Int(screenshotTarget.rawValue) }
        set { screenshotTarget = NSBitmapImageRep.FileType(rawValue: UInt(newValue))! }
    }
    
    /// Media file default actions
    var autoMountAction: [String: AutoMountAction] = Defaults.autoMountAction

    /// Media file auto-type enable / disable
    var autoType: [String: Bool] = Defaults.autoType

    /// Media file auto-type text
    var autoTypeText: [String: String] = Defaults.autoTypeText
    
    /// Indicates if the user should be warned if an unsaved document is closed.
    var closeWithoutAsking = GeneralDefaults.std.closeWithoutAsking

    /// Indicates if the user should be warned if an unsaved disk is ejected.
    var ejectWithoutAsking = GeneralDefaults.std.driveEjectUnasked

    /// Indicates if the emulator should pause when it looses focus.
    var pauseInBackground =  GeneralDefaults.std.pauseInBackground
    */
 
    /// Remembers if the emulator was running or paused when it lost focus.
    /// Needed to implement the pauseInBackground feature.
    var pauseInBackgroundSavedState = false
    
    //
    // Timers
    //
    
    func startSnapshotTimer() {
        
        if pref.snapshotInterval > 0 {
            
            snapshotTimer?.invalidate()
            snapshotTimer =
                Timer.scheduledTimer(timeInterval: TimeInterval(pref.snapshotInterval),
                                     target: self,
                                     selector: #selector(snapshotTimerFunc),
                                     userInfo: nil,
                                     repeats: true)
        }
    }
    
    func stopSnapshotTimer() {
        
        snapshotTimer?.invalidate()
    }
    
    func updateWarp() { }
    /*
        
        var warp: Bool
        
        switch pref.warpMode {
        case .auto: warp = amiga.diskController.spinning()
        case .off: warp = false
        case .on: warp = true
        }
        
        if warp != amiga.warp() {
            warp ? amiga.warpOn() : amiga.warpOff()
            refreshStatusBar()
        }
    }
    */
    
    //
    // Outlets
    //
    
    // Main screen
    @IBOutlet weak var metalScreen: MetalView!
    var renderer: Renderer!
    
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
    @IBOutlet weak var tapeIcon: NSButton!
    @IBOutlet weak var tapeProgress: NSProgressIndicator!
    @IBOutlet weak var crtIcon: NSButton!
    @IBOutlet weak var crtSwitch: NSButton!
    @IBOutlet weak var crtButton1: NSButton!
    @IBOutlet weak var crtButton2: NSButton!
    @IBOutlet weak var clockSpeed: NSTextField!
    @IBOutlet weak var clockSpeedBar: NSLevelIndicator!
    @IBOutlet weak var warpIcon: NSButton!
    
    // Toolbar
    @IBOutlet weak var toolbar: NSToolbar!

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
    @IBOutlet weak var spriteXStepper: NSStepper!
    @IBOutlet weak var spriteY: NSTextField!
    @IBOutlet weak var spriteYStepper: NSStepper!
    @IBOutlet weak var spritePtr: NSTextField!
    @IBOutlet weak var spritePtrStepper: NSStepper!
    @IBOutlet weak var spriteIsMulticolor: NSPopUpButton!
    @IBOutlet weak var spriteColor: NSButton!
    @IBOutlet weak var spriteExtraColor1: NSButton!
    @IBOutlet weak var spriteExtraColor2: NSButton!
    @IBOutlet weak var spriteExpandX: NSButton!
    @IBOutlet weak var spriteExpandY: NSButton!
    @IBOutlet weak var spritePriority: NSPopUpButton!
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
    override open var undoManager: UndoManager? { return metalScreen.undoManager }
     
    // Indicates if the emulator needs saving
    var needsSaving: Bool {
        get {
            return document?.changeCount != 0
        }
        set {
            if newValue && !pref.closeWithoutAsking {
                document?.updateChangeCount(.changeDone)
            } else {
                document?.updateChangeCount(.changeCleared)
            }
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
        
        mydocument = document as? MyDocument
        config = Configuration.init(with: self)
        macAudio = MacAudio.init(withSID: c64.sid)
    }

    override open func windowDidLoad() {
 
        track()
        
        // Reset mouse coordinates
        mouseXY = NSPoint.zero
        hideMouse = false
        
        // Create keyboard controller
        keyboard = KeyboardController(controller: self)
        if keyboard == nil {
            track("Failed to create keyboard controller")
            return
        }

        // Create game pad manager
        gamePadManager = GamePadManager(controller: self)
        if gamePadManager == nil {
            track("Failed to create game pad manager")
            return
        }
                
        // Setup renderer
        renderer = Renderer(view: metalScreen,
                            device: MTLCreateSystemDefaultDevice()!,
                            controller: self)
        
        // Setup toolbar, window, and debugger
        configureToolbar()
        configureWindow()
        setupDebugger()

        // Load user defaults
        loadUserDefaults()
        
        // Enable message processing (register callback)
        addListener()
        
        // Process attachment (if any)
        mydocument.mountAttachment()

        // Check if the C64 is ready to power on
        if c64.isReady() {
            
            // Power on the C64
            c64.powerOn()
            
            // Launch the emulator thread
            c64.run()

        } else {
            
            // Open the Rom dialog
            openConfigurator(tab: "Roms")
            // renderer.zoomOut()
        }

        // Create speed monitor and get the timer tunning
        createTimer()
        
        // Update toolbar
        toolbar.validateVisibleItems()
    }
    
    func configureWindow() {
    
        // Add status bar
        window?.autorecalculatesContentBorderThickness(for: .minY)
        window?.setContentBorderThickness(32.0, for: .minY)
        statusBar = true
        
        // Adjust size and enable auto-save for window coordinates
        adjustWindowSize()
        window?.windowController?.shouldCascadeWindows = false // true ?!
        let name = NSWindow.FrameAutosaveName("dirkwhoffmann.de.virtualC64.window")
        window?.setFrameAutosaveName(name)
        
        // Enable fullscreen mode
        window?.collectionBehavior = .fullScreenPrimary
    }
    
    func configureToolbar() {
        
        // Get and resize images
        let cutout = NSRect.init(x: 2, y: 0, width: 28, height: 28)
        
        var none = NSImage(named: "oxygen_none")
        none = none?.resizeImage(width: 32, height: 32, cutout: cutout)
        var keyset = NSImage(named: "oxygen_keys")
        keyset = keyset?.resizeImage(width: 32, height: 32, cutout: cutout)
        var mouse = NSImage(named: "devMouseTemplate")
        mouse = mouse?.resizeImage(width: 32, height: 32, cutout: cutout)
        var gamepad = NSImage(named: "crystal_gamepad")
        gamepad = gamepad?.resizeImage(width: 32, height: 32, cutout: cutout)
        
        toolbar.validateVisibleItems()
    }
    
    func addListener() {
        
        track()
        
        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        c64.addListener(myself) { (ptr, type, data) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                let mType = MessageType(rawValue: UInt32(type))
                myself.processMessage(Message(type: mType, data: data))
            }
        }
        
        track("Listener is in place")
    }
    
    func createTimer() {
    
        // Create speed monitor
        speedometer = Speedometer()
        
        // Create timer and speedometer
        assert(timer == nil)
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

        timerLock.lock()
 
        animationCounter += 1

        // Do 12 times a second ...
        if (animationCounter % 1) == 0 {
            
            // Refresh debug panel if open
            if c64.isRunning() {
                let state = debugger.state
                if state == NSDrawerState.open || state == NSDrawerState.opening {
                    refresh()
                }
            }
            
            // Update cartridge LED
            if c64.expansionport.hasLed() {
                let led = c64.expansionport.led() ? 1 : 0
                if crtIcon.tag != led {
                    crtIcon.tag = led
                    crtIcon.image = NSImage(named: led == 1 ? "crtLedOnTemplate" : "crtTemplate")
                    crtIcon.needsDisplay = true
                }
            }
        }
        
        // Do 6 times a second ...
        if (animationCounter % 2) == 0 {
 
            // Update tape progress icon
            // Note: The tape progress icon is not switched on or off by push
            // notification (message), because some games continously switch the
            // datasette motor on and off.
            if c64.datasette.motor() && c64.datasette.playKey() {
                tapeProgress.startAnimation(self)
            } else {
                tapeProgress.stopAnimation(self)
            }
        }
        
        // Do lesser time...
        if (animationCounter % 4) == 0 {
            
            updateSpeedometer()
        
            // Let the cursor disappear in fullscreen mode
            if renderer.fullscreen &&
                CGEventSource.secondsSinceLastEventType(.combinedSessionState,
                                                        eventType: .mouseMoved) > 1.0 {
                NSCursor.setHiddenUntilMouseMoves(true)
            }
        }
        
        timerLock.unlock()
    }
    
    func updateSpeedometer() {
        
        speedometer.updateWith(cycle: c64.cpu.cycle(), frame: renderer.frames)
        let mhz = speedometer.mhz
        let fps = speedometer.fps
        clockSpeed.stringValue = String(format: "%.2f MHz %.0f fps", mhz, fps)
        clockSpeedBar.doubleValue = 10 * mhz
    }
    
    @objc func snapshotTimerFunc() {
        
        if pref.autoSnapshots { } //  { takeAutoSnapshot() }
    }
    
    func processMessage(_ msg: Message) {

        func firstDrive() -> Bool {
            precondition(msg.data == 1 || msg.data == 2)
            return msg.data == 1
        }

        proxyLock.lock()
        
        switch msg.type {
    
        case MSG_POWER_ON:
            track("MSG_POWER_ON")
            virtualKeyboard = nil
            renderer.blendIn()
            // renderer.zoomIn()
            toolbar.validateVisibleItems()

        case MSG_POWER_OFF:
            track("MSG_POWER_OFF")
            renderer.blendOut()
            // renderer.zoomOut(steps: 20)
            toolbar.validateVisibleItems()
            
        case MSG_RUN:
            track("MSG_RUN")
            needsSaving = true
            toolbar.validateVisibleItems()
            disableUserEditing()
            refresh()
    
        case MSG_PAUSE:
            track("MSG_PAUSE")
            toolbar.validateVisibleItems()
            enableUserEditing()
            refresh()
    
        case MSG_RESET:
            track("MSG_RESET")

        case MSG_BASIC_ROM_LOADED,
             MSG_CHAR_ROM_LOADED,
             MSG_KERNAL_ROM_LOADED,
             MSG_VC1541_ROM_LOADED:
            break
            
        case MSG_ROM_MISSING:
            // openConfigurator()
            break
            
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
                let name = NSImage.Name("hourglass3Template")
                warpIcon.image = NSImage.init(named: name)
            } else if c64.warp() {
                let name = NSImage.Name("hourglass2Template")
                warpIcon.image = NSImage.init(named: name)
            } else {
                let name = NSImage.Name("hourglass1Template")
                warpIcon.image = NSImage.init(named: name)
            }
    
        case MSG_PAL,
             MSG_NTSC:

            renderer.updateTextureRect()
    
        case MSG_VC1541_ATTACHED:
            
            if pref.driveSounds && pref.driveConnectSound {
                playSound(name: "drive_click", volume: 1.0)
            }
            
            let image = NSImage.init(named: "LEDgreen")
            
            if firstDrive() {
                greenLED1.image = image
            } else {
                greenLED2.image = image
            }

        case MSG_VC1541_DETACHED:
            
            if pref.driveSounds && pref.driveConnectSound {
                playSound(name: "drive_click", volume: 1.0)
            }
            
            let image = NSImage.init(named: "LEDgray")
            
            if firstDrive() {
                greenLED1.image = image
            } else {
                greenLED2.image = image
            }
                        
        case MSG_VC1541_HEAD_UP,
             MSG_VC1541_HEAD_DOWN:
            
            if pref.driveSounds && pref.driveHeadSound {
                playSound(name: "drive_click", volume: 1.0)
            }
                        
        case MSG_VC1541_DISK:
            
            if pref.driveSounds && pref.driveInsertSound {
                playSound(name: "drive_snatch_uae", volume: 0.1)
            }

            if firstDrive() {
                diskIcon1.isHidden = false
            } else {
                diskIcon2.isHidden = false
            }
              
        case MSG_VC1541_NO_DISK:
            
            if pref.driveSounds && pref.driveEjectSound {
                playSound(name: "drive_snatch_uae", volume: 0.1)
            }

            if firstDrive() {
                diskIcon1.isHidden = true
            } else {
                diskIcon2.isHidden = true
            }
                            
        case MSG_DISK_SAVED:
            
            let image = NSImage.init(named: "mediaDiskSavedTemplate")
            if firstDrive() {
                diskIcon1.image = image
            } else {
                diskIcon2.image = image
            }
            
        case MSG_DISK_UNSAVED:
            
            track("Disk is unsaved")
            let image = NSImage.init(named: "mediaDiskUnsavedTemplate")
            if firstDrive() {
                diskIcon1.image = image
            } else {
                diskIcon2.image = image
            }
            
        case MSG_VC1541_RED_LED_ON:
            
            let image = NSImage.init(named: "LEDred")
            if firstDrive() {
                redLED1.image = image
                redLED1.setNeedsDisplay()
            } else {
                redLED2.image = image
                redLED2.setNeedsDisplay()
            }
            
        case MSG_VC1541_RED_LED_OFF:
            
            let image = NSImage.init(named: "LEDgray")
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
             MSG_VC1541_MOTOR_OFF:
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
            
            crtIcon.isHidden = false
            crtSwitch.isHidden = !c64.expansionport.hasSwitch()
            crtButton1.isHidden = c64.expansionport.numButtons() < 1
            crtButton2.isHidden = c64.expansionport.numButtons() < 2
            crtButton1.toolTip = c64.expansionport.getButtonTitle(1)
            crtButton2.toolTip = c64.expansionport.getButtonTitle(2)

        case MSG_NO_CARTRIDGE:
            
            crtIcon.isHidden = true
            crtSwitch.isHidden = true
            crtButton1.isHidden = true
            crtButton2.isHidden = true
            
        case MSG_CART_SWITCH:
            
            let pos = c64.expansionport.switchPosition()
            crtSwitch.image = NSImage(named:
                (pos < 0) ? "crtSwitchLeftTemplate"
                    : (pos > 0) ? "crtSwitchRightTemplate"
                    : "crtSwitchNeutralTemplate")
            crtSwitch.toolTip = c64.expansionport.switchDescription(pos)
            
        default:
            
            track("Unknown message: \(msg)")
            assert(false)
        }
        
        proxyLock.unlock()
    }

    //
    // Dialogs
    //
    
    /*
    func openPreferences() {
        
        if preferencesController == nil {
            let nibName = NSNib.Name("Preferences")
            preferencesController = ConfigurationController.init(windowNibName: nibName)
        }
        preferencesController!.showSheet()
    }
    */
    
    //
    // Loading Roms
    //
    
    /*
    @discardableResult
    func loadRom(_ url: URL?) -> Bool {
        
        if url == nil {
            return false
        }
        
        if c64.loadBasicRom(url!) {
            config.basicRomURL = url!
            return true
        }
        if c64.loadCharRom(url!) {
            config.charRomURL = url!
            return true
        }
        if c64.loadKernalRom(url!) {
            config.kernalRomURL = url!
            return true
        }
        if c64.loadVC1541Rom(url!) {
            config.vc1541RomURL = url!
            return true
        }
        
        track("ROM file \(url!) not found")
        return false
    }
    */
    
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
        
        if slot == inputDevice1 {
            for event in events { c64.port1.trigger(event) }
            return true
        }

        if slot == inputDevice2 {
            for event in events { c64.port2.trigger(event) }
            return true
        }
        
        return false
    }    

    //
    // Action methods (main screen)
    //
    
    @IBAction func alwaysWarpAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.alwaysWarpAction(sender)
        }
    
        c64.setAlwaysWarp(!c64.alwaysWarp())
        refresh()
    }
    
    //
    // Mounting media files
    //
    
    @discardableResult
    func mount(_ item: AnyC64FileProxy?) -> Bool {

        guard let type = item?.type() else { return false }
        
        switch type {
            
        case CRT_FILE:
            c64.expansionport.attachCartridgeAndReset(item as? CRTFileProxy)
            return true
            
        case TAP_FILE:
            return c64.datasette.insertTape(item as? TAPFileProxy)
            
        case T64_FILE, D64_FILE,
             PRG_FILE, P00_FILE,
             G64_FILE:
            // We need to take some special care for items that mount as a disk.
            // In that case, the light barrier has to be broken several times.
            // TODO: Use insertDisk for these attachments in future
            changeDisk(item, drive: 1)
            return true
                        
        default:
            track("Unknown attachment type \(type).")
            fatalError()
        }
    }
    
    // Emulates changing a disk including the necessary light barrier breaks
    // If disk is nil, only the ejection is emulated.
    func changeDisk(_ disk: AnyC64FileProxy?, drive nr: Int) {
        
        let drive = c64.drive(nr)!

        DispatchQueue.global().async {
            
            // For a better user experience, we switch on automatically
            // when a disk is inserted.
            if disk != nil {
                self.c64.drive(nr).connect()
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
                drive.insertDisk(disk as? AnyArchiveProxy)
            }
        }
    }
    
    //
    // Misc
    //
    
    func playSound(name: String, volume: Float) {
        
        if let s = NSSound.init(named: name) {
            s.volume = volume
            s.play()
        } else {
            track("ERROR: Cannot create NSSound object.")
        }
    }
}
