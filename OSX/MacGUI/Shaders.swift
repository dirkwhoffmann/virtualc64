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

    // var computeEncoder : MTLComputeCommandEncoder
    var uniformBuffer : MTLBuffer!
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

