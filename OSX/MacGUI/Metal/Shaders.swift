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

    convenience init?(name: String, device: MTLDevice, library: MTLLibrary) {
        
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
    }
    
    func apply(commandBuffer: MTLCommandBuffer,
               source: MTLTexture,
               target: MTLTexture,
               options: ShaderOptions? = nil)
    {
        guard let encoder = commandBuffer.makeComputeCommandEncoder() else {
            return
        }
        
        // Bind pipeline and textures
        encoder.setComputePipelineState(kernel)
        encoder.setTexture(source, index: 0)
        encoder.setTexture(target, index: 1)
     
        // Pass in shader options
        if var _options = options {
            encoder.setBytes(&_options,
                             length: MemoryLayout<ShaderOptions>.stride,
                             index: 0);
        }
        
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
    }
}

class EPXUpscaler : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "epxupscaler", device: device, library: library)
    }
}

class XBRUpscaler : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "xbrupscaler", device: device, library: library)
    }
}


//
// Bloom filters
//

class BloomFilter : ComputeKernel {

    convenience init?(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "bloom", device: device, library: library)
    }
}

    
//
// Scanline filters
//

class SimpleScanlines : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "scanlines", device: device, library: library)
    }
}


//
// Bypass filter (DEPRECATED)
//

class BypassFilter : ComputeKernel {
    
    convenience init?(device: MTLDevice, library: MTLLibrary) {
        self.init(name: "bypass", device: device, library: library)
    }
}
