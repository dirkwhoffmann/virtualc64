//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController : NSWindowDelegate {
        
    public func windowDidBecomeMain(_ notification: Notification) {
        
        track()
        
        // Enable audio
        c64.sid.rampUpFromZero()
        audioEngine.startPlayback()
        
        // Start emulator if it was only paused while in background
        if pauseInBackgroundSavedState { c64.run() }

        // Register for mouse move events
        window?.acceptsMouseMovedEvents = true
        
        // Make sure the aspect ratio is correct
        adjustWindowSize()
    }
    
    public func windowDidResignMain(_ notification: Notification) {
        
        track()
        
        // Disable audio
        c64.sid.rampDown()
        audioEngine.stopPlayback()
        
        // Stop emulator if it is configured to pause in background
        pauseInBackgroundSavedState = c64.isRunning()
        if pauseInBackground { c64.halt() }
    }
    
    public func windowWillClose(_ notification: Notification) {
        
        track()
        
        // Close virtual keyboard
        virtualKeyboard?.close()
        
        // Stop timer
        timer?.invalidate()
        timer = nil
        
        // Stop audio playback
        audioEngine.stopPlayback()
        
        // Quit message queue
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        c64.removeListener(myself)
  
        // Disconnect emulator
        cpuTableView.dataSource = nil
        cpuTableView.delegate = nil
        cpuTableView.c = nil
        cpuTraceView.dataSource = nil
        cpuTraceView.delegate = nil
        cpuTraceView.c = nil
        memTableView.dataSource = nil
        memTableView.delegate = nil
        memTableView.c = nil

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
    
    public func window(_ window: NSWindow, willUseFullScreenContentSize proposedSize: NSSize) -> NSSize {

        // track("Proposed size: \(proposedSize.width) x \(proposedSize.height)")
        var myRect = metalScreen.bounds
        myRect.size = proposedSize
        return proposedSize
    }

    public func windowWillResize(_ sender: NSWindow, to frameSize: NSSize) -> NSSize {
        
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
    func adjustWindowSize() {
        
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

