// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Metal
import MetalKit
import MetalPerformanceShaders

enum ScreenshotSource: Int {
        
    case visible = 0
    case visibleUpscaled = 1
    case entire = 2
    case entireUpscaled = 3
}

class Renderer: NSObject, MTKViewDelegate {
    
    let mtkView: MTKView
    let device: MTLDevice
    let parent: MyController
    
    var prefs: Preferences { return parent.pref }
    var config: Configuration { return parent.config }

    // Number of drawn frames since power up
    var frames: Int64 = 0
    
    // Frame synchronization semaphore
    var semaphore = DispatchSemaphore(value: 1)
    
    //
    // Metal objects
    //

    var queue: MTLCommandQueue! = nil
    var pipeline: MTLRenderPipelineState! = nil
    var depthState: MTLDepthStencilState! = nil
    
    //
    // Layers
    //
    
    var metalLayer: CAMetalLayer! = nil
    var splashScreen: SplashScreen! = nil
    var canvas: Canvas! = nil
    var console: Console! = nil
        
    //
    // Ressources
    //
    
    var ressourceManager: RessourceManager! = nil
    
    //
    // Buffers and Uniforms
    //
    
    var bgRect: Node?
    
    var vertexUniformsBg = VertexUniforms(mvp: matrix_identity_float4x4)

    // Texture to hold the pixel depth information
    var depthTexture: MTLTexture! = nil
    
    //
    // Texture samplers
    //
    
    // Shader options
    var shaderOptions: ShaderOptions!
    
    // Indicates if an animation is currently performed
     var animates = 0
    
    // Animation parameters
    var angleX = AnimatedFloat(0.0)
    var angleY = AnimatedFloat(0.0)
    var angleZ = AnimatedFloat(0.0)
    
    var shiftX = AnimatedFloat(0.0)
    var shiftY = AnimatedFloat(0.0)
    var shiftZ = AnimatedFloat(0.0)
    
    var alpha = AnimatedFloat(0.0)
    var noise = AnimatedFloat(0.0)
    
    // Animation variables for smooth texture zooming
    var cutoutX1 = AnimatedFloat.init()
    var cutoutY1 = AnimatedFloat.init()
    var cutoutX2 = AnimatedFloat.init()
    var cutoutY2 = AnimatedFloat.init()
    
    // Part of the texture that is currently visible
    var textureRect = CGRect.init() { didSet { buildVertexBuffers() } }
        
    // Is set to true when fullscreen mode is entered
    var fullscreen = false
        
    //
    // Initializing
    //
    
    init(view: MTKView, device: MTLDevice, controller: MyController) {
        
        self.mtkView = view
        self.device = device
        self.parent = controller
        super.init()
        
        setupMetal()
        
        mtkView.delegate = self
        mtkView.device = device
    }
    
    //
    // Managing layout
    //

    var size: CGSize {
        
        let frameSize = mtkView.frame.size
        let scale = mtkView.layer?.contentsScale ?? 1
        
        return CGSize(width: frameSize.width * scale,
                      height: frameSize.height * scale)
    }
        
    func reshape(withSize size: CGSize) {
    
        reshape()
    }

    func reshape() {

        // Rebuild matrices
        buildMatricesBg()
        buildMatrices2D()
        buildMatrices3D()
    
        // Rebuild depth buffer
        buildDepthBuffer()
    }
    
    public func cleanup() {
    
        track()
    }

    //
    // Screenshots
    //
    
    func screenshot(source: ScreenshotSource) -> NSImage? {

        switch source {
            
        case .entire:
            return screenshot(texture: canvas.emulatorTexture, rect: largestVisibleNormalized)
        case .entireUpscaled:
            return screenshot(texture: canvas.upscaledTexture, rect: largestVisibleNormalized)
        case .visible:
            return screenshot(texture: canvas.emulatorTexture, rect: textureRect)
        case .visibleUpscaled:
            return screenshot(texture: canvas.upscaledTexture, rect: textureRect)
        }
    }

    func screenshot(texture: MTLTexture, rect: CGRect) -> NSImage? {
        
        // Use the blitter to copy the texture data back from the GPU
        let queue = texture.device.makeCommandQueue()!
        let commandBuffer = queue.makeCommandBuffer()!
        let blitEncoder = commandBuffer.makeBlitCommandEncoder()!
        blitEncoder.synchronize(texture: texture, slice: 0, level: 0)
        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
        
        return NSImage.make(texture: texture, rect: rect)
    }
      
    //
    //  Drawing
    //
    
    func makeCommandBuffer() -> MTLCommandBuffer {
    
        let commandBuffer = queue.makeCommandBuffer()!
        canvas.makeCommandBuffer(buffer: commandBuffer)
        
        return commandBuffer
    }
    
    func makeCommandEncoder(drawable: CAMetalDrawable, buffer: MTLCommandBuffer) -> MTLRenderCommandEncoder {
        
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
        let commandEncoder = buffer.makeRenderCommandEncoder(descriptor: descriptor)!
        commandEncoder.setRenderPipelineState(pipeline)
        commandEncoder.setDepthStencilState(depthState)
        commandEncoder.setFragmentBytes(&shaderOptions,
                                        length: MemoryLayout<ShaderOptions>.stride,
                                        index: 0)
                
        // Finally, we have to decide for a texture sampler. We use a linear
        // interpolation sampler, if Gaussian blur is enabled, and a nearest
        // neighbor sampler otherwise.
        if shaderOptions.blur > 0 {
            commandEncoder.setFragmentSamplerState(ressourceManager.samplerLinear, index: 0)
        } else {
            commandEncoder.setFragmentSamplerState(ressourceManager.samplerNearest, index: 0)
        }

        return commandEncoder
    }
    
    //
    // Methods from MTKViewDelegate
    //

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {

        reshape(withSize: size)
    }
    
    func draw(in view: MTKView) {
        
        frames += 1
        
        semaphore.wait()
        if let drawable = metalLayer.nextDrawable() {
                    
            canvas.updateTexture()
            
            // Create the command buffer
            let buffer = makeCommandBuffer()
            
            // Create the command encoder
            let encoder = makeCommandEncoder(drawable: drawable, buffer: buffer)
            
            // Render the scene
            if fullscreen && !parent.pref.keepAspectRatio {
                canvas.render2D(encoder: encoder)
            } else {
                canvas.render3D(encoder: encoder)
            }
            
            // Commit the command buffer
            encoder.endEncoding()
            buffer.addCompletedHandler { _ in self.semaphore.signal() }
            buffer.present(drawable)
            buffer.commit()
        }
    }
}
