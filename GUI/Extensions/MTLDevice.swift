// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
    
    func makeTexture(size: MTLSize,
                     gradient: [ (Int, Int, Int, Int) ],
                     usage: MTLTextureUsage = [.shaderRead]) -> MTLTexture? {
        
        let capacity = size.width * size.height
        let buffer = UnsafeMutablePointer<UInt32>.allocate(capacity: capacity)
        buffer.drawGradient(size: size, gradient: gradient)
        
        return makeTexture(size: size, buffer: buffer, usage: usage)
    }
}
