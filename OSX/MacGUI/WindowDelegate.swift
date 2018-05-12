//
//  WindowDelegate.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 11.02.18.
//

import Foundation

extension MyController : NSWindowDelegate {
    
    func mousePort() -> ControlPortProxy?
    {
        return
            (gamepadSlot1 == InputDevice.analogMouse) ? c64.port1 :
                (gamepadSlot2 == InputDevice.analogMouse) ? c64.port2 : nil
    }
    
    func convertC64(_ point: NSPoint, frame: NSRect) -> NSPoint
    {
        let x = (frame.width == 0) ? 0 : (point.x / frame.width)
        let y = (frame.height == 0) ? 0 : (point.y / frame.height)
        
        // Translate into C64 coordinate system (this is a hack)
        let xmax = CGFloat(380.0)
        let ymax = CGFloat(268.0)
        var c64x = x * xmax + 22
        var c64y = y * ymax + 10
        c64x = (c64x < 0.0) ? 0.0 : (c64x > xmax) ? xmax : c64x
        c64y = (c64y < 0.0) ? 0.0 : (c64y > ymax) ? ymax : c64y
        
        return NSMakePoint(c64x,c64y)
    }
    
    open override func mouseEntered(with event: NSEvent) {
        track()
        NSCursor.hide();
    }
    
    open override func mouseExited(with event: NSEvent) {
        track()
        NSCursor.unhide();
    }
    
    open override func mouseDown(with event: NSEvent) {
        track();
        mousePort()?.trigger(PRESS_FIRE)
    }
    
    open override func mouseUp(with event: NSEvent) {
        track();
        mousePort()?.trigger(RELEASE_FIRE)
    }
    
    open override func mouseMoved(with event: NSEvent) {

        // var port : ControlPortProxy
        
        // Return if no virtual mouse is connected
        
        if (gamepadSlot1 != InputDevice.analogMouse) &&
            (gamepadSlot2 != InputDevice.analogMouse) {
            return
        }
        
        // Compute mouse position relative to the emulator window
        let locationInView = metalScreen.convert(event.locationInWindow, from: nil)
        let locationInC64 = convertC64(locationInView, frame: metalScreen.frame)
        
        // Pass values to mouse
        c64.setMouseTargetX(Int(locationInC64.x))
        c64.setMouseTargetY(Int(locationInC64.y))
    }
    
    override open func rightMouseUp(with event: NSEvent) {
        
        track()
        
        // Compute mouse position relative to the emulator window
        let locationInView = metalScreen.convert(event.locationInWindow, from: nil)
        let locationInC64 = convertC64(locationInView, frame: metalScreen.frame)
        
        // Calibrate mouse position
        c64.setMouseX(Int(locationInC64.x))
        c64.setMouseTargetX(Int(locationInC64.x))
        c64.setMouseY(Int(locationInC64.y))
        c64.setMouseTargetY(Int(locationInC64.y))
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

