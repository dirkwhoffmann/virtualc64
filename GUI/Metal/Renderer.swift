// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Metal

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
    var dropZone: DropZone! = nil
        
    //
    // Ressources
    //
    
    var ressourceManager: RessourceManager! = nil
    
    //
    // Uniforms
    //

    var shaderOptions: ShaderOptions!
    
    //
    // Animations
    //
    
    // Indicates if an animation is in progress
     var animates = 0
    
    // Geometry animation parameters
    var angleX = AnimatedFloat(0.0)
    var angleY = AnimatedFloat(0.0)
    var angleZ = AnimatedFloat(0.0)
    
    var shiftX = AnimatedFloat(0.0)
    var shiftY = AnimatedFloat(0.0)
    var shiftZ = AnimatedFloat(0.0)

    // Color animation parameters
    var white = AnimatedFloat(0.0)
        
    // Texture animation parameters
    var cutoutX1 = AnimatedFloat()
    var cutoutY1 = AnimatedFloat()
    var cutoutX2 = AnimatedFloat()
    var cutoutY2 = AnimatedFloat()
            
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
                
        self.view.device = device
        self.view.delegate = self
        
        setup()
    }
    
    func halt() {

        // Wait until the current frame has been completed
        semaphore.wait()
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
        
    //
    //  Drawing
    //
    
    func makeCommandBuffer() -> MTLCommandBuffer {
    
        let commandBuffer = queue.makeCommandBuffer()!
        canvas.makeCommandBuffer(buffer: commandBuffer)
        
        return commandBuffer
    }
    
    func makeCommandEncoder(_ drawable: CAMetalDrawable, _ buffer: MTLCommandBuffer) -> MTLRenderCommandEncoder? {
        
        // Update the render pass descriptor
        descriptor.colorAttachments[0].texture = drawable.texture
        descriptor.depthAttachment.texture = ressourceManager.depthTexture

        // Create command encoder
        let encoder = buffer.makeRenderCommandEncoder(descriptor: descriptor)
        encoder?.setRenderPipelineState(pipeline)
        encoder?.setDepthStencilState(depthState)
                
        return encoder
    }
    
    //
    // Updating
    //
    
    func update(frames: Int64) {
                         
        if animates != 0 { animate() }
        
        dropZone.update(frames: frames)
        console.update(frames: frames)
        canvas.update(frames: frames)
    }
    
    //
    // Methods from MTKViewDelegate
    //

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {

        parent.c64.host.frameBufferSize = size
        reshape(withSize: size)
    }
    
    func draw(in view: MTKView) {
        
        frames += 1
        update(frames: frames)
        
        semaphore.wait()
        
        if let drawable = metalLayer.nextDrawable() {
                 
            // Create the command buffer
            let buffer = makeCommandBuffer()
            
            // Create the command encoder
            guard let encoder = makeCommandEncoder(drawable, buffer) else {
                semaphore.signal()
                return
            }
            
            // Render the scene
            let flat = fullscreen && !parent.pref.keepAspectRatio
            if canvas.isTransparent || animates != 0 { splashScreen.render(encoder) }
            if canvas.isVisible { canvas.render(encoder, flat: flat) }
            encoder.endEncoding()

            // Commit the command buffer
            buffer.addCompletedHandler { _ in
                self.canvas.updateTexture()
                self.semaphore.signal()
            }
            buffer.present(drawable)
            buffer.commit()
        }
        
        // Perform periodic events inside the controller
        if frames % 5 == 0 { parent.timerFunc() }
    }
}
