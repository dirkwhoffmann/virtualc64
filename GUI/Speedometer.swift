// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Speedometer: NSObject {
    
    // Current emulation speed in MHz
    private var _mhz = 0.0
    var mhz: Double { return _mhz.truncate(digits: 2); }
    
    // Current GPU performance in frames per second
    private var _fps = 0.0
    var fps: Double { return _fps.truncate(digits: 2); }
    
    // Smoothing factor
    private let alpha = 0.6
    
    // Time of the previous update
    private var latchedTimestamp: Double
    
    // Value of the master clock in the previous update
    private var latchedCycle: Int64 = Int64.max
    
    // Frame count in the previous update
    private var latchedFrame: Int64 = Int64.max
    
    override init() {

        latchedTimestamp = Date().timeIntervalSince1970
    }
    
    /* Updates speed, frame and jitter information. 'cycles' is the number of
     * elapsed cycles since power up. 'frames' is the number of computed frames
     * since power up.
     */
    func updateWith(cycle: Int64, frame: Int64) {
        
        let timestamp = Date().timeIntervalSince1970
        
        if cycle >= latchedCycle && frame >= latchedFrame {
            
            // Measure elapsed time in microseconds
            let elapsedTime = timestamp - latchedTimestamp
            
            // Measure clock frequency in MHz
            let elapsedCycles = Double(cycle - latchedCycle) / 1_000_000
            _mhz = alpha * (elapsedCycles / elapsedTime) + (1 - alpha) * _mhz
            
            // Measure frames per second
            let elapsedFrames = Double(frame - latchedFrame)
            _fps = alpha * (elapsedFrames / elapsedTime) + (1 - alpha) * _fps            
        }
        
        // Keep values
        latchedTimestamp = timestamp
        latchedCycle = cycle
        latchedFrame = frame
    }
}
