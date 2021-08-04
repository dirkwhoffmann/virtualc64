// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

class DropZone: Layer {
 
    let controller: MyController
    
    var window: NSWindow { return controller.window! }
    var contentView: NSView { return window.contentView! }
    
    let drive8 = NSImageView()
    let drive9 = NSImageView()

    var ul = [NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0)]
    var lr = [NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0)]
    
    static let unconnected = 0.15
    static let unselected = 0.6
    static let selected = 1.0
    
    var currentAlpha = [0.0, 0.0]
    var targetAlpha = [unselected, unselected]
    var maxAlpha = [0.0, 0.0]
        
    var isDirty = false
        
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {
        
        controller = renderer.parent
        
        drive8.image = NSImage.init(named: "dropDrive8")
        drive8.unregisterDraggedTypes()

        drive9.image = NSImage.init(named: "dropDrive9")
        drive9.unregisterDraggedTypes()

        super.init(renderer: renderer)
        
        resize()
        isDirty = true
    }

    override func open() {
    
        track()        
        super.open()
        resize()
    }

    override func update(frames: Int64) {
        
        super.update(frames: frames)

        if alpha.current > 0 {
            updateAlpha()
        }
        
        if isDirty {
            
            isDirty = false
        }
    }
    
    func isInside(_ sender: NSDraggingInfo, zone i: Int) -> Bool {

        assert(i >= 0 && i <= 1)

        let x = sender.draggingLocation.x
        let y = sender.draggingLocation.y

        return x > ul[i].x && x < lr[i].x && y > ul[i].y && y < lr[i].y
    }
    
    func draggingUpdated(_ sender: NSDraggingInfo) {
                
        var connected = false
        
        for i in 0...1 {
        
            if i == 0 { connected = renderer.parent.c64.drive8.isConnected() }
            if i == 1 { connected = renderer.parent.c64.drive9.isConnected() }
            
            if !connected {
                targetAlpha[i] = DropZone.unconnected
            } else if isInside(sender, zone: i) {
                targetAlpha[i] = DropZone.selected
            } else {
                targetAlpha[i] = DropZone.unselected
            }
        }
    }
    
    override func alphaDidChange() {
                
        maxAlpha[0] = Double(alpha.current)
        maxAlpha[1] = Double(alpha.current)
        
        if alpha.current > 0 && drive8.superview == nil {
            contentView.addSubview(drive8)
        }
        if alpha.current > 0 && drive9.superview == nil {
            contentView.addSubview(drive9)
        }
        if alpha.current == 0 && drive8.superview != nil {
            drive8.removeFromSuperview()
        }
        if alpha.current == 0 && drive9.superview != nil {
            drive9.removeFromSuperview()
        }
    }
    
    func updateAlpha() {
            
        for i in 0...1 {
    
            var delta = 0.0

            if currentAlpha[i] < targetAlpha[i] && currentAlpha[i] < maxAlpha[i] {
                delta = 0.05
            }
            if currentAlpha[i] > targetAlpha[i] || currentAlpha[i] > maxAlpha[i] {
                delta = -0.05
            }
            
            if delta != 0.0 {
                currentAlpha[i] += delta
                if i == 0 { drive8.alphaValue = CGFloat(currentAlpha[i]) }
                if i == 1 { drive9.alphaValue = CGFloat(currentAlpha[i]) }
            }
        }
    }
    
    func resize() {
                          
        let size = controller.metal.frame.size
        let origin = controller.metal.frame.origin
        let midx = origin.x + (size.width / 2)
        let midy = origin.y + (size.height / 2)

        let w = size.width / 4
        let h = w
        let margin = w / 8
        let iconSize = NSSize.init(width: w, height: h)
        
        ul[0] = CGPoint.init(x: midx - w - margin, y: midy - (h / 2))
        lr[0] = CGPoint.init(x: ul[0].x + w, y: ul[0].y + h)
        drive8.frame.origin = ul[0]
        drive8.setFrameSize(iconSize)

        ul[1] = CGPoint.init(x: midx + margin, y: midy - (h / 2))
        lr[1] = CGPoint.init(x: ul[1].x + w, y: ul[1].y + h)
        drive9.frame.origin = ul[1]
        drive9.setFrameSize(iconSize)
    }
}
