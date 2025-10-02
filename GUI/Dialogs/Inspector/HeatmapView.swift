// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

class HeatmapView: NSImageView {

    @IBOutlet weak var inspector: Inspector!

    var mem: MemoryProxy? { return inspector.emu?.mem }

    // View size
    var size: NSSize!
    var wordCount: Int { return Int(size.width) * Int(size.height) }

    // Data buffer
    var buffer: UnsafeMutablePointer<UInt32>!

    required init?(coder decoder: NSCoder) {

        super.init(coder: decoder)
    }

    required override init(frame frameRect: NSRect) {

        super.init(frame: frameRect)
    }

    override func awakeFromNib() {

        let w = 256
        let h = 256
        size = NSSize(width: w, height: h)
        buffer = UnsafeMutablePointer<UInt32>.allocate(capacity: wordCount)
    }

    func update() {

        needsDisplay = true
    }

    override func draw(_ dirtyRect: NSRect) {

        mem?.drawHeatmap(buffer, w: Int(size.width), h: Int(size.height))
        image = NSImage.make(data: buffer, rect: CGSize(width: size.width, height: size.height))
        super.draw(dirtyRect)
    }
}
