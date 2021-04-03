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
// import MetalPerformanceShaders

enum ScreenshotSource: Int {
        
    case visible = 0
    case visibleUpscaled = 1
    case entire = 2
    case entireUpscaled = 3
}

class Renderer: NSObject, MTKViewDelegate {
    
    let view: MTKView
    let device: MTLDevice
    let parent: MyController
    
    var prefs: Preferences { return parent.pref }
    var config: Configuration { return parent.config }

    // Number of drawn frames since power up
    var frames: Int64 = 0
    
    // Frame synchronization semaphore
    var semaphore = DispatchSemaphore(value: 1)
    
    //
    // Metal entities
    //

    var queue: MTLCommandQueue! = nil
    var pipeline: MTLRenderPipelineState! = nil
    var depthState: MTLDepthStencilState! = nil
    var descriptor: MTLRenderPassDescriptor! = nil
    
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

    // Shader options
    var shaderOptions: ShaderOptions!
    
    //
    // Animations
    //
    
    // Indicates if an animation is currently performed
     var animates = 0
    
    // Geometry animation parameters
    var angleX = AnimatedFloat(0.0)
    var angleY = AnimatedFloat(0.0)
    var angleZ = AnimatedFloat(0.0)
    
    var shiftX = AnimatedFloat(0.0)
    var shiftY = AnimatedFloat(0.0)
    var shiftZ = AnimatedFloat(0.0)
    
    // Color animation parameters
    // var alpha = AnimatedFloat(0.0)
    // var noise = AnimatedFloat(0.0)
    
    // Texture animation parameters
    var cutoutX1 = AnimatedFloat.init()
    var cutoutY1 = AnimatedFloat.init()
    var cutoutX2 = AnimatedFloat.init()
    var cutoutY2 = AnimatedFloat.init()
            
    // Indicates if fullscreen mode is enabled
    var fullscreen = false
        
    //
    // Initializing
    //
    
    init(view: MTKView, device: MTLDevice, controller: MyController) {
        
        self.view = view
        self.device = device
        self.parent = controller
        super.init()
        
        setupMetal()
        
        view.delegate = self
        view.device = device
    }
    
    //
    // Managing layout
    //

    var size: CGSize {
        
        let frameSize = view.frame.size
        let scale = view.layer?.contentsScale ?? 1
        
        return CGSize(width: frameSize.width * scale,
                      height: frameSize.height * scale)
    }
        
    func reshape(withSize size: CGSize) {
    
        reshape()
    }

    func reshape() {

        // Rebuild matrices
        buildMatrices2D()
        buildMatrices3D()
    
        // Rebuild the depth buffer
        ressourceManager.buildDepthBuffer()
    }
    
    public func cleanup() {
    
        track()
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
        
        // Update the render pass descriptor
        descriptor.colorAttachments[0].texture = drawable.texture
        descriptor.depthAttachment.texture = ressourceManager.depthTexture

        // Create command encoder
        let commandEncoder = buffer.makeRenderCommandEncoder(descriptor: descriptor)!
        commandEncoder.setRenderPipelineState(pipeline)
        commandEncoder.setDepthStencilState(depthState)
                
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
                 
            canvas.update(frames: frames)
            
            // Create the command buffer
            let buffer = makeCommandBuffer()
            
            // Create the command encoder
            let encoder = makeCommandEncoder(drawable: drawable, buffer: buffer)
            
            // Render the scene
            let flat = fullscreen && !parent.pref.keepAspectRatio
            if canvas.isTransparent { splashScreen.render(encoder: encoder) }
            if canvas.isVisible { canvas.render(encoder: encoder, flat: flat) }
            
            // Commit the command buffer
            encoder.endEncoding()
            buffer.addCompletedHandler { _ in self.semaphore.signal() }
            buffer.present(drawable)
            buffer.commit()
        }
    }
}
