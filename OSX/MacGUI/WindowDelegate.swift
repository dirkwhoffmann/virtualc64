//
//  WindowDelegate.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 11.02.18.
//

import Foundation

extension MyController : NSWindowDelegate {
    
    open override func mouseDown(with event: NSEvent) {

        track();
    }
    
    open override func mouseMoved(with event: NSEvent) {
       
        // let x = Int(NSEvent.mouseLocation.x)
        // let y = Int(NSEvent.mouseLocation.y)
        
        // Compute mouse position relative to the emulator window
        let locationInView = metalScreen.convert(event.locationInWindow, from: nil)
        let width = metalScreen.frame.width
        let height = metalScreen.frame.height
        let x = (width == 0) ? 0 : Double(locationInView.x / width)
        let y = (height == 0) ? 0 : Double(locationInView.y / height)
        
        // Translate into C64 coordinates (this is a hack)
        let scalex = 380
        let scaley = 267 
        var c64x = Int(x * Double(scalex)) + 22
        var c64y = Int(y * Double(scaley)) + 10
        c64x = (c64x < 0) ? 0 : (c64x > scalex) ? scalex : c64x
        c64y = (c64y < 0) ? 0 : (c64y > scaley) ? scaley : c64y

        track("Location = (\(x),\(y)) (\(c64x),\(c64y))");
        
        /* Mouse  movement  is  tracked internally within the mouse. The
         * position of the mouse MOD 64  is  transmitted to the SID  POTX
         * and  POTY  registers  every  512  microsecond  and requires no
         * software intervention.
         * The  POTX  register  is  used  to read X position of the mouse
         * and the POTY register is used to read Y position of the mouse.
         * The register contents are as follows:
         *
         *                  +-------------------------------+
         * Bit Position     | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
         *                  +---+---+---+---+---+---+---+---+
         * POT Register     | X | P5| P4| P3| P2| P1| P0| N |
         *                  +-------------------------------+
         */
        let potX = UInt8((c64x % 64) << 1)
        let potY = UInt8((c64y % 64) << 1)

        c64.sid.setPotX(potX)
        c64.sid.setPotY(potY)
    }
    
    public func windowDidBecomeMain(_ notification: Notification) {
        
        // track()
        c64.enableAudio()
        
        // Register for mouse move events
        window?.acceptsMouseMovedEvents = true
    }
    
    public func windowDidResignMain(_ notification: Notification) {
        
        // track()
        c64.disableAudio()
    }
    
    public func windowWillClose(_ notification: Notification) {
        
        track()
        
        // Stop timer
        timer.invalidate()
        timer = nil
        
        // Disconnect emulator
        c64.setListener(nil, function: nil)
        memTableView.dataSource = nil
        memTableView.delegate = nil
        cpuTableView.dataSource = nil
        cpuTableView.delegate = nil
        
        // Stop metal view
        metalScreen.cleanup()
    }
    
    public func windowWillEnterFullScreen(_ notification: Notification)
    {
        track()
        metalScreen.fullscreen = true
        showStatusBar(false)
    }
    
    public func  windowDidEnterFullScreen(_ notification: Notification)
    {
        track()
    }
    
    public func windowWillExitFullScreen(_ notification: Notification)
    {
        track()
        metalScreen.fullscreen = false
        showStatusBar(true)
    }
    
    public func windowDidExitFullScreen(_ notification: Notification)
    {
        track()
    }
    
    public func window(_ window: NSWindow, willUseFullScreenPresentationOptions proposedOptions: NSApplication.PresentationOptions = []) -> NSApplication.PresentationOptions {
        
        track()
        let autoHideToolbar = NSApplication.PresentationOptions.autoHideToolbar
        var options = NSApplication.PresentationOptions.init(rawValue: autoHideToolbar.rawValue)
        options.insert(proposedOptions)
        return options
    }
    
    @objc public func window(_ window: NSWindow, willUseFullScreenContentSize proposedSize: NSSize) -> NSSize {

        // track("Proposed size: \(proposedSize.width) x \(proposedSize.height)")
        var myRect = metalScreen.bounds
        myRect.size = proposedSize
        return proposedSize
    }

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
}

extension MyController {
    
    /// Adjusts the windows vertical size programatically
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
}

