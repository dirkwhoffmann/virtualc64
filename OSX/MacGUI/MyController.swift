//
//  MyController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 29.01.18.
//

import Foundation

// --------------------------------------------------------------------------------
//                          Window life cycle methods
// --------------------------------------------------------------------------------

extension MyController : NSWindowDelegate {
 
    @objc public func windowWillResize(_ sender: NSWindow, to frameSize: NSSize) -> NSSize {

        // Get some basic parameters
        let windowFrame = sender.frame
        let deltaX = frameSize.width - windowFrame.size.width
        let deltaY = frameSize.height - windowFrame.size.height
    
        // How big would the metal view become?
        let metalFrame = metalScreen.frame
        let metalX = metalFrame.size.width + deltaX
        let metalY = metalFrame.size.height + deltaY
    
        // We want to achieve an aspect ratio of 804:621
        let newMetalY  = metalX * (621.0 / 804.0)
        let correction = newMetalY - metalY
    
        return NSMakeSize(frameSize.width, frameSize.height + correction)
    }
    
    /// Adjusts the window size programatically
    /// The size is adjusted to get the metal view's aspect ration right
    
    @objc func adjustWindowSize() {
        
        track()
        if var frame = window?.frame {
    
            // Compute size correction
            let newsize = windowWillResize(window!, to: frame.size)
            let correction = newsize.height - frame.size.height
    
            // Adjust frame
            frame.origin.y -= correction;
            frame.size = newsize;
    
            window!.setFrame(frame, display: true)
        }
    }

    @objc public func windowWillClose(_ notification: Notification) {
        
        track()
        
        // Stop timer
        timer.invalidate()
        timer = nil
        
        // Stop metal view
        metalScreen.cleanup()
    }
}

extension MyController {

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
    
    func createTimer() {
    
        // Create speed monitor
        // TODO: Implement as class extension in Swift
        speedometer = Speedometer()
        fps = PAL_REFRESH_RATE;
        mhz = Double(CLOCK_FREQUENCY_PAL) / 100000.0;
        
        // Create timer and speedometer
        timerLock = NSLock()
        timer = Timer.scheduledTimer(timeInterval: 1.0/24.0, // 24 times a second
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
        while let message = c64.message() {
            processMessage(message)
        }
 
        // Do 12 times a second ...
        if (animationCounter % 2) == 0 {
 
            // Refresh debug panel if open
            if c64.isRunning() {
                let state = debugPanel.state
                if state == NSDrawerState.open || state == NSDrawerState.opening {
                    refresh()
                }
            }
        }
        
        // Do 6 times a second ...
        if (animationCounter % 4) == 0 {
 
            // Update tape progress icon
            // Note: The tape progress icon is not switched on or off by a "push" message,
            // because some games continously switch on and off the datasette motor.
            // This would quickly overflow the message queue.
            if (c64.tapeBusIsBusy) {
                tapeProgress.startAnimation(self)
            } else {
                tapeProgress.stopAnimation(self)
            }
/*
            if ([[c64 datasette] motor] != [c64 tapeBusIsBusy]) {
            if ([[c64 datasette] motor] && [[c64 datasette] playKey]) {
            [tapeProgress startAnimation:nil];
            [c64 setTapeBusIsBusy:YES];
            } else {
            [tapeProgress stopAnimation:nil];
            [c64 setTapeBusIsBusy:NO];
             }}
 */
        }
        
        // Do 3 times a second ...
        if (animationCounter % 8) == 0 {
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
    // Action methods (Drive)
    // --------------------------------------------------------------------------------
    
 
    // --------------------------------------------------------------------------------
    // Action methods (Cartridge)
    // --------------------------------------------------------------------------------

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
  
        NSLog("\(#function)")
        c64.detachCartridgeAndReset()
    }
    
    
}
