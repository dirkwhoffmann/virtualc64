//
//  MetalViewEvents.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 13.05.18.
//

import Foundation

public extension MetalView {

    //
    // Keyboard events
    //
    
    override public func keyDown(with event: NSEvent)
    {
        // track()
        controller.keyboardcontroller.keyDown(with: event)
    }
    
    override public func keyUp(with event: NSEvent)
    {
        controller.keyboardcontroller.keyUp(with: event)
    }
    
    override public func flagsChanged(with event: NSEvent) {
        
        // Save modifier flags. They are needed in TouchBar code
        controller.modifierFlags = event.modifierFlags
        controller.keyboardcontroller.flagsChanged(with: event)
    }
    
    //
    // Mouse events
    //
    
    func scaledMouseCoordinate(with event: NSEvent) -> NSPoint
    {
        // Get coordinate relative to view
        let locationInView = convert(event.locationInWindow, from: nil)
        
        // Scale into range 0..1
        var x = (frame.width == 0) ? 0.0 : (locationInView.x / frame.width)
        var y = (frame.height == 0) ? 0.0 : (locationInView.y / frame.height)

        // Clamp
        x = (x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x
        y = (y < 0.0) ? 0.0 : (y > 1.0) ? 1.0 : y
        
        return NSMakePoint(x, y)
    }
    
    override public func mouseEntered(with event: NSEvent)
    {
        // track()
        NSCursor.hide();
    }
    
    override public func mouseExited(with event: NSEvent)
    {
        // track()
        NSCursor.unhide()
    }
    
    override public func mouseDown(with event: NSEvent) {
        
        // track()
        controller.c64.setMouseLeftButton(true)
    }
    
    override public func mouseUp(with event: NSEvent)
    {
        // track()
        controller.c64.setMouseLeftButton(false)
    }
    
    override public func rightMouseUp(with event: NSEvent)
    {
        // track()
        controller.c64.setMouseRightButton(false)
    }
    
    override public func rightMouseDown(with event: NSEvent)
    {
        // track()
        controller.c64.setMouseRightButton(true)
    }
    
    override public func mouseMoved(with event: NSEvent) {
        
        // Check for Command key (mouse callibration mode)
        let cmdKeyPressed = controller.keyboardcontroller.command
        
        // Compute mouse position relative to the emulator window
        let locationInView = scaledMouseCoordinate(with: event)
        
        // track("\(locationInView.x) \(locationInView.y)\n");
        controller.c64.setMouseXY(locationInView, silent: cmdKeyPressed)
    }
    
    override public func mouseDragged(with event: NSEvent)
    {
        mouseMoved(with: event)
    }
    
    override public func rightMouseDragged(with event: NSEvent)
    {
        mouseMoved(with: event)
    }
}
