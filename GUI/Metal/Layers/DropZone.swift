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
 
    var window: NSWindow { return controller.window! }
    var contentView: NSView { return window.contentView! }
    var metal: MetalView { return controller.metal! }
    var mydocument: MyDocument { return controller.mydocument! }
    var mm: MediaManager { return controller.mm }

    var zones = [ NSImageView(), NSImageView(),
                  NSImageView(), NSImageView(),
                  NSImageView() ]

    var ul = [ NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
               NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
               NSPoint(x: 0, y: 0) ]

    var lr = [ NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
               NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
               NSPoint(x: 0, y: 0) ]
    
    static let unconnected = 0.92
    static let unselected = 0.92
    static let selected = 0.92
    
    var hideAll = false
    var enabled = [false, false, false, false, false]
    var inUse = [false, false, false, false, false ]
    var inside = [false, false, false, false, false ]
    var currentAlpha = [0.0, 0.0, 0.0, 0.0, 0.0 ]
    var targetAlpha = [unselected, unselected, unselected, unselected, unselected]
    var maxAlpha = [0.0, 0.0, 0.0, 0.0, 0.0]
    
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {

        for i in 0...4 { zones[i].unregisterDraggedTypes() }
        super.init(renderer: renderer)
        resize()
    }

    private func image(zone: Int) -> NSImage {

        let suffix = enabled[zone] ? (inUse[zone] ? "InUse" : "Empty") : "Disabled"
        return NSImage(named: "dropZone\(zone)\(suffix)")!
    }
    
    private func setType(_ type: vc64.FileType) {
    
        if let emu = emu {

            let info8 = emu.drive8.info
            let config8 = emu.drive8.config

            let info9 = emu.drive9.info
            let config9 = emu.drive9.config

            inUse[0] = info8.hasDisk
            inUse[1] = info9.hasDisk
            inUse[2] = false
            inUse[3] = emu.expansionport.cartridgeAttached()
            inUse[4] = emu.datasette.info.hasTape

            switch type {

            case .T64, .P00, .PRG:
                enabled = [config8.connected, config9.connected, true, false, false]

            case .FOLDER, .D64, .G64:
                enabled = [config8.connected, config9.connected, false, false, false]

            case .CRT:
                enabled = [false, false, false, true, false]

            case .TAP:
                enabled = [false, false, false, false, true]

            default:
                enabled = [false, false, false, false, false]
            }

            for i in 0...4 {

                zones[i].image = image(zone: i)
            }

            // Hide all drop zones if none is enabled
            hideAll = !enabled[0] && !enabled[1] && !enabled[2] && !enabled[3] && !enabled[4]
        }
    }

    func open(type: vc64.FileType, delay: Double) {

        setType(type)
        open(delay: delay)
        resize()
    }
    
    override func update(frames: Int64) {
        
        super.update(frames: frames)
        if alpha.current > 0 { updateAlpha() }
    }
    
    func isInside(_ sender: NSDraggingInfo, zone i: Int) -> Bool {

        assert(i >= 0 && i <= 4)

        let x = sender.draggingLocation.x
        let y = sender.draggingLocation.y

        return x > ul[i].x && x < lr[i].x && y > ul[i].y && y < lr[i].y
    }

    func draggingUpdated(_ sender: NSDraggingInfo) {
                   
        if hideAll { return }
        
        for i in 0...4 {
        
            if !enabled[i] {
                targetAlpha[i] = DropZone.unconnected
                continue
            }
            
            let isIn = isInside(sender, zone: i)

            if isIn && !inside[i] {
                
                inside[i] = true
                zones[i].image = NSImage(named: "dropZone\(i)Selected")
                targetAlpha[i] = DropZone.selected
            }

            if !isIn && inside[i] {

                inside[i] = false
                zones[i].image = image(zone: i)
                targetAlpha[i] = DropZone.unselected
            }
        }
    }
    
    override func alphaDidChange() {
                
        if hideAll { return }
        
        for i in 0...4 {
            
            maxAlpha[i] = Double(alpha.current)
        
            if alpha.current > 0 && zones[i].superview == nil {
                contentView.addSubview(zones[i])
            }
            if alpha.current == 0 && zones[i].superview != nil {
                zones[i].removeFromSuperview()
            }
        }
        
        resize()
    }

    override func layerDidClose() {

        guard let url = metal.dropUrl else { return }
        guard let type = metal.dropType else { return }

        do {

            if let n = metal.dropZone {

                switch n {

                case 0: try mm.addMedia(url: url, allowedTypes: [type], drive: DRIVE8)
                case 1: try mm.addMedia(url: url, allowedTypes: [type], drive: DRIVE9)
                case 2: try mm.addMedia(url: url, allowedTypes: [type], options: [.flash])
                case 3: try mm.addMedia(url: url, allowedTypes: [type])
                case 4: try mm.addMedia(url: url, allowedTypes: [type])

                default:
                    fatalError()
                }

            } else {

                switch type {

                case .SNAPSHOT:

                    try mm.addMedia(url: url, allowedTypes: [type])

                case .SCRIPT:

                    try mm.addMedia(url: url, allowedTypes: [type])
                    mm.console.open()

                default:
                    
                    NSSound.beep()
                }
            }

        } catch {

            controller.showAlert(.cantOpen(url: url), error: error, async: true)
        }
    }

    func updateAlpha() {
            
        for i in 0...4 {

            let current = currentAlpha[i]
            var delta = 0.0

            if current < targetAlpha[i] && current < maxAlpha[i] { delta = 0.05 }
            if current > targetAlpha[i] || current > maxAlpha[i] { delta = -0.05 }

            if delta != 0.0 {
                
                currentAlpha[i] += delta
                zones[i].alphaValue = CGFloat(currentAlpha[i])
            }
        }
    }
    
    func resize() {
                         
        let size = controller.metal.frame.size
        let origin = controller.metal.frame.origin
        let midx = origin.x + (size.width / 2)

        let w = size.width / 6
        let h = w * 1.2
        let y = size.height + origin.y - 24 - h * CGFloat(alpha.current)
        let margin = w / 8

        var x = midx - 2.5 * w - 2 * margin

        for i in 0...4 {

            ul[i] = CGPoint(x: x, y: y)
            lr[i] = CGPoint(x: ul[i].x + w, y: ul[i].y + h)

            zones[i].setFrameSize(NSSize(width: w, height: h))
            zones[i].frame.origin = ul[i]

            x += w + margin
        }
    }
}
