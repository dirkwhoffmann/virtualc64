// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

//
// RGBA buffers
//

extension UnsafeMutablePointer where Pointee == UInt32 {
    
    func drawGradient(size: MTLSize, region: MTLRegion,
                      rgba1: (Int, Int, Int, Int), rgba2: (Int, Int, Int, Int)) {
        
        let w = region.size.width
        let h = region.size.height
        
        // Compute delta steps
        let dr = Double(rgba2.0 - rgba1.0) / Double(h); var r = Double(rgba1.0)
        let dg = Double(rgba2.1 - rgba1.1) / Double(h); var g = Double(rgba1.1)
        let db = Double(rgba2.2 - rgba1.2) / Double(h); var b = Double(rgba1.2)
        let da = Double(rgba2.3 - rgba1.3) / Double(h); var a = Double(rgba1.3)
        
        // Create gradient
        var index = size.width * region.origin.y + region.origin.x
        let skip = Int(size.width) - w
        assert(skip >= 0)
        
        for _ in 0 ..< h {
            let c = UInt32(a) << 24 | UInt32(b) << 16 | UInt32(g) << 8 | UInt32(r)
            for _ in 0 ..< w {
                self[index] = c
                index += 1
            }
            r += dr; g += dg; b += db; a += da
            index += skip
        }
    }
    
    func drawGradient(size: MTLSize, region: MTLRegion, gradient: [ (Int, Int, Int, Int) ]) {
        
        let h = region.size.height / (gradient.count - 1)
        var r = region; r.size.height = h
        
        for i in 0 ..< gradient.count - 1 {
            drawGradient(size: size, region: r, rgba1: gradient[i], rgba2: gradient[i+1])
            r.origin.y += h
        }
    }
    
    func drawGradient(size: MTLSize, gradient: [ (Int, Int, Int, Int) ]) {
        
        let region = MTLRegionMake2D(0, 0, size.width, size.height)
        drawGradient(size: size, region: region, gradient: gradient)
    }
}
