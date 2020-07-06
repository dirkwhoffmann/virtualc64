//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

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

    /// Proxy object.
    /// Implements a bridge between the emulator written in C++ and the
    /// GUI written in Swift. Because Swift cannot interact with C++ directly,
    //  the proxy is written in Objective-C.
    var c64: C64Proxy!
    
    /// Audio Engine
    var audioEngine: AudioEngine!
    
    /// Game pad manager
    var gamePadManager: GamePadManager!
    
    /// Keyboard controller
    var keyboardcontroller: KeyboardController!

    /// Virtual C64 keyboard (opened as a sheet)
    var virtualKeyboardSheet: VirtualKeyboardController?
    
    /// Preferences controller
    var preferencesController: PreferencesController?

    /// Loop timer
    /// The timer fires 60 times a second and executes all tasks that need to be
    //  done perdiodically (e.g., updating the speedometer and the debug panels)
    var timer: Timer?
    
    /// Lock to prevent reentrance into the timer function
    var timerLock = NSLock()
    
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

    /// Rom file URLs
    var basicRomURL: URL = Defaults.basicRom
    var charRomURL: URL = Defaults.charRom
    var kernalRomURL: URL = Defaults.kernalRom
    var vc1541RomURL: URL = Defaults.vc1541Rom

    /// Screenshot resolution (0 = low, 1 = high)
    var screenshotSource = Defaults.screenshotSource
    
    /// Screenshot image format
    var screenshotTarget = Defaults.screenshotTarget
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
    var closeWithoutAsking = Defaults.closeWithoutAsking

    /// Indicates if the user should be warned if an unsaved disk is ejected.
    var ejectWithoutAsking = Defaults.ejectWithoutAsking

    /// Indicates if the emulator should pause when it looses focus.
    var pauseInBackground =  Defaults.pauseInBackground
    
    /// Remembers if the emulator was running or paused when it lost focus.
    /// Needed to implement the pauseInBackground feature.
    var pauseInBackgroundSavedState = false

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
 
    // Provides the document casted to the correct type
    var mydocument: MyDocument? { return document as? MyDocument }
    
    /// Indicates if the emulator needs saving
    var needsSaving: Bool {
        get {
            return document?.changeCount != 0
        }
        set {
            if newValue && !closeWithoutAsking {
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
        
        // Create audio engine
        audioEngine = AudioEngine.init(withSID: c64.sid)
    }

    override open func windowDidLoad() {
 
        track()
        
        // Reset mouse coordinates
        mouseXY = NSPoint.zero
        hideMouse = false
        
        // Create keyboard controller
        keyboardcontroller = KeyboardController(controller: self)
        if keyboardcontroller == nil {
            track("Failed to create keyboard controller")
            return
        }

        // Create game pad manager
        gamePadManager = GamePadManager(controller: self)
        if gamePadManager == nil {
            track("Failed to create game pad manager")
            return
        }
        
        // Setup toolbar, window, and debugger
        configureToolbar()
        configureWindow()
        setupDebugger()
        
        // Get metal running
        metalScreen.setupMetal()
    
        // Load user defaults
        loadUserDefaults()
        
        // Enable message processing (register callback)
        addListener()
        
        // Check if the C64 is ready to power on
        if c64.isReady() {
            
            // Power on the C64
            c64.powerOn()
            
            // Launch the emulator thread
            c64.run()
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
        
        // Do 3 times a second ...
        if (animationCounter % 4) == 0 {
            speedometer.updateWith(cycle: c64.cpu.cycle(), frame: metalScreen.frames)
            let mhz = speedometer.mhz(digits: 2)
            let fps = speedometer.fps(digits: 0)
            clockSpeed.stringValue = String(format: "%.2f MHz %.0f fps", mhz, fps)
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
            return msg.data == 1
        }

        proxyLock.lock()
        
        switch msg.type {
    
        case MSG_READY_TO_RUN:
    
            // Start emulator
            c64.run()
            
            // Blend in C64 screen
            if !metalScreen.drawC64texture {
                metalScreen.blendIn()
                metalScreen.drawC64texture = true
            }
            
            // Process attachment (if any)
            mydocument?.mountAttachment()
    
        case MSG_POWER_ON:
            track("MSG_POWER_ON")
            
        case MSG_POWER_OFF:
            track("MSG_POWER_OFF")
            
        case MSG_RUN:
            track("MSG_RUN")
            needsSaving = true
            disableUserEditing()
            toolbar.validateVisibleItems()
            refresh()
    
        case MSG_PAUSE:
            track("MSG_PAUSE")
            enableUserEditing()
            toolbar.validateVisibleItems()
            refresh()
    
        case MSG_RESET:
            track("MSG_RESET")

        case MSG_BASIC_ROM_LOADED,
             MSG_CHAR_ROM_LOADED,
             MSG_KERNAL_ROM_LOADED,
             MSG_VC1541_ROM_LOADED:
            break
            
        case MSG_ROM_MISSING:
            openPreferences()

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

            metalScreen.updateScreenGeometry()
    
        case MSG_KEYMATRIX,
             MSG_CHARSET:
            
            let appDelegate = NSApp.delegate as? MyAppDelegate
            appDelegate?.virtualKeyboard?.refresh()

            // virtualKeyboard?.refresh()
            virtualKeyboardSheet?.refresh()

        case MSG_VC1541_ATTACHED:
            
            let image = NSImage.init(named: "LEDgreen")
            
            if firstDrive() {
                greenLED1.image = image
            } else {
                greenLED2.image = image
            }

        case MSG_VC1541_DETACHED:
            
            let image = NSImage.init(named: "LEDgray")
            
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
    
    func openPreferences() {
        
        if preferencesController == nil {
            let nibName = NSNib.Name("Preferences")
            preferencesController = PreferencesController.init(windowNibName: nibName)
        }
        preferencesController!.showSheet()
    }
    
    //
    // Loading Roms
    //
    
    @discardableResult
    func loadRom(_ url: URL?) -> Bool {
        
        if url == nil {
            return false
        }
        
        if c64.loadBasicRom(url!) {
            basicRomURL = url!
            return true
        }
        if c64.loadCharRom(url!) {
            charRomURL = url!
            return true
        }
        if c64.loadKernalRom(url!) {
            kernalRomURL = url!
            return true
        }
        if c64.loadVC1541Rom(url!) {
            vc1541RomURL = url!
            return true
        }
        
        track("ROM file \(url!) not found")
        return false
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
