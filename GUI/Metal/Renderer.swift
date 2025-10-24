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

    case emulator = 0
    case upscaler = 1
    case framebuffer = 2
}

enum ScreenshotCutout: Int {

    case visible = 0
    case entire = 1
    case automatic = 2
    case custom = 3
}

@MainActor
class Renderer: NSObject, MTKViewDelegate {
    
    let view: MTKView
    let device: MTLDevice
    let parent: MyController
    
    var prefs: Preferences { return parent.pref }
    var config: Configuration { return parent.config }
    var emu: EmulatorProxy? { return parent.emu }

    // Number of drawn frames since power up
    var frames: Int64 = 0

    // The current GPU frame rate
    var fps = 60

    // Time stamp used for auto-detecting the frame rate
    var timestamp = CACurrentMediaTime()

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
    var onboarding: Onboarding! = nil
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

        if !BuildSettings.msgCallback {

            // Process all pending messages
            var msg = Message()
            while emu?.c64.getMessage(&msg) == true {
                parent.process(messsage: msg)
            }
        }

        splashScreen.update(frames: frames)
        onboarding.update(frames: frames)
        dropZone.update(frames: frames)
        console.update(frames: frames)
        canvas.update(frames: frames)
        parent.update(frames: frames)

        measureFps(frames: frames)
    }

    func measureFps(frames: Int64) {

        let interval = Int64(32)

        if frames % interval == 0 {

            let now = CACurrentMediaTime()
            let elapsed = now - timestamp
            timestamp = now

            let newfps = Int(round(Double(interval) / elapsed))
            if newfps != fps {

                debug(.vsync, "Measured GPU frame rate: \(newfps)")

                if [50, 60, 100, 120, 200, 240].contains(newfps) {

                    fps = newfps
                    emu?.set(.HOST_REFRESH_RATE, value: Int(fps))
                    debug(.vsync, "New GPU frame rate: \(fps)")
                }
            }
        }
    }

    func process(messsage msg: Message) {

        let option = Opt(rawValue: Int(msg.value))!

        switch msg.type {
            
        case .MON_SETTING:
            
            switch option {
                
            case .MON_HCENTER, .MON_VCENTER, .MON_HZOOM, .MON_VZOOM:
                canvas.updateTextureRect()

            default:
                updateShaderOption(option, value: msg.value2)
            }
            
        default:
            break
        }
    }
    
    //
    // Methods from MTKViewDelegate
    //

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {

        reshape(withSize: size)
    }
    
    func draw(in view: MTKView) {

        MainActor.assertIsolated("Not isolated!")

        frames += 1
        update(frames: frames)

        // Wait for the next frame
        semaphore.wait()

        // Get drawable
        guard let drawable = metalLayer.nextDrawable() else {

            semaphore.signal()
            return
        }

        // Create the command buffer
        let buffer = makeCommandBuffer()

        // Create the command encoder
        guard let encoder = makeCommandEncoder(drawable, buffer) else {

            semaphore.signal()
            return
        }

        // Render the scene
        let flat = fullscreen && !prefs.keepAspectRatio
        if canvas.isTransparent || animates != 0 { splashScreen.render(encoder) }
        if canvas.isVisible { canvas.render(encoder, flat: flat) }
        encoder.endEncoding()

        // Commit the command buffer
        buffer.addCompletedHandler { @Sendable [weak self] buffer in
            Task { @MainActor in
                self?.semaphore.signal()
            }
        }
        buffer.present(drawable)
        buffer.commit()
    }
}
