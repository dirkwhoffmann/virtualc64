// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Extensions to UInt32
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
// Extensions to MTLTexture
//

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

//
// Extensions to MTLDevice
//

extension MTLDevice {
    
    func makeTexture(size: MTLSize,
                     buffer: UnsafeMutablePointer<UInt32>? = nil,
                     usage: MTLTextureUsage = [.shaderRead]) -> MTLTexture? {
        
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: size.width,
            height: size.height,
            mipmapped: false)
        descriptor.usage = usage
        
        let texture = makeTexture(descriptor: descriptor)
        texture?.replace(size: size, buffer: buffer)
        return texture
    }
    
    func makeTexture(w: Int, h: Int,
                     buffer: UnsafeMutablePointer<UInt32>? = nil,
                     usage: MTLTextureUsage = [.shaderRead]) -> MTLTexture? {
        
        let size = MTLSizeMake(w, h, 0)
        return makeTexture(size: size, buffer: buffer, usage: usage)
    }
}
