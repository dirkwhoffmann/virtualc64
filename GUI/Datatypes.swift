// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Integer types
//

extension UInt32 {

    init(rgba: (UInt8, UInt8, UInt8, UInt8)) {
        
        let r = UInt32(rgba.0)
        let g = UInt32(rgba.1)
        let b = UInt32(rgba.2)
        let a = UInt32(rgba.3)
        
        self.init(bigEndian: r << 24 | g << 16 | b << 8 | a)
    }

    init(rgba: (UInt8, UInt8, UInt8)) {
        
        self.init(rgba: (rgba.0, rgba.1, rgba.2, 0xFF))
     }
    
    init(r: UInt8, g: UInt8, b: UInt8, a: UInt8) { self.init(rgba: (r, g, b, a)) }
    init(r: UInt8, g: UInt8, b: UInt8) { self.init(rgba: (r, g, b)) }
}

//
// Numeric types
//

extension Comparable {
    
    func clamped(_ f: Self, _ t: Self) -> Self {
        
        var r = self
        if r < f { r = f }
        if r > t { r = t }
        return r
    }
}
