// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class WaveformPanel: NSImageView {

    var audioPort: AudioPortProxy?
    var sid: SIDProxy?
    
    // Waveform source
    var source = -1
    
    // Waveform size
    var size: NSSize!
    var wordCount: Int { return Int(size.width) * Int(size.height) }

    // Image buffer
    var buffer: UnsafeMutablePointer<UInt32>!

    // Remembers the highest amplitude (used for auto scaling)
    var maxAmp: Float = 0.001
    
    // Foreground color
    var color = UInt32(NSColor.gray.usingColorSpace(.sRGB)!.gpuColor)
    
    required init?(coder decoder: NSCoder) {

        super.init(coder: decoder)
        commonInit()
    }

    required override init(frame frameRect: NSRect) {
        
        super.init(frame: frameRect)
        commonInit()
    }
    
    convenience init(frame frameRect: NSRect, channel: Int) {

        self.init(frame: frameRect)
        self.tag = channel
    }

    func commonInit() {
                        
        size = NSSize(width: 300, height: 100)
        buffer = UnsafeMutablePointer<UInt32>.allocate(capacity: wordCount)
        imageScaling = .scaleAxesIndependently
        
    }

    func update() {

        needsDisplay = true
    }

    override var intrinsicContentSize: NSSize {
        
        // Let the image scale with the ImageView
        return .zero
    }
    
    override func draw(_ dirtyRect: NSRect) {

        maxAmp = sid?.drawWaveform(buffer, size: size, scale: maxAmp, color: color, source: source) ?? 0
        image = NSImage.make(data: buffer, rect: CGSize(width: size.width, height: size.height))
        super.draw(dirtyRect)
    }
}
