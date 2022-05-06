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
    var metal: MetalView { return controller.metal! }
    var mydocument: MyDocument { return controller.mydocument! }
        
    var zones = [NSImageView(), NSImageView(), NSImageView(), NSImageView()]
    var ul = [NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
              NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0)]
    var lr = [NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0),
              NSPoint(x: 0, y: 0), NSPoint(x: 0, y: 0)]
    
    static let unconnected = 0.92
    static let unselected = 0.92
    static let selected = 0.92
    
    var hideAll = false
    var enabled = [false, false, false, false]
    var inUse = [false, false, false, false]
    var inside = [false, false, false, false]
    var currentAlpha = [0.0, 0.0, 0.0, 0.0]
    var targetAlpha = [unselected, unselected, unselected, unselected]
    var maxAlpha = [0.0, 0.0, 0.0, 0.0]
    
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {
        
        controller = renderer.parent
        
        for i in 0...3 { zones[i].unregisterDraggedTypes() }
        super.init(renderer: renderer)
        resize()
    }

    private func image(zone: Int) -> NSImage {

        var suffix = "Disabled"

        if enabled[zone] {
            
            switch zone {
            case 0: suffix = c64.drive8.hasDisk ? "InUse" : "Empty"
            case 1: suffix = c64.drive9.hasDisk ? "InUse" : "Empty"
            case 2: suffix = c64.expansionport.cartridgeAttached() ? "InUse" : "Empty"
            case 3: suffix = c64.datasette.hasTape ? "InUse" : "Empty"
            default: fatalError()
            }
        }
        return NSImage(named: "dropZone\(zone)\(suffix)")!
    }
    
    private func setType(_ type: FileType) {
    
        let connected8 = c64.drive8.isConnected()
        let connected9 = c64.drive9.isConnected()

        inUse[0] = c64.drive8.hasDisk
        inUse[1] = c64.drive9.hasDisk
        inUse[2] = c64.expansionport.cartridgeAttached()
        inUse[3] = c64.datasette.hasTape
        
        switch type {
        
        case .T64, .P00, .PRG, .FOLDER, .D64, .G64:
            enabled = [connected8, connected9, false, false]

        case .CRT:
            enabled = [false, false, true, false]
            
        case .TAP:
            enabled = [false, false, false, true]
            
        default:
            enabled = [false, false, false, false]
        }
        
        for i in 0...3 {
            
            zones[i].image = image(zone: i)
        }
        
        // Hide all drop zones if none is enabled
        hideAll = !enabled[0] && !enabled[1] && !enabled[2] && !enabled[3]
    }

    func open(type: FileType, delay: Double) {

        setType(type)
        open(delay: delay)
        resize()
    }
    
    override func update(frames: Int64) {
        
        super.update(frames: frames)
        if alpha.current > 0 { updateAlpha() }
    }
    
    func isInside(_ sender: NSDraggingInfo, zone i: Int) -> Bool {

        assert(i >= 0 && i <= 3)
        
        // if !enabled[i] { return false }
        
        let x = sender.draggingLocation.x
        let y = sender.draggingLocation.y

        return x > ul[i].x && x < lr[i].x && y > ul[i].y && y < lr[i].y
    }

    func draggingUpdated(_ sender: NSDraggingInfo) {
                   
        if hideAll { return }
        
        for i in 0...3 {
        
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
        
        for i in 0...3 {
            
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

                switch type {

                case .SNAPSHOT, .SCRIPT:

                    try mydocument.addMedia(url: url,
                                            allowedTypes: [type])

                case .CRT, .T64, .P00, .PRG, .FOLDER, .D64, .G64, .TAP:

                    let drive = n == 0 ? DRIVE8 : DRIVE9
                    try mydocument.addMedia(url: url,
                                            allowedTypes: [type], drive: drive)

                default:
                    fatalError()
                }

            } else {
                
                if let file = try mydocument.createFileProxy(from: url,
                                                             allowedTypes: FileType.draggable) {

                    log()

                    // Run the import dialog
                    let importer = ImportDialog(with: controller, nibName: "ImportDialog")
                    importer?.show(proxy: file)
                }
            }

        } catch {

            controller.showAlert(.cantOpen(url: url), error: error, async: true)
        }
    }

    func updateAlpha() {
            
        for i in 0...3 {

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
        let iconSize = NSSize(width: w, height: h)
            
        ul[0] = CGPoint(x: midx - 2 * w - 1.5 * margin, y: y)
        lr[0] = CGPoint(x: ul[0].x + w, y: ul[0].y + h)

        ul[1] = CGPoint(x: midx - w - 0.5 * margin, y: y)
        lr[1] = CGPoint(x: ul[1].x + w, y: ul[1].y + h)

        ul[2] = CGPoint(x: midx + 0.5 * margin, y: y)
        lr[2] = CGPoint(x: ul[2].x + w, y: ul[2].y + h)

        ul[3] = CGPoint(x: midx + w + 1.5 * margin, y: y)
        lr[3] = CGPoint(x: ul[3].x + w, y: ul[3].y + h)

        for i in 0...3 {

            zones[i].setFrameSize(iconSize)
            zones[i].frame.origin = ul[i]
        }
    }
}
