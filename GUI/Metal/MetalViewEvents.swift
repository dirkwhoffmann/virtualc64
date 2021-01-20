// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public extension MetalView {

    //
    // Keyboard events
    //
    
    override func keyDown(with event: NSEvent) {

        parent.keyboard.keyDown(with: event)
    }
    
    override func keyUp(with event: NSEvent) {

        parent.keyboard.keyUp(with: event)
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        // Save modifier flags. They are needed in TouchBar code
        parent.modifierFlags = event.modifierFlags
        parent.keyboard.flagsChanged(with: event)
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

        mouse!.processMouseEvents(events: [.PRESS_LEFT])
    }
    
    override func mouseUp(with event: NSEvent) {

        mouse!.processMouseEvents(events: [.RELEASE_LEFT])
    }

    override func rightMouseDown(with event: NSEvent) {

        mouse!.processMouseEvents(events: [.PRESS_RIGHT])
    }

    override func rightMouseUp(with event: NSEvent) {

        mouse!.processMouseEvents(events: [.RELEASE_RIGHT])
    }
        
    override func mouseMoved(with event: NSEvent) {
        
        let dx = event.deltaX
        let dy = -event.deltaY
        
        parent.mouseXY.x += dx
        parent.mouseXY.y += dy

        // Make coordinate independent of window size
        let scaleX = (256.0 * 400.0) / frame.width
        let scaleY = (256.0 * 300.0) / frame.height
        let newX = parent.mouseXY.x * scaleX
        let newY = parent.mouseXY.y * scaleY

        let newLocation = NSPoint.init(x: newX, y: newY)
        
        mouse?.processMouseEvents(xy: newLocation)
        //track("\(dx) \(dy)\n");
    }
    
    override func mouseDragged(with event: NSEvent) {

        mouseMoved(with: event)
    }
    
    override func rightMouseDragged(with event: NSEvent) {
        
        mouseMoved(with: event)
    }
}
