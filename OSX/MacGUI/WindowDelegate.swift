//
//  WindowDelegate.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 11.02.18.
//

import Foundation

extension MyController : NSWindowDelegate {
    
    public func windowDidBecomeMain(_ notification: Notification) {
        
        track()
        c64.enableAudio()
    }
    
    public func windowDidResignMain(_ notification: Notification) {
        
        track()
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

        track("Proposed size: \(proposedSize.width) x \(proposedSize.height)")
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

