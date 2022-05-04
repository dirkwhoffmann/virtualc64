// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController: NSWindowDelegate {
        
    public func windowDidBecomeMain(_ notification: Notification) {

        guard let window = notification.object as? NSWindow else { return }

        // Inform the application delegate
        myAppDelegate.windowDidBecomeMain(window)
        
        // Restart the emulator if it was paused when the window lost focus
        if pref.pauseInBackground && pauseInBackgroundSavedState { try? c64.run() }

        // Register to receive mouse movement events
        window.acceptsMouseMovedEvents = true
        
        // Make sure the aspect ratio is correct
        adjustWindowSize()
        
        // Update the status bar
        refreshStatusBar()
    }
    
    public func windowDidResignMain(_ notification: Notification) {
                
        // Stop the emulator if it is supposed to pause in background
        if c64 != nil {
            pauseInBackgroundSavedState = c64.running
            if pref.pauseInBackground { c64.pause() }
        }        
    }
    
    func windowDidResize(_ notification: Notification) {
    
        renderer?.console.resize()
    }

    public func windowShouldClose(_ sender: NSWindow) -> Bool {

        log()
        if proceedWithUnsavedFloppyDisks() {
            return true
        } else {
            return false
        }
    }

    public func windowWillClose(_ notification: Notification) {
        
        log()
        
        log("Stopping renderer...", level: 2)
        renderer.halt()

        log("Stopping timers...", level: 2)
        snapshotTimer?.invalidate()
        snapshotTimer = nil

        log("Closing auxiliary windows...", level: 2)
        inspector?.c64 = nil
        inspector?.close()
        monitor?.c64 = nil
        monitor?.close()
                        
        log("Shutting down the audio backend...", level: 2)
        macAudio.shutDown()
        
        log("Disconnecting gaming devices...", level: 2)
        gamePadManager.shutDown()
        
        log("Shutting down the emulator...", level: 2)
        c64.halt()
    }
    
    func shutDown() {
        
        log("Removing proxy...", level: 2)
        
        c64.kill()
        c64 = nil
    }
    
    public func windowWillEnterFullScreen(_ notification: Notification) {

        log()
        renderer.fullscreen = true
        showStatusBar(false)
    }
    
    public func  windowDidEnterFullScreen(_ notification: Notification) {

        log()
    }
    
    public func windowWillExitFullScreen(_ notification: Notification) {

        log()
        renderer.fullscreen = false
        showStatusBar(true)
    }
    
    public func windowDidExitFullScreen(_ notification: Notification) {

        log()
    }
    
    public func window(_ window: NSWindow, willUseFullScreenPresentationOptions proposedOptions: NSApplication.PresentationOptions = []) -> NSApplication.PresentationOptions {
        
        log()
        let autoHideToolbar = NSApplication.PresentationOptions.autoHideToolbar
        var options = NSApplication.PresentationOptions(rawValue: autoHideToolbar.rawValue)
        options.insert(proposedOptions)
        return options
    }
    
    public func window(_ window: NSWindow, willUseFullScreenContentSize proposedSize: NSSize) -> NSSize {

        var myRect = metal.bounds
        myRect.size = proposedSize
        return proposedSize
    }
    
    // Fixes a NSSize to match our desired aspect ration
    func fixSize(window: NSWindow, size: NSSize) -> NSSize {
        
        // Get some basic parameters
        let windowFrame = window.frame
        let deltaX = size.width - windowFrame.size.width
        let deltaY = size.height - windowFrame.size.height
        
        // How big would the metal view become?
        let metalFrame = metal.frame
        let metalX = metalFrame.size.width + deltaX
        let metalY = metalFrame.size.height + deltaY
        
        // We want to achieve an aspect ratio of 804:621
        let newMetalX  = metalY * (804.0 / 621.0)
        let dx = newMetalX - metalX
        
        return NSSize(width: size.width + dx, height: size.height)
    }

    // Fixes a NSRect to match our desired aspect ration
    func fixRect(window: NSWindow, rect: NSRect) -> NSRect {
        
        let newSize = fixSize(window: window, size: rect.size)
        let newOriginX = (rect.width - newSize.width) / 2.0
        
        return NSRect(x: newOriginX, y: 0, width: newSize.width, height: newSize.height)
    }
    
    public func windowWillResize(_ sender: NSWindow, to frameSize: NSSize) -> NSSize {
                
        return fixSize(window: sender, size: frameSize)
    }
    
    public func windowWillUseStandardFrame(_ window: NSWindow,
                                           defaultFrame newFrame: NSRect) -> NSRect {

        return fixRect(window: window, rect: newFrame)
    }
}

extension MyController {
    
    func adjustWindowSize(_ dv: CGFloat = 0.0) {

        // Only proceed in window mode
        if renderer?.fullscreen == true { return }

        // Get window frame
        guard var frame = window?.frame else { return }

        // Modify the frame height
        frame.origin.y -= dv
        frame.size.height += dv

        // Compute the size correction
        let newsize = windowWillResize(window!, to: frame.size)
        let correction = newsize.height - frame.size.height

        // Adjust frame
        frame.origin.y -= correction
        frame.size = newsize

        window!.setFrame(frame, display: true)          }
}
