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


@objc class SwiftComputeKernel : NSObject {

    var kernel : MTLComputePipelineState!
    var sampler : MTLSamplerState!
    
    var threadgroupSize : MTLSize
    var threadgroupCount : MTLSize

    override init()
    {
        // Set thread group size of 16x16
        // TODO: Which thread group size suits best for out purpose?
        let groupSizeX = 16
        let groupSizeY = 16
        threadgroupSize = MTLSizeMake(groupSizeX, groupSizeY, 1 /* depth */)
        
        // Calculate the compute kernel's width and height
        let threadCountX = (1024 /* texture width */ + groupSizeX -  1) / groupSizeX
        let threadCountY = (1024 /* texture height */ + groupSizeY - 1) / groupSizeY
        threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1)
        print("threadCountX = \(threadCountX)");
        
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
        
        // Build default texture sampler
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = MTLSamplerMinMagFilter.linear
        samplerDescriptor.magFilter = MTLSamplerMinMagFilter.linear
        samplerDescriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.mipFilter = MTLSamplerMipFilter.notMipmapped
        sampler = device.makeSamplerState(descriptor: samplerDescriptor)!
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

@objc class SwiftBypassUpscaler : SwiftComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "bypassupscaler", device: device, library: library)
        
        // Replace default texture sampler
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.mipFilter = MTLSamplerMipFilter.notMipmapped
        sampler = device.makeSamplerState(descriptor: samplerDescriptor)!
    }
}

@objc class SwiftEPXUpscaler : SwiftComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "epxupscaler", device: device, library: library)
        
        // Replace default texture sampler
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.mipFilter = MTLSamplerMipFilter.notMipmapped
        sampler = device.makeSamplerState(descriptor: samplerDescriptor)!
    }
}


// --------------------------------------------------------------------------------------------
//                                     Filters
// --------------------------------------------------------------------------------------------

@objc class SwiftBypassFilter : SwiftComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "bypass", device: device, library: library)
        
        // Replace default texture sampler
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerDescriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        samplerDescriptor.mipFilter = MTLSamplerMipFilter.notMipmapped
        sampler = device.makeSamplerState(descriptor: samplerDescriptor)!
    }
}

@objc class SwiftBlurFilter : SwiftComputeKernel {
    
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


@objc class SwiftSaturationFilter : SwiftComputeKernel {

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

@objc class SwiftSepiaFilter : SwiftComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "sepia", device: device, library: library)
    }
}

@objc class SwiftCrtFilter : SwiftComputeKernel {
    
    @objc convenience init(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "crt", device: device, library: library)
    }
}


