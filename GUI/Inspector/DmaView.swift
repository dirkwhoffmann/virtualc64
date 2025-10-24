// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

class DmaView: NSImageView {

    @IBOutlet weak var inspector: Inspector!

    var sid: SIDProxy? { return inspector.emu?.sid }
    var videoPort: VideoPortProxy? { return inspector.emu?.videoPort }
    
    // Canvas size
    var size: NSSize!
    var wordCount: Int { return Int(size.width) * Int(size.height) }

    // Waveform buffer
    var buffer: UnsafeMutablePointer<UInt32>!

    // Remembers the highest amplitude (used for auto scaling)
    var maxAmp: Float = 0.001

    // Foreground color
    let color = UInt32(NSColor.gray.usingColorSpace(.sRGB)!.gpuColor)

    required init?(coder decoder: NSCoder) {

        super.init(coder: decoder)
        commonInit()
    }

    required override init(frame frameRect: NSRect) {
        
        super.init(frame: frameRect)
        commonInit()
    }

    func commonInit() {

        let w = vc64.Tex.width
        let h = vc64.Tex.height

        size = NSSize(width: w, height: h)
        buffer = UnsafeMutablePointer<UInt32>.allocate(capacity: wordCount)
    }

    func update() {

        needsDisplay = true
    }

    override func draw(_ dirtyRect: NSRect) {

        var texture: UnsafePointer<UInt32>!
        var nr = 0

        if let videoPort = videoPort {

            videoPort.dmaTexture(&texture, nr: &nr)
            buffer.update(from: texture, count: wordCount)
        }
        /*
        if let texture = videoPort?.oldDmaTexture {

            buffer.update(from: texture, count: wordCount)
        }
        */

        // maxAmp = sid?.drawWaveform(buffer, size: size, scale: maxAmp, color: color, source: source) ?? 0
        image = NSImage.make(data: buffer, rect: CGSize(width: size.width, height: size.height))
        super.draw(dirtyRect)
    }
}
