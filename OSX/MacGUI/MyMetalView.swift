//
//  MyMetalView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 15.01.18.
//

// TODO (Once fully ported to Swift):
// eyeX,eyeY,eyeZ -> eye : float3
// Move buildKernels() -> setup
// startFrame() should return void
// Remove _ prefix from some variables
//

import Foundation

// All currently supported texture upscalers
var bypassUpscaler: ComputeKernel?
var epxUpscaler: ComputeKernel?
var xbrUpscaler: ComputeKernel?

// All currently supported texture filters
var bypassFilter: ComputeKernel?
var smoothFilter: ComputeKernel?
var blurFilter: ComputeKernel?
var saturationFilter: ComputeKernel?
var sepiaFilter: ComputeKernel?
var grayscaleFilter: ComputeKernel?
var crtFilter: ComputeKernel?


struct Sizeof {
    static let float = 4
    static let matrix4x4 = 16 * 4
}

struct C64Texture {
    static let orig = NSSize.init(width: 512, height: 512)
    static let upscaled = NSSize.init(width: 2048, height: 2048)
}

struct C64Upscaler {
    static let none = 1
    static let epx = 2
    static let xbr = 3
}

struct C64Filter {
    static let none = 1
    static let smooth = 2
    static let blur = 3
    static let saturation = 4
    static let grayscale = 5
    static let sepia = 6
    static let crt = 7
}

public extension MyMetalView {
    
    //! Adjusts view height by a certain number of pixels
    func adjustHeight(_ height: CGFloat) {
    
        var newFrame = frame
        newFrame.origin.y -= height
        newFrame.size.height += height
        frame = newFrame
    }
    
    //! Shrinks view vertically by the height of the status bar
    @objc public func shrink() { adjustHeight(-24.0) }
    
    //! Expand view vertically by the height of the status bar
    @objc public func expand() { adjustHeight(24.0) }

    internal func buildKernels() {
        
        precondition(device != nil)
        precondition(library != nil)
        
        // Build upscalers
        bypassUpscaler = BypassUpscaler.init(device: device!, library: library)
        epxUpscaler = EPXUpscaler.init(device: device!, library: library)
        xbrUpscaler = XBRUpscaler.init(device: device!, library: library)
    
        // Build filters
        bypassFilter = BypassFilter.init(device: device!, library: library)
        smoothFilter = SaturationFilter.init(device: device!, library: library, factor: 1.0)
        blurFilter = BlurFilter.init(device: device!, library: library, radius: 2.0)
        saturationFilter = SaturationFilter.init(device: device!, library: library, factor: 1.0)
        sepiaFilter = SepiaFilter.init(device: device!, library: library)
        grayscaleFilter = SaturationFilter.init(device: device!, library: library, factor: 0.0)
        crtFilter = CrtFilter.init(device: device!, library: library)
    }

    //! Returns the compute kernel of the currently selected upscaler
    internal func currentUpscaler() -> ComputeKernel {
    
        precondition(bypassUpscaler != nil)
        
        let upscalers = [C64Upscaler.epx: epxUpscaler,
                         C64Upscaler.xbr: xbrUpscaler]
        
        if let result = upscalers[videoUpscaler] {
            return result!
        } else {
            return bypassUpscaler!
        }
    }
    
    //! Returns the compute kernel of the currently selected postprocessing filer
    internal func currentFilter() -> ComputeKernel {
        
        precondition(bypassFilter != nil)
        
        let filters = [C64Filter.smooth: smoothFilter,
                       C64Filter.blur: blurFilter,
                       C64Filter.saturation: saturationFilter,
                       C64Filter.grayscale: grayscaleFilter,
                       C64Filter.sepia: sepiaFilter,
                       C64Filter.crt: crtFilter]
        
        if let result = filters[videoFilter] {
            return result!
        } else {
            return bypassFilter!
        }
    }
    
    @objc public func startFrame() -> Bool {
    
        _commandBuffer = queue.makeCommandBuffer()
        precondition(_commandBuffer != nil, "Command buffer must not be nil")
    
        // Upscale C64 texture
        let upscaler = currentUpscaler()
        upscaler.apply(commandBuffer: _commandBuffer,
                       source: emulatorTexture,
                       target: upscaledTexture)
    
        // Post-process C64 texture
        let filter = currentFilter()
        filter.apply(commandBuffer: _commandBuffer,
                     source: upscaledTexture,
                     target: filteredTexture)
    
        // Create render pass descriptor
        let descriptor = MTLRenderPassDescriptor.init()
        descriptor.colorAttachments[0].texture = _drawable.texture
        descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1)
        descriptor.colorAttachments[0].loadAction = MTLLoadAction.clear
        descriptor.colorAttachments[0].storeAction = MTLStoreAction.store
        
        descriptor.depthAttachment.texture = depthTexture
        descriptor.depthAttachment.clearDepth = 1
        descriptor.depthAttachment.loadAction = MTLLoadAction.clear
        descriptor.depthAttachment.storeAction = MTLStoreAction.dontCare
        
        // Create command encoder
        _commandEncoder = _commandBuffer.makeRenderCommandEncoder(descriptor: descriptor)
        _commandEncoder.setRenderPipelineState(pipeline)
        _commandEncoder.setDepthStencilState(depthState)
        _commandEncoder.setFragmentTexture(bgTexture, index: 0)
        _commandEncoder.setFragmentSamplerState(filter.getsampler(), index: 0)
        _commandEncoder.setVertexBuffer(positionBuffer, offset: 0, index: 0)
        
        return true
    }
}

