// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MTLTexture {
    
    func replace(region: MTLRegion, buffer: UnsafeMutablePointer<UInt32>?) {
        
        if buffer != nil {
            let bpr = 4 * region.size.width
            replace(region: region, mipmapLevel: 0, withBytes: buffer!, bytesPerRow: bpr)
        }
    }
    
    func replace(size: MTLSize, buffer: UnsafeMutablePointer<UInt32>?) {
        
        let region = MTLRegionMake2D(0, 0, size.width, size.height)
        replace(region: region, buffer: buffer)
    }
    
    func replace(w: Int, h: Int, buffer: UnsafeMutablePointer<UInt32>?) {
        
        let region = MTLRegionMake2D(0, 0, w, h)
        replace(region: region, buffer: buffer)
    }
}
