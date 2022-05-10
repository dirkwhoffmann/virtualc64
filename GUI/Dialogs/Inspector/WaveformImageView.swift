// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

class WaveformImageView: NSImageView {

    @IBOutlet weak var inspector: Inspector!

    var sid: SIDProxy { return inspector.c64.sid }
    var running: Bool { return inspector.c64.running }

    // Waveform size
    // var size = NSSize(width: 400, height: 200)
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
    }

    required override init(frame frameRect: NSRect) {
        super.init(frame: frameRect)
    }

    override func awakeFromNib() {

        let w = inspector.sidWaveformImageView.visibleRect.width
        let h = inspector.sidWaveformImageView.visibleRect.height

        size = NSSize(width: w, height: h)
        buffer = UnsafeMutablePointer<UInt32>.allocate(capacity: wordCount)
    }

    func update() {

        needsDisplay = true
    }

    override func draw(_ dirtyRect: NSRect) {

        var source = -1
        if inspector.sidWaveformSource.selectedTag() == 1 {
            source = inspector.selectedSID
        }

        maxAmp = sid.drawWaveform(buffer, size: size, scale: maxAmp, color: color, source: source)
        image = NSImage.make(data: buffer, rect: CGSize(width: size.width, height: size.height))
        super.draw(dirtyRect)
    }
}
