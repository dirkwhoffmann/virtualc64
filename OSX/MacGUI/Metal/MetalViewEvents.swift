//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


import Foundation

public extension MetalView {

    //
    // Keyboard events
    //
    
    override func keyDown(with event: NSEvent) {

        controller.keyboardcontroller.keyDown(with: event)
    }
    
    override func keyUp(with event: NSEvent) {

        controller.keyboardcontroller.keyUp(with: event)
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        // Save modifier flags. They are needed in TouchBar code
        controller.modifierFlags = event.modifierFlags
        controller.keyboardcontroller.flagsChanged(with: event)
    }
    
    //
    // Mouse events
    //
    
    func scaledMouseCoordinate(with event: NSEvent) -> NSPoint {
        
        // Get coordinate relative to view
        let locationInView = convert(event.locationInWindow, from: nil)
        
        // Scale into range 0..1
        var x = (frame.width == 0) ? 0.0 : (locationInView.x / frame.width)
        var y = (frame.height == 0) ? 0.0 : (locationInView.y / frame.height)

        // Clamp
        x = (x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x
        y = (y < 0.0) ? 0.0 : (y > 1.0) ? 1.0 : y
        
        return NSPoint.init(x: x, y: y)
    }

    override func mouseDown(with event: NSEvent) {

        controller.c64.mouse.setLeftButton(true)
    }
    
    override func mouseUp(with event: NSEvent) {

        controller.c64.mouse.setLeftButton(false)
    }
    
    override func rightMouseUp(with event: NSEvent) {

        controller.c64.mouse.setRightButton(false)
    }
    
    override func rightMouseDown(with event: NSEvent) {

        controller.c64.mouse.setRightButton(true)
    }
    
    override func mouseMoved(with event: NSEvent) {
        
        let dx = event.deltaX
        let dy = -event.deltaY
        
        controller.mouseXY.x += dx
        controller.mouseXY.y += dy

        // Make coordinate independent of window size
        let scaleX = (256.0 * 400.0) / frame.width
        let scaleY = (256.0 * 300.0) / frame.height
        let newX = controller.mouseXY.x * scaleX
        let newY = controller.mouseXY.y * scaleY

        let newLocation = NSPoint.init(x: newX, y: newY)
        controller.c64.mouse.setXY(newLocation)
        //track("\(dx) \(dy)\n");
    }
    
    override func mouseDragged(with event: NSEvent) {

        mouseMoved(with: event)
    }
    
    override func rightMouseDragged(with event: NSEvent) {
        
        mouseMoved(with: event)
    }
}
