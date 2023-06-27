// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

protocol MessageReceiver {
    func processMessage(_ msg: Message)
}

class MyController: NSWindowController, MessageReceiver {

    var pref: Preferences { return myAppDelegate.pref }

    // Reference to the connected document
    var mydocument: MyDocument!
    
    // C64 proxy (bridge between the Swift frontend and the C++ backend)
    var c64: C64Proxy!

    // Media manager (handles the import and export of media files)
    var mm: MediaManager { return mydocument.mm }

    // Inspector panel of this emulator instance
    var inspector: Inspector?
    
    // Monitor panel of this emulator instance
    var monitor: Monitor?
    
    // Configuration panel of this emulator instance
    var configurator: ConfigurationController?

    // Snapshot and screenshot browsers
    var snapshotBrowser: SnapshotViewer?
    var screenshotBrowser: ScreenshotViewer?

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

    // Remembers if an illegal instruction has jammed the CPU
    var jammed = false

    // Remembers if audio is muted (master volume of both channels is 0)
    var muted = false

    // Current keyboard modifier flags
    // These flags tell us if one of the special keysare currently pressed.
    // The flags are utilized, e.g., to alter behaviour when a key on the
    // TouchBar is pressed.
    var modifierFlags: NSEvent.ModifierFlags = .init(rawValue: 0)

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
    @IBOutlet weak var trackIcon: NSButton!
    @IBOutlet weak var muteIcon: NSButton!
    @IBOutlet weak var tapeIcon: NSButton!
    @IBOutlet weak var tapeCounter: NSTextField!
    @IBOutlet weak var tapeProgress: NSProgressIndicator!
    @IBOutlet weak var crtIcon: NSButton!
    
    @IBOutlet weak var warpIcon: NSButton!
    @IBOutlet weak var activityType: NSPopUpButton!
    @IBOutlet weak var activityInfo: NSTextField!
    @IBOutlet weak var activityBar: NSLevelIndicator!

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

        mydocument = document as? MyDocument
        config = Configuration(with: self)
        macAudio = MacAudio(with: self)
    }

    override open func windowDidLoad() {

        debug(.lifetime)
        
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

        // Apply all GUI related user defaults
        pref.applyUserDefaults()
        config.applyUserDefaults()

        // Setup window
        configureWindow()

        // Enable message processing
        launch()

        do {
            // Let the C64 throw an exception if it is not ready to power on
            try c64.isReady()
            
            // Start emulation
            try c64.run()

        } catch {
            
            // Open the Rom dialog
            openConfigurator(tab: "Roms")
        }

        // Add media file (if provided on startup)
        if let url = mydocument.launchUrl { try? mm.addMedia(url: url) }

        // Create speed monitor
        speedometer = Speedometer()
        
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
    
    func launch() {

        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        c64.launch(myself) { (ptr, msg: Message) in

            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()

            // Process message in the main thread
            DispatchQueue.main.async { myself.processMessage(msg) }
        }
    }
    
    //
    // Timer and message processing
    //
    
    func update(frames: Int64) {

        if frames % 5 == 0 {

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
        }

        // Do less times...
        if (frames % 16) == 0 {
            
            updateSpeedometer()
        }
        
        // Do lesser times...
        if (frames % 256) == 0 {
            
            // Let the cursor disappear in fullscreen mode
            if renderer.fullscreen &&
                CGEventSource.secondsSinceLastEventType(.combinedSessionState,
                                                        eventType: .mouseMoved) > 1.0 {
                NSCursor.setHiddenUntilMouseMoves(true)
            }
        }
    }

    func processMessage(_ msg: Message) {

        var value: Int { return Int(msg.value) }
        var driveNr: Int { return Int(msg.drive.nr) }
        var halftrack: Int { return Int(msg.drive.value) }
        var pc: Int { return Int(msg.cpu.pc) }
        var vol: Int { return Int(msg.drive.volume) }
        var pan: Int { return Int(msg.drive.pan) }

        // Only proceed if the proxy object is still alive
        if c64 == nil { return }

        switch msg.type {

        case .CONFIG:
            inspector?.fullRefresh()
            refreshStatusBar()

        case .POWER:

            if value != 0 {

                renderer.canvas.open(delay: 2)
                virtualKeyboard = nil
                toolbar.updateToolbar()
                inspector?.powerOn()

            } else {

                toolbar.updateToolbar()
                inspector?.powerOff()
            }

        case .RUN:
            needsSaving = true
            jammed = false
            toolbar.updateToolbar()
            inspector?.run()
            refreshStatusBar()

        case .PAUSE:
            toolbar.updateToolbar()
            inspector?.pause()
            refreshStatusBar()

        case .STEP:
            needsSaving = true
            inspector?.step()

        case .RESET:
            inspector?.reset()

        case .SHUTDOWN:
            shutDown()

        case .ABORT:
            debug(.shutdown, "Aborting with exit code \(value)")
            exit(Int32(value))

        case .WARP, .TRACK:
            refreshStatusBar()

        case .MUTE:
            muted = value != 0
            refreshStatusBar()

        case .CONSOLE_CLOSE:
            renderer.console.close(delay: 0.25)

        case .CONSOLE_UPDATE:
            renderer.console.isDirty = true

        case .CONSOLE_DEBUGGER:
            break

        case .SCRIPT_DONE,
                .SCRIPT_PAUSE,
                .SCRIPT_ABORT:
            renderer.console.isDirty = true

        case .SCRIPT_WAKEUP:
            c64.continueScript()
            renderer.console.isDirty = true

        case .BASIC_ROM_LOADED,
                .CHAR_ROM_LOADED,
                .KERNAL_ROM_LOADED,
                .DRIVE_ROM_LOADED:
            break

        case .ROM_MISSING:
            break

        case .BREAKPOINT_REACHED:
            inspector?.signalBreakPoint(pc: pc)

        case .WATCHPOINT_REACHED:
            inspector?.signalWatchPoint(pc: pc)

        case .CPU_JAMMED:
            jammed = true
            refreshStatusBar()

        case .CPU_JUMPED:
            inspector?.signalGoto(pc: pc)

        case .PAL, .NTSC:
            renderer.canvas.updateTextureRect()

        case .DRIVE_STEP:
            macAudio.playStepSound(volume: vol, pan: pan)
            refreshStatusBarTracks(drive: driveNr)

        case .DISK_INSERT:
            macAudio.playInsertSound(volume: vol, pan: pan)
            refreshStatusBarDiskIcons(drive: driveNr)
            inspector?.fullRefresh()

        case .DISK_EJECT:
            macAudio.playEjectSound(volume: vol, pan: pan)
            refreshStatusBarDiskIcons(drive: driveNr)
            inspector?.fullRefresh()

        case .FILE_FLASHED:
            break

        case .DISK_PROTECT,
                .DISK_SAVED,
                .DISK_UNSAVED,
                .DRIVE_LED_ON,
                .DRIVE_LED_OFF:
            refreshStatusBar()

        case .IEC_BUS_BUSY,
                .IEC_BUS_IDLE:
            refreshStatusBar()

        case .DRIVE_MOTOR_ON,
                .DRIVE_MOTOR_OFF:
            refreshStatusBar()

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

        case .VC1530_CONNECT:
            hideOrShowDriveMenus()
            refreshStatusBar()

        case .VC1530_TAPE:
            refreshStatusBar()

        case .VC1530_PLAY:
            refreshStatusBar()

        case .VC1530_MOTOR:
            refreshStatusBar()

        case .VC1530_COUNTER:
            refreshStatusBar()

        case .CRT_ATTACHED:
            refreshStatusBar()

        case .CART_SWITCH:
            break

        case .KB_AUTO_RELEASE, .KB_AUTO_PRESS:
            if virtualKeyboard?.window?.isVisible == true {
                virtualKeyboard!.refresh()
            }

        case .SHAKING:
            metal.lastShake = DispatchTime(uptimeNanoseconds: 0)
            if pref.releaseMouseByShaking {
                metal.releaseMouse()
            }

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

        case .RECORDING_ABORTED:
            refreshStatusBar()
            showAlert(.recorderAborted)

        case .DMA_DEBUG:
            value != 0 ? renderer.zoomTextureOut() : renderer.zoomTextureIn()

        case .ALARM:
            debug(.events, "Received Alarm \(msg.value)")

        default:
            warn("Unknown message: \(msg)")
            fatalError()
        }
    }

    //
    // Keyboard events
    //

    // Keyboard events are handled by the emulator window.
    // If they are handled here, some keys such as 'TAB' don't trigger an event.
}
