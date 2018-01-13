//
//  Shaders.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.01.18.
//

import Foundation

// --------------------------------------------------------------------------------------------
//                       Base class for all compute kernels
// --------------------------------------------------------------------------------------------


@objc class ComputeKernel : NSObject {

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
        let threadCountX = (2048 /* texture width */ + groupSizeX -  1) / groupSizeX
        let threadCountY = (2048 /* texture height */ + groupSizeY - 1) / groupSizeY
        threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1)
        
        super.init()
    }

    @objc convenience init(name: String, device: MTLDevice, library: MTLLibrary)
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
    
    @objc func getsampler() -> MTLSamplerState
    {
        return sampler
    }
    
    @objc func configureComputeCommandEncoder(encoder : MTLComputeCommandEncoder)
    {
        // Each specific compute kernel puts its initialization code here
    }
    
    @objc func apply(commandBuffer: MTLCommandBuffer, source: MTLTexture, target: MTLTexture)
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

// --------------------------------------------------------------------------------------------
//                                    Upscalers
// --------------------------------------------------------------------------------------------

@objc class BypassUpscaler : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "bypassupscaler", device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

@objc class EPXUpscaler : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "epxupscaler", device: device, library: library)
        
        // Replace default texture sampler
          sampler = samplerNearest
    }
}

@objc class XBRUpscaler : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "xbrupscaler", device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

// --------------------------------------------------------------------------------------------
//                                     Filters
// --------------------------------------------------------------------------------------------

@objc class BypassFilter : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "bypass", device: device, library: library)

        // Replace default texture sampler
        sampler = samplerNearest
    }
}

@objc class BlurFilter : ComputeKernel {
    
    var blurWeightTexture: MTLTexture!
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary, radius: Float)
    {
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
    
        weights.deallocate(capacity: size * size)
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder)
    {
        encoder.setTexture(blurWeightTexture, index: 2)
    }
}


@objc class SaturationFilter : ComputeKernel {

    var uniformBuffer : MTLBuffer!
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary, factor: Float)
    {
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

@objc class SepiaFilter : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "sepia", device: device, library: library)
    }
}

@objc class CrtFilter : ComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "crt", device: device, library: library)
    }
}


