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

class ComputeKernel : NSObject {
    
    var kernel : MTLComputePipelineState!
    var sampler : MTLSamplerState!
    
    var threadgroupSize : MTLSize
    var threadgroupCount : MTLSize

    var samplerLinear : MTLSamplerState!
    var samplerNearest : MTLSamplerState!
    var blurTexture: MTLTexture!
    
    func isBlurRequired() -> Bool {
        return false;
    }
    
    func setBlurTexture(texture: MTLTexture) {
        self.blurTexture = texture;
    }

    init(width: Int, height: Int)
    {
        // Set thread group size of 16x16
        // TODO: Which thread group size suits best for out purpose?
        let groupSizeX = 16
        let groupSizeY = 16
        threadgroupSize = MTLSizeMake(groupSizeX, groupSizeY, 1 /* depth */)
        
        // Calculate the compute kernel's width and height
        let threadCountX = (width + groupSizeX -  1) / groupSizeX
        let threadCountY = (height + groupSizeY - 1) / groupSizeY
        threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1)
        
        super.init()
    }

    convenience init?(name: String, width: Int, height: Int, device: MTLDevice, library: MTLLibrary)
    {
        self.init(width: width, height: height)

        // Lookup kernel function in library
        guard let function = library.makeFunction(name: name) else {
            track("ERROR: Cannot find kernel function '\(name)' in library.")
            return nil
        }
        
        // Create kernel
        do {
            try kernel = device.makeComputePipelineState(function: function)
        } catch {
            track("ERROR: Cannot create compute kernel '\(name)'.")
            let alert = NSAlert()
            alert.alertStyle = .informational
            alert.icon = NSImage.init(named: NSImage.Name(rawValue: "metal"))
            alert.messageText = "Failed to create compute kernel."
            alert.informativeText = "Kernel '\(name)' will be ignored when selected."
            alert.addButton(withTitle: "OK")
            alert.runModal()
            return nil
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
            if (blurTexture != nil) {
                encoder.setTexture(blurTexture, index: 3);
            }
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
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypassupscaler", width: width, height: height, device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

class EPXUpscaler : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "epxupscaler", width: width, height: height, device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}

class XBRUpscaler : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "xbrupscaler", width: width, height: height, device: device, library: library)
        
        // Replace default texture sampler
        sampler = samplerNearest
    }
}


//
// Filters
//

class BypassFilter : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypass", width: width, height: height, device: device, library: library)
        sampler = samplerNearest
    }
}

class SmoothFilter : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypass", width: width, height: height, device: device, library: library)
        sampler = samplerLinear
    }
}

class BlurFilter : ComputeKernel {
    
    var blurWeightTexture: MTLTexture!

    convenience init?(name: String, width: Int, height: Int, device: MTLDevice, library: MTLLibrary, radius: Float) {
        self.init(name: name, width: width, height: height, device: device, library: library)
    
        // Build blur weight texture
        let sigma: Float = radius / 2.0
        let size:  Int   = Int(round(radius) * 2 + 1)
    
        var delta: Float = 0.0;
        var expScale: Float = 0.0
        
        if (radius > 0.0) {
            delta = (radius * 2) / Float(size - 1)
            expScale = -1.0 / (2 * sigma * sigma)
        }
    
        let weights = UnsafeMutablePointer<Float>.allocate(capacity: size)
        
        var weightSum: Float = 0.0;
        
        var x = -radius
        for i in 0 ..< size {
            let weight = expf((x * x) * expScale)
            weights[i] = weight
            weightSum += weight
            x += delta
        }
        
        let weightScale: Float = 1.0 / weightSum
        for j in 0 ..< size {
            weights[j] *= weightScale;
        }
    
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: MTLPixelFormat.r32Float, width: size, height: 1, mipmapped: false)
    
        blurWeightTexture = device.makeTexture(descriptor: textureDescriptor)!
        
        let region = MTLRegionMake2D(0, 0, size, 1)
        blurWeightTexture.replace(region: region, mipmapLevel: 0, withBytes: weights, bytesPerRow: size * 4 /* size of float */)
    
        weights.deallocate()
    }
    
    override func isBlurRequired() -> Bool {
        return true;
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder) {
        encoder.setTexture(blurWeightTexture, index: 2)
    }
}

class CrtFilter : ComputeKernel {
    
    var _bloomingFactor : Float = 1.0
    var bloom: MTLBuffer!
    
    func setBloomingFactor(_ value : Float) {
        
        _bloomingFactor = value;
        var _alpha : Float = 0.0
        
        let contents = bloom.contents()
        memcpy(contents, &_bloomingFactor, 4)
        memcpy(contents + 4, &_bloomingFactor, 4)
        memcpy(contents + 8, &_bloomingFactor, 4)
        memcpy(contents + 12, &_alpha, 4)
    }
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {

        self.init(name: "crt", width: width, height: height, device: device, library: library)
        self.bloom = device.makeBuffer(length: 16, options: .storageModeShared)
        
        setBloomingFactor(1.0)
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder) {
        
        encoder.setBuffer(bloom, offset: 0, index: 2)
    }
    
}

class BlurSampleFilter : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        self.init(name: "blur_sample", width: width, height: height, device: device, library: library)
        sampler = samplerLinear
    }
    
    override func isBlurRequired() -> Bool {
        return true;
    }
}

class ScanlineFilter : ComputeKernel {
    
    convenience init?(width: Int, height: Int, device: MTLDevice, library: MTLLibrary) {
        self.init(name: "scanline", width: width, height: height, device: device, library: library)
        sampler = samplerLinear
    }
}
