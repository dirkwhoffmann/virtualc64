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
// Get rid of commandBuffer argument in updateTexture
// Replace textureX etc. by textureRect : CGRect

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

    @objc public func updateScreenGeometry() {
    
        var rect: CGRect
        
        if c64proxy?.isPAL() == true {
    
            // PAL border will be 36 pixels wide and 34 pixels heigh
            rect = NSRect.init(x: CGFloat(PAL_LEFT_BORDER_WIDTH - 36),
                               y: CGFloat(PAL_UPPER_BORDER_HEIGHT - 34),
                               width: CGFloat(PAL_CANVAS_WIDTH + 2 * 36),
                               height: CGFloat(PAL_CANVAS_HEIGHT + 2 * 34))
            
            /*
            textureXStart = Float(PAL_LEFT_BORDER_WIDTH - 36) / Float(C64Texture.orig.width)
            textureXEnd = Float(PAL_LEFT_BORDER_WIDTH + PAL_CANVAS_WIDTH + 36) / Float(C64Texture.orig.width)
            textureYStart = Float(PAL_UPPER_BORDER_HEIGHT - 34) / Float(C64Texture.orig.height)
            textureYEnd = Float(PAL_UPPER_BORDER_HEIGHT + PAL_CANVAS_HEIGHT + 34) / Float(C64Texture.orig.height)
            */
        } else {
    
            // NTSC border will be 42 pixels wide and 9 pixels heigh
            rect = NSRect.init(x: CGFloat(NTSC_LEFT_BORDER_WIDTH - 42),
                               y: CGFloat(NTSC_UPPER_BORDER_HEIGHT - 9),
                               width: CGFloat(NTSC_CANVAS_WIDTH + 2 * 42),
                               height: CGFloat(NTSC_CANVAS_HEIGHT + 2 * 9))
        }
        
        textureXStart = Float(rect.minX / C64Texture.orig.width)
        textureXEnd = Float(rect.maxX / C64Texture.orig.width)
        textureYStart = Float(rect.minY / C64Texture.orig.height)
        textureYEnd = Float(rect.maxY / C64Texture.orig.height)
        
        /*
            textureXStart = Float(NTSC_LEFT_BORDER_WIDTH - 42) / Float(C64Texture.orig.width)
            textureXEnd = Float(NTSC_LEFT_BORDER_WIDTH + NTSC_CANVAS_WIDTH + 42) / Float(C64Texture.orig.width)
            textureYStart = Float(NTSC_UPPER_BORDER_HEIGHT - 9) / Float(C64Texture.orig.height)
            textureYEnd = Float(NTSC_UPPER_BORDER_HEIGHT + NTSC_CANVAS_HEIGHT + 9) / Float(C64Texture.orig.height)
         */

    
        // Enable this for debugging (will display the whole texture)
        // textureXStart = 0.0;
        // textureXEnd = 1.0;
        // textureYStart = 0.0;
        // textureYEnd = 1.0;
    
        // Update texture coordinates in vertex buffer
        buildVertexBuffer()
    }
    
    @objc public func updateTexture(cmdBuffer : MTLCommandBuffer?) {
    
        if c64proxy == nil || cmdBuffer == nil {
            return
        }
    
        let buf = c64proxy.vic.screenBuffer()
        precondition(buf != nil)
        
        let pixelSize = 4
        let width = Int(NTSC_PIXELS)
        let height = Int(PAL_RASTERLINES)
        let rowBytes = width * pixelSize
        let imageBytes = rowBytes * height
        let region = MTLRegionMake2D(0,0,width,height)
            
        emulatorTexture.replace(region: region,
                                mipmapLevel: 0,
                                slice: 0,
                                withBytes: buf!,
                                bytesPerRow: rowBytes,
                                bytesPerImage: imageBytes)
    }
    

    
    // MOVE TO SETUP
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
    
    @objc public func startFrame() {
    
        commandBuffer = queue.makeCommandBuffer()
        precondition(commandBuffer != nil, "Command buffer must not be nil")
    
        // Upscale C64 texture
        let upscaler = currentUpscaler()
        upscaler.apply(commandBuffer: commandBuffer,
                       source: emulatorTexture,
                       target: upscaledTexture)
    
        // Post-process C64 texture
        let filter = currentFilter()
        filter.apply(commandBuffer: commandBuffer,
                     source: upscaledTexture,
                     target: filteredTexture)
    
        // Create render pass descriptor
        let descriptor = MTLRenderPassDescriptor.init()
        descriptor.colorAttachments[0].texture = drawable.texture
        descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1)
        descriptor.colorAttachments[0].loadAction = MTLLoadAction.clear
        descriptor.colorAttachments[0].storeAction = MTLStoreAction.store
        
        descriptor.depthAttachment.texture = depthTexture
        descriptor.depthAttachment.clearDepth = 1
        descriptor.depthAttachment.loadAction = MTLLoadAction.clear
        descriptor.depthAttachment.storeAction = MTLStoreAction.dontCare
        
        // Create command encoder
        commandEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: descriptor)
        commandEncoder.setRenderPipelineState(pipeline)
        commandEncoder.setDepthStencilState(depthState)
        commandEncoder.setFragmentTexture(bgTexture, index: 0)
        commandEncoder.setFragmentSamplerState(filter.getsampler(), index: 0)
        commandEncoder.setVertexBuffer(positionBuffer, offset: 0, index: 0)
    }
    
    @objc public func drawScene2D() {
    
        startFrame()
    
        // Render quad
        commandEncoder.setFragmentTexture(filteredTexture, index: 0)
        commandEncoder.setVertexBuffer(uniformBuffer2D, offset: 0, index: 1)
        commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                      vertexStart: 42,
                                      vertexCount: 6,
                                      instanceCount: 1)
        endFrame()
    }
    
    @objc public func drawScene3D() {
    
        let animates = self.animates()
        let drawBackground = !fullscreen && (animates || !drawC64texture)
        
        if animates {
            updateAngles()
            buildMatrices3D()
        }

        startFrame()
    
        // Make texture transparent if emulator is halted
        let alpha = c64proxy.isHalted() ? 0.5 : currentAlpha
        fillAlpha(uniformBuffer3D, alpha)
        
        // Render background
        if drawBackground {
            commandEncoder.setFragmentTexture(bgTexture, index: 0)
            commandEncoder.setVertexBuffer(uniformBufferBg, offset: 0, index: 1)
            commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                          vertexStart: 0,
                                          vertexCount: 6,
                                          instanceCount: 1)
        }
        
        // Render cube
        if drawC64texture {
            commandEncoder.setFragmentTexture(filteredTexture, index: 0)
            commandEncoder.setVertexBuffer(uniformBuffer3D, offset: 0, index: 1)
            commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                          vertexStart: 6,
                                          vertexCount: (animates ? 24 : 6),
                                          instanceCount: 1)
        }
                
        endFrame()
    }

    @objc public func endFrame() {
    
        commandEncoder.endEncoding()
    
        let block_sema = semaphore
        
        commandBuffer.addCompletedHandler { cb in
            block_sema?.signal()
        }
            
        // if (drawable) {
        commandBuffer.present(drawable)
        commandBuffer.commit()
        // }
    }
    
    override open func setFrameSize(_ newSize: NSSize) {
        
        super.setFrameSize(newSize)
        layerIsDirty = true
    }
    
    func reshape(withFrame frame: CGRect) {
    
        if let scale = NSScreen.main?.backingScaleFactor {
            
            var size = bounds.size
            size.width *= scale
            size.height *= scale
            
            metalLayer.drawableSize = drawableSize
            reshape()
        }
    }

    func reshape() {

        let drawableSize = metalLayer.drawableSize
        
        if layerWidth == drawableSize.width && layerHeight == drawableSize.height {
            return
        }
    
        layerWidth = drawableSize.width
        layerHeight = drawableSize.height
    
        // NSLog("MetalLayer::reshape (%f,%f)", drawableSize.width, drawableSize.height);
    
        // Rebuild matrices
        buildMatricesBg()
        buildMatrices2D()
        buildMatrices3D()
    
        // Rebuild depth buffer
        buildDepthBuffer()
    }
    
    override open func draw(_ rect: NSRect) {
        
        if c64proxy == nil || !enableMetal {
            return
        }

        // Wait until it's save to go ...
        // let result semaphore.wait (timeout: .distantFuture)
        semaphore.wait()
        
        // Refresh size dependent items if needed
        if layerIsDirty {
            reshape(withFrame: frame)
            layerIsDirty = false
        }
    
        // Draw scene
        drawable = metalLayer.nextDrawable()
        if (drawable != nil) {
            updateTexture(cmdBuffer: commandBuffer)
            if fullscreen && !fullscreenKeepAspectRatio {
                drawScene2D()
            } else {
                drawScene3D()
            }
        }
    }
   
    @objc public func cleanup() {
    
        NSLog("MyMetalView::cleanup")
    }
    
}

