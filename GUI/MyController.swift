// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
    
    /* Proxy object. The proxy implements a bridge between the emulator written
     * in C++ and the GUI written in Swift. Because Swift cannot interact with
     * C++ directly, the proxy is written in Objective-C.
     */
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
        
    // Indicates if mouse is currently hidden
    var hideMouse = false
        
    // Indicates if a status bar is shown
    var statusBar = true
    
    // Small disk icon to be shown in NSMenuItems
    static let iconSize = CGSize(width: 16, height: 16)
    var smallDisk = NSImage(named: "diskTemplate")!.resize(size: iconSize)
    var smallTape = NSImage(named: "tapeTemplate")!.resize(size: iconSize)
    var smallCart = NSImage(named: "crtTemplate")!.resize(size: iconSize)
        
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
        case .auto: warp = c64.iec.transferring
        case .off: warp = false
        case .on: warp = true
        }
        
        c64.warp = warp
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
    @IBOutlet weak var tapeCounter: NSTextField!
    @IBOutlet weak var tapeProgress: NSProgressIndicator!
    @IBOutlet weak var crtIcon: NSButton!
    @IBOutlet weak var cpuInfo: NSTextField!
    @IBOutlet weak var mhzInfo: NSTextField!
    @IBOutlet weak var cpuIndicator: NSLevelIndicator!
    @IBOutlet weak var mhzIndicator: NSLevelIndicator!
    @IBOutlet weak var warpIcon: NSButton!
    
    // Toolbar
    @IBOutlet weak var toolbar: MyToolbar!
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
        config = Configuration(with: self)
        macAudio = MacAudio(with: self)
    }

    override open func windowDidLoad() {
 
        track()
        
        // Reset mouse coordinates
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
        
        // Setup window
        configureWindow()
        
        // Load user defaults
        loadUserDefaults()
        
        // Enable message processing
        addListener()
                
        // Process attachment (if any)
        mydocument.mountAttachment()

        // Check if the C64 is ready to power on
        if c64.isReady() {
            
            // Switch on and launch the emulator thread
            try? c64.run()

        } else {
            
            // Open the Rom dialog
            openConfigurator(tab: "Roms")
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
        
        c64.setListener(myself) { (ptr, type, data) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                let mType = MsgType(rawValue: type)
                myself.processMessage(Message(type: mType!, data: data))
            }
        }
    }
    
    func createTimer() {
    
        // Create speed monitor
        speedometer = Speedometer()
        
        // Create timer and speedometer
        /*
        assert(timer == nil)
        timer = Timer.scheduledTimer(timeInterval: 1.0/12, // 12 times a second
                                     target: self,
                                     selector: #selector(timerFunc),
                                     userInfo: nil,
                                     repeats: true)
        */
    }

    //
    // Timer and message processing
    //
    
    @objc func timerFunc() {

        // timerLock.lock()
 
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
            if c64.datasette.motor && c64.datasette.playKey {
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
        
        // timerLock.unlock()
    }
    
    func updateSpeedometer() {
        
        speedometer.updateWith(cycle: c64.cpu.cycle(), frame: renderer.frames)
        
        // let fps = speedometer.fps
        let cpu = c64.cpuLoad
        let mhz = speedometer.mhz

        cpuInfo.stringValue = String(format: "%d%% CPU", cpu)
        mhzInfo.stringValue = String(format: "%.2f MHz", mhz)
        cpuIndicator.integerValue = cpu
        mhzIndicator.doubleValue = 10 * mhz
    }
    
    @objc func snapshotTimerFunc() {
        
        if pref.autoSnapshots { c64.requestAutoSnapshot() }
    }
    
    func processMessage(_ msg: Message) {

        var driveNr: Int { return msg.data & 0xFF }
        var driveId: DriveID { return DriveID(rawValue: driveNr)! }
        var halftrack: Int { return (msg.data >> 8) & 0xFF; }
        var vol: Int { return (msg.data >> 16) & 0xFF; }
        var pan: Int { return (msg.data >> 24) & 0xFF; }

        // Only proceed if the proxy object is still alive
        if c64 == nil { return }
                        
        switch msg.type {
    
        case .REGISTER:
            track("Registered to message queue")
            
        case .UNREGISTER:
            track("Unregistered from message queue")

        case .CONFIG:
            inspector?.fullRefresh()
            refreshStatusBar()
            
        case .POWER_ON:
            renderer.canvas.open(delay: 2)
            virtualKeyboard = nil
            toolbar.updateToolbar()
            inspector?.fullRefresh()

        case .POWER_OFF:
            toolbar.updateToolbar()
            inspector?.fullRefresh()
            
        case .RUN:
            needsSaving = true
            toolbar.updateToolbar()
            inspector?.fullRefresh()
            inspector?.clearWatchPoint()
            refreshStatusBar()
    
        case .PAUSE:
            toolbar.updateToolbar()
            inspector?.fullRefresh()
            refreshStatusBar()

        case .STEP:
            needsSaving = true
            inspector?.clearWatchPoint()
            inspector?.fullRefresh()
            inspector?.scrollToPC()

        case .RESET:
            mydocument.deleteBootDiskID()
            mydocument.setBootDiskID(mydocument.attachment?.fnv ?? 0)
            updateWarp()
            inspector?.fullRefresh()
            
        case .HALT:
            shutDown()

        case .MUTE_ON:
            muted = true
            refreshStatusBar()
            
        case .MUTE_OFF:
            muted = false
            refreshStatusBar()

        case .WARP_ON,
             .WARP_OFF:
            refreshStatusBar()
            
        case .SCRIPT_DONE,
             .SCRIPT_PAUSE,
             .SCRIPT_ABORT:
            renderer.console.isDirty = true

        case .SCRIPT_WAKEUP:
            track()
            c64.continueScript()
            renderer.console.isDirty = true

        case .BASIC_ROM_LOADED,
             .CHAR_ROM_LOADED,
             .KERNAL_ROM_LOADED,
             .DRIVE_ROM_LOADED:
            break
            
        case .ROM_MISSING:
            break
                
        case .CPU_OK:
            break
            
        case .BREAKPOINT_REACHED:
            inspector?.fullRefresh()
            inspector?.scrollToPC()

        case .WATCHPOINT_REACHED:
            inspector?.fullRefresh()
            inspector?.scrollToPC()
            inspector?.signalWatchPoint()

        case .CPU_JAMMED:
            refreshStatusBar()
            
        case .PAL,
             .NTSC:
            renderer.canvas.updateTextureRect()
    
        case .DRIVE_STEP:
            macAudio.playStepSound(volume: vol, pan: pan)
            refreshStatusBarTracks(drive: driveId)
                        
        case .DISK_INSERT:
            macAudio.playInsertSound(volume: vol, pan: pan)
            mydocument.setBootDiskID(mydocument.attachment?.fnv ?? 0)
            refreshStatusBarDiskIcons(drive: driveId)
            inspector?.fullRefresh()

        case .DISK_EJECT:
            track()
            macAudio.playEjectSound(volume: vol, pan: pan)
            refreshStatusBarDiskIcons(drive: driveId)
            inspector?.fullRefresh()

        case .FILE_FLASHED:
            mydocument.setBootDiskID(mydocument.attachment?.fnv ?? 0)
            
        case .DISK_PROTECT,
             .DISK_SAVED,
             .DISK_UNSAVED,
             .DRIVE_LED_ON,
             .DRIVE_LED_OFF:
            refreshStatusBar()
    
        case .IEC_BUS_BUSY,
             .IEC_BUS_IDLE:
            updateWarp()
            refreshStatusBarDriveActivity()

        case .DRIVE_MOTOR_ON,
             .DRIVE_MOTOR_OFF:
            refreshStatusBarDriveActivity()
            
        case .DRIVE_CONNECT,
             .DRIVE_DISCONNECT,
             .DRIVE_POWER_OFF:
            hideOrShowDriveMenus()
            refreshStatusBar()

        case .DRIVE_POWER_ON:
            macAudio.playPowerSound(volume: vol, pan: pan)
            hideOrShowDriveMenus()
            refreshStatusBar()
            
        case .DRIVE_POWER_SAVE_ON,
             .DRIVE_POWER_SAVE_OFF:
            break
            
        case .VC1530_TAPE:
            if msg.data == 1 {
                mydocument.setBootDiskID(mydocument.attachment?.fnv ?? 0)
            }
            refreshStatusBar()

        case .VC1530_PLAY:
            refreshStatusBar()

        case .VC1530_MOTOR:
            refreshStatusBar()

        case .VC1530_COUNTER:
            refreshStatusBar()

        case .CRT_UNSUPPORTED:
            VC64Error.unsupportedCrtAlert(type: msg.data)
            
        case .CRT_ATTACHED:
            mydocument.setBootDiskID(mydocument.attachment?.fnv ?? 0)
            refreshStatusBar()

        case .CRT_DETACHED:
            refreshStatusBar()
            
        case .CART_SWITCH:
            break
            
        case .KB_AUTO_RELEASE:
            if virtualKeyboard?.window?.isVisible == true {
                virtualKeyboard!.refresh()
            }
            
        case .SHAKING:
            track()
            metal.lastShake = DispatchTime(uptimeNanoseconds: 0)
            if pref.releaseMouseByShaking {
                metal.releaseMouse()
            }

        case .SNAPSHOT_TOO_OLD:
            track("Unable to restore snapshot (too old)")
            // VC64Error(.SNP_TOO_OLD).warning("Unable to restore snapshot")
                        
        case .SNAPSHOT_TOO_NEW:
            track("Unable to restore snapshot (too new)")
            // VC64Error(.SNP_TOO_NEW).warning("Unable to restore snapshot")

        case .AUTO_SNAPSHOT_TAKEN:
            mydocument.snapshots.append(c64.latestAutoSnapshot)

        case .USER_SNAPSHOT_TAKEN:
            mydocument.snapshots.append(c64.latestUserSnapshot)
            renderer.flash()
            
        case .SNAPSHOT_RESTORED:
            renderer.rotateRight()
            renderer.canvas.updateTextureRect()
            refreshStatusBar()
            hideOrShowDriveMenus()

        case .RECORDING_STARTED:
            window?.backgroundColor = .recordingColor
            refreshStatusBar()
                
        case .RECORDING_STOPPED:
            window?.backgroundColor = .windowBackgroundColor
            refreshStatusBar()

        case .CLOSE_CONSOLE:
            renderer.console.close(delay: 0.25)
            
        case .DMA_DEBUG_ON:
            renderer.zoomTextureOut()

        case .DMA_DEBUG_OFF:
            renderer.zoomTextureIn()

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
}
