// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class WaveformView: NSView {
 
    @IBOutlet weak var inspector: Inspector!
    
    var sid: SIDProxy { return inspector.c64.sid }
    var running: Bool { return inspector.c64.isRunning }
    
    // Remembers the highest amplitude (used for auto scaling)
    var highestAmplitude: Float = 0.001
    
    // Modulo counter to trigger image rendering
    var delayCounter = 0
    
    required init?(coder decoder: NSCoder) {
        super.init(coder: decoder)
    }
 
    required override init(frame frameRect: NSRect) {
        super.init(frame: frameRect)
    }
  
    // Restarts the auto scaling mechanism
    func initAutoScaler() {
        highestAmplitude = 0.001
    }
    
    func update() {
        needsDisplay = true
    }
    
    override func draw(_ dirtyRect: NSRect) {
        
        super.draw(dirtyRect)
        
        let context = NSGraphicsContext.current?.cgContext
        
        NSColor.clear.set()
        context?.fill(dirtyRect)
        
        let w = Int(frame.width)
        let baseline = Float(frame.height / 2)
        let normalizer = highestAmplitude
        highestAmplitude = 0.001

        var l = Float(0)
        var r = Float(0)
        
        for x in 0...w {
            
            sid.ringbufferData(40 * x, left: &l, right: &r)
            
            // Scale samples and determine the highest amplitude
            let absl = abs(l), absr = abs(r)
            var scaledl = absl / normalizer * baseline
            var scaledr = absr / normalizer * baseline
            highestAmplitude = max(highestAmplitude, max(absl, absr))

            // Apply some eye candy (artifical noise)
            if scaledl == 0 { scaledl = (running && drand48() > 0.5) ? 1.0 : 0.0 }
            if scaledr == 0 { scaledr = (running && drand48() > 0.5) ? 1.0 : 0.0 }

            // Draw bars
            let from = CGPoint(x: x, y: Int(baseline + scaledl + 1))
            let to = CGPoint(x: x, y: Int(baseline - scaledr))
            context?.move(to: from)
            context?.addLine(to: to)
        }
        NSColor.gray.setStroke()
        context?.strokePath()
    }
}
