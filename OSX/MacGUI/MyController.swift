//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation


extension MyController {
    
    // --------------------------------------------------------------------------------
    //                               Life cycle
    // --------------------------------------------------------------------------------

    override open func awakeFromNib() {

        track()
    }
    
    override open func windowDidLoad() {
 
        track()

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
        gamepadSlotA = -1 // No gampad assigned
        gamepadSlotB = -1
        validateJoystickToolbarItems()
        
        // Setup window properties
        configureWindow()
        
        // TODO: GET RID OF THIS: Move to it's own window controller
        setHexadecimalAction(self)
        cpuTableView.setController(self)
        memTableView.setController(self)
        cheatboxImageBrowserView.setController(self)
        
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
        // TODO: Implement as class extension in Swift
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
    
            // Close ROM dialog if open
            // TODO: MAKE SURE THAT DIALOG IS ALREADY CLOSED
            /*
            if (romDialog != nil) {
                romDialog.orderOut(nil)
                window?.endSheet(romDialog, returnCode: .cancel)
                romDialog = nil
            }
            */
            
            // Start emulator
            c64.run()
            metalScreen.blendIn()
            metalScreen.drawC64texture = true
    
            // Show mount dialog if an attachment is present
            showMountDialog()
            break;
    
        case MSG_RUN:
            
            enableUserEditing(false)
            refresh()
            cheatboxPanel.close()
 
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
             MSG_KERNEL_ROM_LOADED,
             MSG_VC1541_ROM_LOADED:
            
            break
            // Update ROM dialog
            /*
            if romDialog != nil {
                romDialog.update(Int32(c64.missingRoms()))
            }
            break
            */
            
        case MSG_ROM_MISSING:

            track("MSG_ROM_MISSING")
            let nibName = NSNib.Name(rawValue: "RomDialog")
            let dialogController = RomDialogController.init(windowNibName: nibName)
            dialogController.showSheet(withParent: self)
            break
            
        case MSG_SNAPSHOT_TAKEN:

            // Update TouchBar with new snapshpot image
            rebuildTouchBar()
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
    //                              Keyboard events
    // --------------------------------------------------------------------------------

    /*
    override open func resignFirstResponder() -> Bool
    {
        track()
        return true
    }
    */
    
    /*
    override func becomeFirstResonder()
    {
        track()
    }
    */
    
    override open func keyDown(with event: NSEvent) {

        track()
        keyboardcontroller.keyDown(with: event)
    }
    
    override open func keyUp(with event: NSEvent) {
        
        track()
        keyboardcontroller.keyUp(with: event)
    }
    
    override open func flagsChanged(with event: NSEvent) {
        
        let flags = event.modifierFlags
        
        // Save modifier flags. They are needed in TouchBar code
        modifierFlags = flags
        
        keyboardcontroller.flagsChanged(with: event)
    }
    
 
    // --------------------------------------------------------------------------------
    //                               Game pad events
    // --------------------------------------------------------------------------------

    /// GamePadManager delegation method
    /// - Returns: true, iff a joystick event has been triggered on port A or port B
    @discardableResult
    func joystickEvent(slot: Int, event: JoystickEvent) -> Bool {
        
        if (slot == gamepadSlotA) {
            c64.joystickA.trigger(event)
            return true
        }

        if (slot == gamepadSlotB) {
            c64.joystickB.trigger(event)
            return true
        }
        
        return false
    }    

    // --------------------------------------------------------------------------------
    //                                 Modal dialogs
    // --------------------------------------------------------------------------------

    @objc func showMountDialog() {
        
        track()
        
        // Does this document have an attachment?
        let document = self.document as! MyDocument
        guard let attachment = document.attachment else {
            return
        }
        
        // Which mount dialog should we use?
        var controller: UserDialogController!
        switch attachment.type() {
        case T64_CONTAINER,
             PRG_CONTAINER,
             P00_CONTAINER,
             D64_CONTAINER:
            let nibName = NSNib.Name(rawValue: "ArchiveMountDialog")
            controller = ArchiveMountController.init(windowNibName: nibName)
            break
            
        case G64_CONTAINER,
             NIB_CONTAINER:
            let nibName = NSNib.Name(rawValue: "DiskMountDialog")
            controller = DiskMountController.init(windowNibName: nibName)
            break

        case CRT_CONTAINER:
            track()
            
            // Check if we support this cartridge
            let crt = attachment as! CRTProxy
            if !crt.isSupported() {
                showUnsupportedCartridgeAlert(crt)
                return
            }
            
            let nibName = NSNib.Name(rawValue: "CartridgeMountDialog")
            controller = CartridgeMountController.init(windowNibName: nibName)
            break

        case TAP_CONTAINER:
            let nibName = NSNib.Name(rawValue: "TapeMountDialog")
            controller = TapeMountController.init(windowNibName: nibName)
            break
            
        default:
            // There is no mount dialog availabe for the attachments type
            return
        }
        
        controller.showSheet(withParent: self)
    }

    // --------------------------------------------------------------------------------
    //                         Action methods (main screen)
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
