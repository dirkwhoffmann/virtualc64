// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Speedometer {
    
    // Emulator performance in MHz
    private var _mhz = 0.0
    var mhz: Double { return _mhz.truncate(digits: 2) }

    // Emulator performance in frames per second
    private var _emuFps = 0.0
    var emuFps: Double { return _emuFps.truncate(digits: 0) }

    // GPU performance in frames per second
    private var _gpuFps = 0.0
    var gpsFps: Double { return _gpuFps.truncate(digits: 0) }

    // Latched values from the previous update
    private var latchedTimestamp = Date().timeIntervalSince1970
    private var latchedCycle = Int64.max
    private var latchedEmuFrame = Int64.max
    private var latchedGpuFrame = Int64.max

    // Update counter
    var counter = 0

    init() {

        latchedTimestamp = Date().timeIntervalSince1970
    }
    
    /* Updates speed, frame and jitter information. 'cycles' is the number of
     * elapsed cycles since power up. 'xxxFrames' is the number of computed
     * frames since power up.
     */
    func updateWith(cycle: Int64, emuFrame: Int64, gpuFrame: Int64) {

        let timestamp = Date().timeIntervalSince1970
        counter += 1

        // Measure elapsed time in microseconds
        let elapsedTime = timestamp - latchedTimestamp

        if cycle >= latchedCycle {

            // Measure clock frequency
            let elapsedCycles = Double(cycle - latchedCycle) / 1_000_000
            let alpha = 0.5
            _mhz = alpha * (elapsedCycles / elapsedTime) + (1 - alpha) * _mhz
        }

        if emuFrame >= latchedEmuFrame {

            // Use a larger measurement interval
            if counter % 2 == 0 { return }

            // Measure emulator frames per second
            let elapsedFrames = Double(emuFrame - latchedEmuFrame)
            let alpha = 0.9
            _emuFps = alpha * (elapsedFrames / elapsedTime) + (1 - alpha) * _emuFps
        }

        if gpuFrame >= latchedGpuFrame {

            // Use a larger measurement interval
            if counter % 2 == 0 { return }

            // Measure GPU frames per second
            let elapsedFrames = Double(gpuFrame - latchedGpuFrame)
            let alpha = 0.9
            _gpuFps = alpha * (elapsedFrames / elapsedTime) + (1 - alpha) * _gpuFps
        }

        // Keep values
        latchedTimestamp = timestamp
        latchedCycle = cycle
        latchedEmuFrame = emuFrame
        latchedGpuFrame = gpuFrame
    }
}
