// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MTLTexture {

    var size: MTLSize {

        return MTLSize(width: width, height: height, depth: 1)
    }

    func replace(region: MTLRegion, buffer: UnsafePointer<UInt32>?) {
        
        if buffer != nil {
            let bpr = 4 * region.size.width
            replace(region: region, mipmapLevel: 0, withBytes: buffer!, bytesPerRow: bpr)
        }
    }
    
    func replace(size: MTLSize, buffer: UnsafePointer<UInt32>?) {
        
        let region = MTLRegionMake2D(0, 0, size.width, size.height)
        replace(region: region, buffer: buffer)
    }
    
    func replace(w: Int, h: Int, buffer: UnsafePointer<UInt32>?) {
        
        let region = MTLRegionMake2D(0, 0, w, h)
        replace(region: region, buffer: buffer)
    }

    func blit() {

        // Use the blitter to copy the texture data back from the GPU
        let queue = device.makeCommandQueue()!
        let commandBuffer = queue.makeCommandBuffer()!
        let blitEncoder = commandBuffer.makeBlitCommandEncoder()!
        blitEncoder.synchronize(texture: self, slice: 0, level: 0)
        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
    }

    func blitTextureToBuffer(device: MTLDevice, texture: MTLTexture) -> MTLBuffer? {

        let textureWidth = texture.width
        let textureHeight = texture.height
        let pixelFormat = texture.pixelFormat

        // Ensure the pixel format is compatible
        guard pixelFormat == .rgba8Unorm else {
            print("Unsupported pixel format: \(pixelFormat)")
            return nil
        }

        let bytesPerPixel = 4
        let bytesPerRow = textureWidth * bytesPerPixel
        let bufferSize = bytesPerRow * textureHeight

        // Create a buffer on the GPU
        guard let buffer = device.makeBuffer(length: bufferSize, options: .storageModeShared) else {
            print("Failed to create buffer")
            return nil
        }

        // Create a command queue and command buffer
        guard let commandQueue = device.makeCommandQueue(),
              let commandBuffer = commandQueue.makeCommandBuffer(),
              let blitEncoder = commandBuffer.makeBlitCommandEncoder() else {
            print("Failed to create Metal objects")
            return nil
        }

        // Copy texture data into the buffer
        blitEncoder.copy(from: texture,
                         sourceSlice: 0,
                         sourceLevel: 0,
                         sourceOrigin: MTLOrigin(x: 0, y: 0, z: 0),
                         sourceSize: MTLSize(width: textureWidth, height: textureHeight, depth: 1),
                         to: buffer,
                         destinationOffset: 0,
                         destinationBytesPerRow: bytesPerRow,
                         destinationBytesPerImage: bytesPerRow * textureHeight)

        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()

        return buffer
    }
}
