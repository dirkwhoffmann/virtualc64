//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

extension MyController {

    // Get the undo manager from the first responder (metalScreen)
    override open var undoManager: UndoManager? { get { return metalScreen.undoManager } }

    //
    // Initialization
    //

    override open func awakeFromNib() {

        track()
        cpuTableView.c = self
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
        
        var none = NSImage(named: NSImage.Name(rawValue: "oxygen_none.png"))
        none = none?.resizeImage(width: 32, height: 32, cutout: cutout)
        var keyset = NSImage(named: NSImage.Name(rawValue: "oxygen_keys.png"))
        keyset = keyset?.resizeImage(width: 32, height: 32, cutout: cutout)
        var mouse = NSImage(named: NSImage.Name(rawValue: "oxygen_mouse.png"))
        mouse = mouse?.resizeImage(width: 32, height: 32, cutout: cutout)
        var gamepad = NSImage(named: NSImage.Name(rawValue: "crystal_gamepad.png"))
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
        
        c64.setListener(myself) { (ptr, msg) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<MyController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                myself.processMessage(VC64Message(UInt32(msg)))
            }
        }
        
        track("Listener is in place")
    }
    
    func createTimer() {
    
        // Create speed monitor
        speedometer = Speedometer()
        
        // Create timer and speedometer
        timerLock = NSLock()
        timer = Timer.scheduledTimer(timeInterval: 1.0/12.0, // 12 times a second
                                     target: self,
                                     selector: #selector(timerFunc),
                                     userInfo: nil,
                                     repeats: true)
        
        track("GUI timer is up and running")
    }
    
// --------------------------------------------------------------------------------
//                           Timer and message processing
// --------------------------------------------------------------------------------

    @objc func timerFunc() {

        precondition(timerLock != nil)
        timerLock.lock()
 
        animationCounter += 1
        
        // Process all pending messages
        /*
        var msg: VC64Message = c64.message()
        while msg != MSG_NONE {
            processMessage(msg)
            msg = c64.message()
        }
        */

        // Do 12 times a second ...
        if (animationCounter % 1) == 0 {
 
            // Refresh debug panel if open
            if c64.isRunning() {
                let state = debugPanel.state
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
 
    func processMessage(_ msg: VC64Message) {

        // track("Message \(msg)")
    
        switch (msg) {
    
        case MSG_READY_TO_RUN:
    
            // Start emulator
            c64.run()
            metalScreen.blendIn()
            metalScreen.drawC64texture = true
    
            // Show mount dialog if an attachment is present
            processAttachment()
            break;
    
        case MSG_RUN:
            
            enableUserEditing(false)
            refresh()
 
            // Disable undo because the internal state changes permanently
            document?.updateChangeCount(.changeDone)
            undoManager?.removeAllActions()
            break
    
        case MSG_HALT:
            
            enableUserEditing(true)
            refresh()
            break
    
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
            break
            
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
            break
    
        case MSG_PAL,
             MSG_NTSC:

            metalScreen.updateScreenGeometry()
            break
    
        case MSG_VC1541_ATTACHED:
            
            greenLED.image = NSImage.init(named: NSImage.Name(rawValue:"LEDgreen"))
            break

        case MSG_VC1541_DETACHED:
            
            greenLED.image = NSImage.init(named: NSImage.Name(rawValue:"LEDgray"))
            break

        case MSG_VC1541_ATTACHED_SOUND:
            
            // Not sure about the copyright of the following sound:
            // [[c64 vc1541] playSound:@"1541_power_on_0" volume:0.2];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            c64.vc1541.playSound("drive_click", volume: 1.0)
            break
        
        case MSG_VC1541_DETACHED_SOUND:
            
            // Not sure about the copyright of the following sound:
            // [[c64 vc1541] playSound:@"1541_track_change_0" volume:0.6];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            c64.vc1541.playSound("drive_click", volume: 1.0)
            break
    
        case MSG_VC1541_DISK_SOUND:
            
            // [[c64 vc1541] playSound:@"1541_door_closed_2" volume:0.2];
            c64.vc1541.playSound("drive_snatch_uae", volume: 0.1)
            break
            
        case MSG_VC1541_NO_DISK_SOUND:
            
            // [[c64 vc1541] playSound:@"1541_door_open_1" volume:0.2];
            c64.vc1541.playSound("drive_snatch_uae", volume: 0.1)
            break
            
        case MSG_VC1541_HEAD_UP_SOUND:
            
            // Not sure about the copyright of the following sound:
            // [[c64 vc1541] playSound:@"1541_track_change_0" volume:0.6];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            c64.vc1541.playSound("drive_click", volume: 1.0)
            break
            
        case MSG_VC1541_HEAD_DOWN_SOUND:
            
            // Not sure about the copyright of the following sound:
            // [[c64 vc1541] playSound:@"1541_track_change_2" volume:1.0];
            // Sound from Commodore 64 (C64) Preservation Project (c64preservation.com):
            c64.vc1541.playSound("drive_click", volume: 1.0)
            break
            
        case MSG_VC1541_DISK:
            
            driveIcon.isHidden = false
            break
  
        case MSG_VC1541_NO_DISK:
            
            driveIcon.isHidden = true
            break
            
        case MSG_VC1541_RED_LED_ON:
            
            redLED.image = NSImage.init(named: NSImage.Name(rawValue: "LEDred"))
            redLED.setNeedsDisplay()
            break
            
        case MSG_VC1541_RED_LED_OFF:
            
            redLED.image = NSImage.init(named: NSImage.Name(rawValue: "LEDgray"))
            redLED.setNeedsDisplay()
            break
    
        case MSG_VC1541_DATA_ON:

            c64.iecBusIsBusy = true
            break
    
        case MSG_VC1541_DATA_OFF:
            
            c64.iecBusIsBusy = false
            break
            
        case MSG_VC1541_MOTOR_ON,
             MSG_VC1541_MOTOR_OFF,
             MSG_VC1541_HEAD_UP,
             MSG_VC1541_HEAD_DOWN:
            break
    
        case MSG_VC1530_TAPE:
            
            tapeIcon.isHidden = false
            break

        case MSG_VC1530_NO_TAPE:
            
            tapeIcon.isHidden = true
            break

        //case MSG_VC1530_PLAY:
        //    break
    
        case MSG_VC1530_PROGRESS:
            break
    
        case MSG_CARTRIDGE:
            
            cartridgeIcon.isHidden = false
            break
    
        case MSG_NO_CARTRIDGE:
         
            cartridgeIcon.isHidden = true
            break
            
        default:
            track("Unknown message: \(msg)")
            assert(false)
            break
        }
    }

    // --------------------------------------------------------------------------------
    //  Keyboard events
    // --------------------------------------------------------------------------------

    // Keyboard events are handled by the emulator window.
    // If they are handled here, some keys such as 'TAB' don't trigger an event.
 
    // --------------------------------------------------------------------------------
    //  Game pad events
    // --------------------------------------------------------------------------------

    /// GamePadManager delegation method
    /// - Returns: true, iff a joystick event has been triggered on port A or port B
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

    // --------------------------------------------------------------------------------
    // Attachment processing
    // --------------------------------------------------------------------------------

    /// System-level entry point for processing attachment
    /// According to the attachment type and the user preferences, a user dialog is
    /// presented before loading the attachment into the emulator.
    func processAttachment() {
       
        // Get attachment from document
        let document = self.document as! MyDocument
        guard let attachment = document.attachment else {
            return
        }
        
        // Process according to attachment type
        switch attachment.type() {
            
        case V64_CONTAINER:
            c64.load(fromSnapshot: attachment as! SnapshotProxy)
            return
            
        case CRT_CONTAINER:
            let cartridge = attachment as! CRTProxy
            if !cartridge.isSupported() {
                showUnsupportedCartridgeAlert(cartridge)
                return
            }
            let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
            let controller = CartridgeMountController.init(windowNibName: nibName)
            controller.showSheet(withParent: self)
            return
            
        case TAP_CONTAINER:
            let nibName = NSNib.Name(rawValue: "TapeMountDialog")
            let controller = TapeMountController.init(windowNibName: nibName)
            controller.showSheet(withParent: self)
            return
            
        case T64_CONTAINER, PRG_CONTAINER, P00_CONTAINER, D64_CONTAINER:
            if autoMount {
                c64.insertDisk(attachment as! ArchiveProxy)
            } else {
                let nibName = NSNib.Name(rawValue: "ArchiveMountDialog")
                let controller = ArchiveMountController.init(windowNibName: nibName)
                controller.showSheet(withParent: self)
            }
            return
            
        case G64_CONTAINER, NIB_CONTAINER:
            if autoMount {
                c64.insertDisk(attachment as! ArchiveProxy)
            } else {
                let nibName = NSNib.Name(rawValue: "DiskMountDialog")
                let controller = DiskMountController.init(windowNibName: nibName)
                controller.showSheet(withParent: self)
            }
            return
            
        default:
            track("Unknown attachment type")
            fatalError()
        }
    }
    
    // --------------------------------------------------------------------------------
    // Action methods (main screen)
    // --------------------------------------------------------------------------------
    
    @IBAction func alwaysWarpAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.alwaysWarpAction(sender)
        }
    
        c64.setAlwaysWarp(!c64.alwaysWarp())
        refresh()
    }
    
    
    // --------------------------------------------------------------------------------
    // Action methods (Cartridge)
    // --------------------------------------------------------------------------------

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
  
        c64.detachCartridgeAndReset()
    }
    
    
}
