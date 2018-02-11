/*
 * (C) 2018 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import Foundation

class Speedometer : NSObject {
    
    /// Current emulation speed in Mhz
    private var mhz = 0.0
    
    /// Current GPU performance in frames per second
    private var fps = 0.0
    
    /// Smooth factor
    private let alpha = 0.6

    /// Stores when updateWithCurrentCycle was called the last time
    private var latchedTimestamp: Double
    
    //! Cycle count in previous call to updateWithCurrentCycle
    private var latchedCycle: UInt64 = UInt64.max
    
    //! Previous frame count in previous call to updateWithCurrentCycle
    private var latchedFrame: UInt64 = UInt64.max
    
    override init() {

        latchedTimestamp = Date().timeIntervalSince1970
        super.init()
    }

    func mhz(digits: Int) -> Double {
        let factor = Double(truncating:pow(10,digits) as NSNumber)
        return round(factor * mhz) / factor
    }
    
    func fps(digits: Int) -> Double {
        let factor = Double(truncating:pow(10,digits) as NSNumber)
        return round(factor * fps) / factor
    }
    
    /// Updates speed, frame and jitter information.
    /// This function needs to be invoked before reading mhz and fps
    /// -param cycles Current cycle count (processed cycles since emulator power up).
    /// -param frame Current frame (processed frames since emulator power up).
    func updateWith(cycle: UInt64, frame: UInt64) {
        
        let timestamp = Date().timeIntervalSince1970
        
        if cycle >= latchedCycle && frame >= latchedFrame {

            // Measure elapsed time in microseconds
            let elapsedTime = timestamp - latchedTimestamp
        
            // Measure clock frequency in MHz
            let elapsedCycles = Double(cycle - latchedCycle) / 1_000_000
            mhz = alpha * (elapsedCycles / elapsedTime) + (1 - alpha) * mhz

            // Measure frames per second
            let elapsedFrames = Double(frame - latchedFrame)
            fps = alpha * (elapsedFrames / elapsedTime) + (1 - alpha) * fps

            // Assign zero if values are completely out of range
            // mhz = (mhz >= 0.0 && mhz <= 100.0) ? mhz : 0.0
            // fps = (fps >= 0.0 && fps <= 200.0) ? fps : 0.
        }
        
        // Keep values
        latchedTimestamp = timestamp
        latchedCycle = cycle
        latchedFrame = frame
    }
}
