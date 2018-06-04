//
//  WaveformView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 23.05.18.
//

import Foundation

class WaveformView: NSView {
 
    @IBOutlet weak var controller: MyController!
    
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
        // track()
        highestAmplitude = 0.001
    }
    
    func update() {
        /*
        delayCounter += 1
        if delayCounter % 2 == 0 {
            return
        }
        */
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
        
        for x in 0...w {
            let sample = controller!.c64.sid.ringbufferData(40 * x)
            let absvalue = abs(sample)
            highestAmplitude = (absvalue > highestAmplitude) ? absvalue : highestAmplitude
            var scaledSample = absvalue / normalizer * baseline
            if (scaledSample == 0) { // just for effect
                scaledSample = drand48() > 0.5 ? 0.0 : 1.0
            }
            let from = CGPoint(x: x, y: Int(baseline + scaledSample + 1))
            let to = CGPoint(x: x, y: Int(baseline - scaledSample))
            context?.move(to: from)
            context?.addLine(to: to)
        }
        NSColor.gray.setStroke()
        context?.strokePath()
    }
}
