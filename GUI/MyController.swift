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
    var inspector: Inspector?
    
    // Monitor panel of this emulator instance
    var monitor: Monitor?
    
    // Configuration panel of this emulator instance
    var configurator: ConfigurationController?

    // Snapshot and screenshot browsers
    var snapshotBrowser: SnapshotDialog?
    var screenshotBrowser: ScreenshotDialog?

    // The current emulator configuration
    var config: Configuration!

    // Audio Engine
    var macAudio: MacAudio!
    
    // Game pad manager
    var gamePadManager: GamePadManager!
    var gamePad1: GamePad? { return gamePadManager.gamePads[config.gameDevice1] }
    var gamePad2: GamePad? { return gamePadManager.gamePads[config.gameDevice2] }
    
    // Keyboard controller
    var keyboard: KeyboardController!

    // Virtual keyboard
    var virtualKeyboard: VirtualKeyboardController?
    
    // Loop timer for scheduling periodic updates
    var timer: Timer?
    
    // // Timer lock
    var timerLock = NSLock()
    
    // Snapshot timers
    var snapshotTimer: Timer?
    
    // Speedometer to measure clock frequence and frames per second
    var speedometer: Speedometer!
    
    // Used inside the timer function to fine tune timed events
    var animationCounter = 0
    
    // Remembers if audio is muted (master volume of both channels is 0)
    var muted = false

    // Current keyboard modifier flags
    // These flags tell us if one of the special keysare currently pressed.
    // The flags are utilized, e.g., to alter behaviour when a key on the
    // TouchBar is pressed.
    var modifierFlags: NSEvent.ModifierFlags = .init(rawValue: 0)
    
    // Current mouse coordinate
    var mouseXY = NSPoint(x: 0, y: 0)
    
    // Indicates if mouse is currently hidden
    var hideMouse = false
        
    // Indicates if a status bar is shown
    var statusBar = true
    
    // Small disk icon to be shown in NSMenuItems
    static let iconSize = CGSize(width: 16, height: 16)
    var smallDisk = NSImage.init(named: "diskTemplate")!.resize(size: iconSize)
    var smallTape = NSImage.init(named: "tapeTemplate")!.resize(size: iconSize)
    var smallCart = NSImage.init(named: "crtTemplate")!.resize(size: iconSize)
    
    // Remembers the running state for the pauseInBackground feature
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
    
    func updateWarp() {
        
        var warp: Bool
        
        switch pref.warpMode {
        case .auto: warp = c64.iec.busy()
        case .off: warp = false
        case .on: warp = true
        }
        
        if warp != c64.warp() {
            warp ? c64.warpOn() : c64.warpOff()
        }
    }
    
    //
    // Outlets
    //
    
    // Main screen
    @IBOutlet weak var metal: MetalView!
    
    var renderer: Renderer!
        
    // Status bar
    @IBOutlet weak var greenLED8: NSButton!
    @IBOutlet weak var greenLED9: NSButton!
    @IBOutlet weak var redLED8: NSButton!
    @IBOutlet weak var redLED9: NSButton!
    @IBOutlet weak var diskIcon8: NSButton!
    @IBOutlet weak var diskIcon9: NSButton!
    @IBOutlet weak var trackNumber8: NSTextField!
    @IBOutlet weak var trackNumber9: NSTextField!
    @IBOutlet weak var spinning8: NSProgressIndicator!
    @IBOutlet weak var spinning9: NSProgressIndicator!
    @IBOutlet weak var haltIcon: NSButton!
    @IBOutlet weak var debugIcon: NSButton!
    @IBOutlet weak var muteIcon: NSButton!
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
    override open var undoManager: UndoManager? { return metal.undoManager }
     
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
    // Initializing
    //

    override open func awakeFromNib() {

        track()
                
        mydocument = document as? MyDocument
        config = Configuration.init(with: self)
        macAudio = MacAudio.init(with: self)
    }

    override open func windowDidLoad() {
 
        track()
        
        // Reset mouse coordinates
        mouseXY = NSPoint.zero
        hideMouse = false
        
        // Create keyboard controller
        keyboard = KeyboardController(parent: self)
        assert(keyboard != nil, "Failed to create keyboard controller")

        // Create game pad manager
        gamePadManager = GamePadManager(parent: self)
        assert(gamePadManager != nil, "Failed to create game pad manager")

        // Setup renderer
        renderer = Renderer(view: metal,
                            device: MTLCreateSystemDefaultDevice()!,
                            controller: self)
        
        // Prepare to run
        configureWindow()
        loadUserDefaults()
        addListener()

        // Evaluate command line arguments
        if CommandLine.arguments.contains("-debugcart") {
            c64.configure(.DEBUGCART, enable: true)
        }
        
        // Check if the C64 is ready to power on
        if c64.isReady() {
            
            // Switch on and launch the emulator thread
            c64.powerOn()

            // Process attachment (if any)
            mydocument.mountAttachment()

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
        
        // Update status bar
        refreshStatusBar()
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
    
    func addListener() {
        
        track()
        
        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        c64.addListener(myself) { (ptr, type, data) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                let mType = MessageType(rawValue: type)
                myself.processMessage(Message(type: mType!, data: data))
            }
        }
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
    }

    //
    // Timer and message processing
    //
    
    @objc func timerFunc() {

        timerLock.lock()
 
        animationCounter += 1

        // Animate the inspector
        if inspector?.window?.isVisible == true { inspector!.continuousRefresh() }
        
        // Update the cartridge LED
        if c64.expansionport.hasLed() {
            let led = c64.expansionport.led() ? 1 : 0
            if crtIcon.tag != led {
                crtIcon.tag = led
                crtIcon.image = NSImage(named: led == 1 ? "crtLedOnTemplate" : "crtTemplate")
                crtIcon.needsDisplay = true
            }
        }
        
        // Do less frequently...
        if (animationCounter % 2) == 0 {
            
            /* Update the tape progress icon. Note: The tape progress icon is
             * not switched on or off by push notification (message), because
             * some games continously switch the datasette motor on and off.
             */
            if c64.datasette.motor() && c64.datasette.playKey() {
                tapeProgress.startAnimation(self)
            } else {
                tapeProgress.stopAnimation(self)
            }
        }
        
        // Do even less frequently...
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
        
        if pref.autoSnapshots { c64.requestAutoSnapshot() }
    }
    
    func processMessage(_ msg: Message) {

        // track("msg: \(msg)")
        
        // TODO: REPLACE BY COMPUTED VARIABLE
        func drive8() -> Bool {
            precondition(msg.data == 8 || msg.data == 9)
            return msg.data == 8
        }
        var driveID: DriveID { return msg.data == 8 ? .DRIVE8 : .DRIVE9 }

        switch msg.type {
    
        case .MSG_CONFIG:

            inspector?.fullRefresh()
            refreshStatusBar()
            
        case .MSG_POWER_ON:
            
            virtualKeyboard = nil
            renderer.blendIn()
            // renderer.zoomIn()
            toolbar.validateVisibleItems()
            inspector?.fullRefresh()

        case .MSG_POWER_OFF:
            
            renderer.blendOut()
            // renderer.zoomOut(steps: 20)
            toolbar.validateVisibleItems()
            inspector?.fullRefresh()
            
        case .MSG_RUN:
            
            needsSaving = true
            toolbar.validateVisibleItems()
            inspector?.fullRefresh()
            refreshStatusBar()
    
        case .MSG_PAUSE:
            
            toolbar.validateVisibleItems()
            inspector?.fullRefresh()
            refreshStatusBar()

        case .MSG_RESET:

            mydocument.deleteBootDiskID()
            mydocument.setBootDiskID(mydocument.attachment?.fnv() ?? 0)
            inspector?.fullRefresh()

        case .MSG_MUTE_ON:
            
            muted = true
            refreshStatusBar()
            
        case .MSG_MUTE_OFF:
            
            muted = false
            refreshStatusBar()

        case .MSG_WARP_ON,
             .MSG_WARP_OFF:
            
            refreshStatusBar()
            
        case .MSG_BASIC_ROM_LOADED,
             .MSG_CHAR_ROM_LOADED,
             .MSG_KERNAL_ROM_LOADED,
             .MSG_DRIVE_ROM_LOADED:
            
            break
            
        case .MSG_ROM_MISSING:
            
            // openConfigurator()
            break
                
        case .MSG_CPU_OK:
            
            break
            
        case .MSG_BREAKPOINT_REACHED,
             .MSG_WATCHPOINT_REACHED:
            
            track("MSG_BREAKPOINT_REACHED MSG_WATCHPOINT_REACHED")
            inspector?.fullRefresh()
            inspector?.scrollToPC()
            
        case .MSG_CPU_JAMMED:
            
            refreshStatusBar()
            
        case .MSG_PAL,
             .MSG_NTSC:
            
            renderer.updateTextureRect()
    
        case .MSG_DRIVE_HEAD:
            
            if pref.driveSounds && pref.driveHeadSound {
                playSound(name: "1541_track_change_2", volume: 1.0)
            }
            refreshStatusBarTracks(drive: DriveID.init(rawValue: msg.data)!)
                        
        case .MSG_DISK_INSERTED:
            
            if pref.driveSounds && pref.driveInsertSound {
                playSound(name: "1541_door_closed_2", volume: 0.2)
            }
            mydocument.setBootDiskID(mydocument.attachment?.fnv() ?? 0)
            refreshStatusBarDiskIcons(drive: driveID)
            inspector?.fullRefresh()

        case .MSG_DISK_EJECTED:
            
            if pref.driveSounds && pref.driveEjectSound {
                playSound(name: "1541_door_open_1", volume: 0.15)
            }
            refreshStatusBarDiskIcons(drive: driveID)
            inspector?.fullRefresh()

        case .MSG_FILE_FLASHED:
            mydocument.setBootDiskID(mydocument.attachment?.fnv() ?? 0)
            
        case .MSG_DISK_PROTECT,
             .MSG_DISK_SAVED,
             .MSG_DISK_UNSAVED,
             .MSG_DRIVE_LED_ON,
             .MSG_DRIVE_LED_OFF:
            
            refreshStatusBar()
    
        case .MSG_IEC_BUS_BUSY,
             .MSG_IEC_BUS_IDLE:
            
            updateWarp()
            refreshStatusBarDriveActivity()

        case .MSG_DRIVE_MOTOR_ON,
             .MSG_DRIVE_MOTOR_OFF:

            refreshStatusBarDriveActivity()
            
        case .MSG_DRIVE_CONNECT,
             .MSG_DRIVE_DISCONNECT,
             .MSG_DRIVE_POWER_ON,
             .MSG_DRIVE_POWER_OFF:
            
            refreshStatusBar()
                        
        case .MSG_DRIVE_ACTIVE:
            
            if pref.driveSounds && pref.driveConnectSound {
                playSound(name: "1541_power_on_0", volume: 0.15)
            }
            myAppDelegate.hideOrShowDriveMenus(proxy: c64)
            refreshStatusBar()
            
        case .MSG_DRIVE_INACTIVE:
            
            if pref.driveSounds && pref.driveConnectSound {
                // playSound(name: "drive_click", volume: 1.0)
            }
            myAppDelegate.hideOrShowDriveMenus(proxy: c64)
            refreshStatusBar()

        case .MSG_VC1530_TAPE:
            
            mydocument.setBootDiskID(mydocument.attachment?.fnv() ?? 0)
            refreshStatusBar()

        case .MSG_VC1530_NO_TAPE,
             .MSG_VC1530_PROGRESS:
            
            refreshStatusBar()

        case .MSG_CARTRIDGE:

            mydocument.setBootDiskID(mydocument.attachment?.fnv() ?? 0)
            refreshStatusBar()

        case .MSG_NO_CARTRIDGE:
            
            refreshStatusBar()
            
        case .MSG_CART_SWITCH:
            
            refreshStatusBarCartridgeIcons()
            
        case .MSG_KB_AUTO_RELEASE:
            
            if virtualKeyboard?.window?.isVisible == true {
                virtualKeyboard!.refresh()
            }
            
        case .MSG_AUTO_SNAPSHOT_TAKEN:
            
            track("MSG_AUTO_SNAPSHOT_TAKEN")
            mydocument.snapshots.append(c64.latestAutoSnapshot)

        case .MSG_USER_SNAPSHOT_TAKEN:
            
            track("MSG_USER_SNAPSHOT_TAKEN")
            mydocument.snapshots.append(c64.latestUserSnapshot)
            renderer.blendIn(steps: 20)
            
        case .MSG_SNAPSHOT_RESTORED:
            
            track("MSG_SNAPSHOT_RESTORED")
            renderer.blendIn(steps: 20)
            renderer.updateTextureRect()
            refreshStatusBar()
            
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
