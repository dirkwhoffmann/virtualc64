//
//  Shaders.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.01.18.
//

import Foundation

//
// Base class for all compute kernels
// 

struct C64_TEXTURE {
    static let width = 512
    static let height = 512
    static let cutout_x = 428
    static let cutout_y = 284
}

struct UPSCALED_TEXTURE {
    static let factor = 4
    static let width = C64_TEXTURE.width * UPSCALED_TEXTURE.factor
    static let height = C64_TEXTURE.height * UPSCALED_TEXTURE.factor
    static let cutout_x = C64_TEXTURE.cutout_x * UPSCALED_TEXTURE.factor
    static let cutout_y = C64_TEXTURE.cutout_y * UPSCALED_TEXTURE.factor
}

class ComputeKernel : NSObject {
    
    var kernel : MTLComputePipelineState!
    var sampler : MTLSamplerState!
    
    var threadgroupSize : MTLSize
    var threadgroupCount : MTLSize

    var samplerLinear : MTLSamplerState!
    var samplerNearest : MTLSamplerState!

    override init()
    {
        // Set thread group size of 16x16
        // TODO: Which thread group size suits best for out purpose?
        let groupSizeX = 16
        let groupSizeY = 16
        threadgroupSize = MTLSizeMake(groupSizeX, groupSizeY, 1 /* depth */)
        
        // Calculate the compute kernel's width and height
        let threadCountX = (UPSCALED_TEXTURE.width + groupSizeX -  1) / groupSizeX
        let threadCountY = (UPSCALED_TEXTURE.height + groupSizeY - 1) / groupSizeY
        threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1)
        
        super.init()
    }

    convenience init(name: String, device: MTLDevice, library: MTLLibrary)
    {
        self.init()
        
        // Lookup kernel function in library
        guard let function = library.makeFunction(name: name) else {
            print("ERROR: Cannot find kernel function '\(name)' in library.")
            abort()
        }
        
        // Create kernel
        do {
            try kernel = device.makeComputePipelineState(function: function)
        }
        catch {
            print("ERROR: Failed to create compute kernel '\(name)' in library.")
            abort()
        }
        
        // Build texture samplers
        let samplerDescriptor1 = MTLSamplerDescriptor()
        samplerDescriptor1.minFilter = MTLSamplerMinMagFilter.linear
        samplerDescriptor1.magFilter = MTLSamplerMinMagFilter.linear
        samplerDescriptor1.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor1.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor1.mipFilter = MTLSamplerMipFilter.notMipmapped
        samplerLinear = device.makeSamplerState(descriptor: samplerDescriptor1)
        
        let samplerDescriptor2 = MTLSamplerDescriptor()
        samplerDescriptor2.minFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor2.magFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor2.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor2.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor2.mipFilter = MTLSamplerMipFilter.notMipmapped
        samplerNearest = device.makeSamplerState(descriptor: samplerDescriptor2)
        
        // Set default sampler
        sampler = samplerLinear
    }
    
    func getsampler() -> MTLSamplerState
    {
        return sampler
    }
    
    func configureComputeCommandEncoder(encoder : MTLComputeCommandEncoder)
    {
        // Each specific compute kernel puts its initialization code here
    }
    
    func apply(commandBuffer: MTLCommandBuffer, source: MTLTexture, target: MTLTexture)
    {
        if let encoder = commandBuffer.makeComputeCommandEncoder() {
            encoder.setComputePipelineState(kernel)
            encoder.setTexture(source, index: 0)
            encoder.setTexture(target, index: 1)

            configureComputeCommandEncoder(encoder: encoder)

            encoder.dispatchThreadgroups(threadgroupCount, threadsPerThreadgroup: threadgroupSize)
            encoder.endEncoding()
        }
    }
}

//
// Upscalers
//

class BypassUpscaler : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "bypassupscaler", device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

class EPXUpscaler : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "epxupscaler", device: device, library: library)
        
        // Replace default texture sampler
          sampler = samplerNearest
    }
}

class XBRUpscaler : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "xbrupscaler", device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

//
// Filters
//

class BypassFilter : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "bypass", device: device, library: library)

        // Replace default texture sampler
        sampler = samplerNearest
    }
}

class BlurFilter : ComputeKernel {
    
    var blurWeightTexture: MTLTexture!
    
    convenience init(device: MTLDevice, library: MTLLibrary, radius: Float) {
        self.init(name: "blur", device: device, library: library)
    
        // Build blur weight texture
        let sigma: Float = radius / 2.0
        let size:  Int   = Int(round(radius) * 2 + 1)
    
        var delta: Float = 0.0;
        var expScale: Float = 0.0
        
        if (radius > 0.0) {
            delta = (radius * 2) / Float(size - 1)
            expScale = -1.0 / (2 * sigma * sigma)
        }
    
        let weights = UnsafeMutablePointer<Float>.allocate(capacity: size * size)
        
        var weightSum: Float = 0.0;
        var y: Float = -radius;
        
        for j in 0 ..< size {
            var x = -radius
            for i in 0 ..< size {
                let weight = expf((x * x + y * y) * expScale)
                weights[j * size + i] = weight
                weightSum += weight
                x += delta
            }
            y += delta
        }
        
        let weightScale: Float = 1.0 / weightSum
        for j in 0 ..< size {
            for i in 0 ..< size {
                weights[j * size + i] *= weightScale;
            }
        }
    
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: MTLPixelFormat.r32Float, width: size, height: size, mipmapped: false)
    
        blurWeightTexture = device.makeTexture(descriptor: textureDescriptor)!
        
        let region = MTLRegionMake2D(0, 0, size, size)
        blurWeightTexture.replace(region: region, mipmapLevel: 0, withBytes: weights, bytesPerRow: size * 4 /* size of float */)
    
        weights.deallocate()
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder) {
        encoder.setTexture(blurWeightTexture, index: 2)
    }
}


class SaturationFilter : ComputeKernel {

    var uniformBuffer : MTLBuffer!
    
    convenience init(device: MTLDevice, library: MTLLibrary, factor: Float) {
        self.init(name: "saturation", device: device, library: library)
        
        // Setup uniform buffer
        let floatSize = MemoryLayout<Float>.size
        let uniformsStructSize = floatSize
        uniformBuffer = device.makeBuffer(length: uniformsStructSize)
        let bufferPointer = uniformBuffer?.contents()
        var value = factor
        memcpy(bufferPointer, &value, floatSize)
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder) {
        encoder.setBuffer(uniformBuffer, offset: 0, index: 0)
    }
}

class SepiaFilter : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "sepia", device: device, library: library)
    }
}

class CrtFilter : ComputeKernel {
    
    convenience init(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "crt", device: device, library: library)
    }
}


