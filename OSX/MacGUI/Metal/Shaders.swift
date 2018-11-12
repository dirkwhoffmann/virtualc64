//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import Metal
import MetalKit
import MetalPerformanceShaders


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
    static let factor_x = 4
    static let factor_y = 4
    static let width = C64_TEXTURE.width * UPSCALED_TEXTURE.factor_x
    static let height = C64_TEXTURE.height * UPSCALED_TEXTURE.factor_y
    static let cutout_x = C64_TEXTURE.cutout_x * UPSCALED_TEXTURE.factor_x
    static let cutout_y = C64_TEXTURE.cutout_y * UPSCALED_TEXTURE.factor_y
}

struct FILTERED_TEXTURE {
    static let factor_x = 4
    static let factor_y = 4
    static let width = C64_TEXTURE.width * FILTERED_TEXTURE.factor_x
    static let height = C64_TEXTURE.height * FILTERED_TEXTURE.factor_y
    static let cutout_x = C64_TEXTURE.cutout_x * FILTERED_TEXTURE.factor_x
    static let cutout_y = C64_TEXTURE.cutout_y * FILTERED_TEXTURE.factor_y
}

class ComputeKernel : NSObject {

    var device : MTLDevice!
    var kernel : MTLComputePipelineState!
    // var sampler : MTLSamplerState!

    // var samplerLinear : MTLSamplerState!
    // var samplerNearest : MTLSamplerState!
    
    convenience init?(name: String, device: MTLDevice, library: MTLLibrary)
    {
        self.init()
        
        self.device = device
        
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
        
        /*
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
        */
    }
    
    /*
    func getSampler() -> MTLSamplerState
    {
        return sampler
    }
    */
    
    func configureComputeCommandEncoder(encoder : MTLComputeCommandEncoder)
    {
        // Each specific compute kernel puts its initialization code here
    }
    
    func apply(commandBuffer: MTLCommandBuffer, source: MTLTexture, target: MTLTexture)
    {
        guard let encoder = commandBuffer.makeComputeCommandEncoder() else {
            return
        }
        
        // Bind pipeline and textures
        encoder.setComputePipelineState(kernel)
        encoder.setTexture(source, index: 0)
        encoder.setTexture(target, index: 1)
     
        // Apply shader specific configurations (if any)
        configureComputeCommandEncoder(encoder: encoder)
        
        // Determine thread group size and number of groups
        let groupW = kernel.threadExecutionWidth
        let groupH = kernel.maxTotalThreadsPerThreadgroup / groupW
        let threadsPerGroup = MTLSizeMake(groupW, groupH, 1)
        
        let countW = (FILTERED_TEXTURE.cutout_x + groupW - 1) / groupW;
        let countH = (FILTERED_TEXTURE.cutout_y + groupH - 1) / groupH;
        let threadgroupCount = MTLSizeMake(countW, countH, 1)
        
        // Finally, we're ready to dispatch
        encoder.dispatchThreadgroups(threadgroupCount,
                                     threadsPerThreadgroup: threadsPerGroup)
        encoder.endEncoding()
    }
}

//
// Upscalers
//

class BypassUpscaler : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypassupscaler", device: device, library: library)
        
        // Replace default texture sampler
        // sampler = samplerNearest
    }
}

class EPXUpscaler : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "epxupscaler", device: device, library: library)
        
        // Replace default texture sampler
        // sampler = samplerNearest
    }
}

class XBRUpscaler : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "xbrupscaler", device: device, library: library)
        
        // Replace default texture sampler
        // sampler = samplerNearest
    }
}


//
// Scanline filters
//

class SimpleScanlines : ComputeKernel {
    
    private var crtParameters: CrtParameters!
    
    struct CrtParameters {
        var scanlineWeight: Float
    }
    
    func setScanlineWeight(_ value : Float) {
        crtParameters.scanlineWeight = value
    }
    
    convenience init?(device: MTLDevice, library: MTLLibrary)
    {
        self.init(name: "scanlines",
                  device: device,
                  library: library)
        crtParameters = CrtParameters.init(scanlineWeight: 0.0)
        // sampler = samplerLinear; //  samplerNearest
    }
    
    override func configureComputeCommandEncoder(encoder: MTLComputeCommandEncoder) {
        encoder.setBytes(&crtParameters, length: MemoryLayout<CrtParameters>.stride, index: 0);
    }
}

//
// Filters
//

class BypassFilter : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypass", device: device, library: library)
        // sampler = samplerNearest
    }
}

/*
class SmoothFilter : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        
        self.init(name: "bypass", device: device, library: library)
        sampler = samplerLinear
    }
}
*/

// DEPRECATED
/*
class GaussFilter : ComputeKernel {
    
    var sigma = Float(0.0)
    
    convenience init?(device: MTLDevice, library: MTLLibrary, sigma: Float) {
        
        self.init(name: "bypass", device: device, library: library)
        self.sigma = sigma
    }
    
    override func apply(commandBuffer: MTLCommandBuffer, source: MTLTexture, target: MTLTexture) {
        
        if #available(OSX 10.13, *) {
            let gauss = MPSImageGaussianBlur(device: device, sigma: sigma)
            gauss.encode(commandBuffer: commandBuffer,
                         sourceTexture: source,
                         destinationTexture: target)
        } else {
            // Apply bypass on earlier versions
            super.apply(commandBuffer: commandBuffer, source: source, target: target)
        }
    }
}
*/
