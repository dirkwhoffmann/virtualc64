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
    
    /* Synchronization semaphore. The semaphore is locked in function draw()
     * and released in function endFrame(). It's puprpose is to prevent a new
     * frame from being drawn if the previous isn't finished yet. Not sure if
     * we really need it.
     */
    var semaphore = DispatchSemaphore(value: 1)
    
    //
    // Metal objects
    //

    var queue: MTLCommandQueue! = nil
    var pipeline: MTLRenderPipelineState! = nil
    var depthState: MTLDepthStencilState! = nil
    
    //
    // Metal layers
    //
    
    var metalLayer: CAMetalLayer! = nil
    var splashScreen: SplashScreen! = nil
    var canvas: Canvas! = nil
    var console: Console! = nil
    
    // Current canvas size
    var size: CGSize {
        
        let frameSize = mtkView.frame.size
        let scale = mtkView.layer?.contentsScale ?? 1
        
        return CGSize(width: frameSize.width * scale,
                      height: frameSize.height * scale)
    }
    
    //
    // Ressources
    //
    
    var ressourceManager: RessourceManager! = nil
    
    //
    // Buffers and Uniforms
    //
    
    var bgRect: Node?
    var quad2D: Node?
    var quad3D: Quad?
    
    var vertexUniforms2D = VertexUniforms(mvp: matrix_identity_float4x4)
    var vertexUniforms3D = VertexUniforms(mvp: matrix_identity_float4x4)
    var vertexUniformsBg = VertexUniforms(mvp: matrix_identity_float4x4)

    var fragmentUniforms = FragmentUniforms(alpha: 1.0,
                                            dotMaskWidth: 0,
                                            dotMaskHeight: 0,
                                            scanlineDistance: 0)
    
    // Texture to hold the pixel depth information
    var depthTexture: MTLTexture! = nil
    
    //
    // Texture samplers
    //
    
    // Nearest neighbor sampler
    var samplerNearest: MTLSamplerState! = nil

    // Linear interpolation sampler
    var samplerLinear: MTLSamplerState! = nil
    
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
    var textureRect = CGRect.init()
    
    // Indicates if the whole texture should be displayed
    var zoom = false
    
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
        
        textureRect = computeTextureRect()
        setupMetal()
        
        mtkView.delegate = self
        mtkView.device = device
    }
        
    var maxTextureRect: CGRect {
        
        if parent.c64.vic.isPAL() {
            return CGRect.init(x: 104, y: 16, width: 487 - 104, height: 299 - 16)
        } else {
            return CGRect.init(x: 104, y: 16, width: 487 - 104, height: 249 - 16)
        }
    }

    var maxTextureRectScaled: CGRect {
        
        let texW = CGFloat(TextureSize.original.width)
        let texH = CGFloat(TextureSize.original.height)
        var rect = maxTextureRect
        rect.origin.x /= texW
        rect.origin.y /= texH
        rect.size.width /= texW
        rect.size.height /= texH
        return rect
    }

    func computeTextureRect() -> CGRect {
                
        // Display the whole texture if zoom is set
        if zoom { return CGRect.init(x: 0.0, y: 0.0, width: 1.0, height: 1.0) }
        
        let rect = maxTextureRect
        let aw = rect.minX
        let dw = rect.maxX
        let ah = rect.minY
        let dh = rect.maxY
        
        /*
        var aw: CGFloat, dw: CGFloat, ah: CGFloat, dh: CGFloat
        if parent.c64.vic.isPAL() {
            
            aw = CGFloat(104)
            dw = CGFloat(487)
            ah = CGFloat(16)
            dh = CGFloat(299)
            
        } else {
            
            aw = CGFloat(104)
            dw = CGFloat(487)
            ah = CGFloat(16)
            dh = CGFloat(249)
        }
        */
        
        /*
         *       aw <--------- maxWidth --------> dw
         *    ah |-----|---------------------|-----|
         *     ^ |     bw                   cw     |
         *     | -  bh *<----- width  ------>*     -
         *     | |     ^                     ^     |
         *     | |     |                     |     |
         *     | |   height                height  |
         *     | |     |                     |     |
         *     | |     v                     v     |
         *     | -  ch *<----- width  ------>*     -
         *     v |                                 |
         *    dh |-----|---------------------|-----|
         *
         *      aw/ah - dw/dh = largest posible texture cutout
         *      bw/bh - cw/ch = currently used texture cutout
         */
        let maxWidth = dw - aw
        let maxHeight = dh - ah
        
        let hZoom = CGFloat(config.hZoom)
        let vZoom = CGFloat(config.vZoom)
        let hCenter = CGFloat(config.hCenter)
        let vCenter = CGFloat(config.vCenter)

        /*
        let hZoom = CGFloat(0)
        let vZoom = CGFloat(0)
        let hCenter = CGFloat(0)
        let vCenter = CGFloat(0)
        */
        
        let width = (1 - hZoom) * maxWidth
        let bw = aw + hCenter * (maxWidth - width)
        let height = (1 - vZoom) * maxHeight
        let bh = ah + vCenter * (maxHeight - height)
        
        let texW = CGFloat(TextureSize.original.width)
        let texH = CGFloat(TextureSize.original.height)
        
        return CGRect.init(x: bw / texW,
                           y: bh / texH,
                           width: width / texW,
                           height: height / texH)
    }
     
    func updateTextureRect() {
        
        textureRect = computeTextureRect()
        buildVertexBuffer()
    }

    //
    // Managing layout
    //
    
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
            return screenshot(texture: canvas.emulatorTexture, rect: maxTextureRectScaled)
        case .entireUpscaled:
            return screenshot(texture: canvas.upscaledTexture, rect: maxTextureRectScaled)
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
    
        var bloomFilter: ComputeKernel! { return ressourceManager.bloomFilter }
        var upscaler: ComputeKernel! { return ressourceManager.upscaler }
        var scanlineFilter: ComputeKernel! { return ressourceManager.scanlineFilter }

        // Set uniforms for the fragment shader (where shall this be put?)
        fragmentUniforms.alpha = 1.0
        fragmentUniforms.dotMaskHeight = Int32(ressourceManager.dotMask.height)
        fragmentUniforms.dotMaskWidth = Int32(ressourceManager.dotMask.width)
        fragmentUniforms.scanlineDistance = Int32(size.height / 256)

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
        commandEncoder.setFragmentTexture(ressourceManager.dotMask, index: 4)
        commandEncoder.setFragmentBytes(&shaderOptions,
                                        length: MemoryLayout<ShaderOptions>.stride,
                                        index: 0)
                
        // Finally, we have to decide for a texture sampler. We use a linear
        // interpolation sampler, if Gaussian blur is enabled, and a nearest
        // neighbor sampler if Gaussian blur is disabled.
        let sampler = shaderOptions.blur > 0 ? samplerLinear : samplerNearest
        commandEncoder.setFragmentSamplerState(sampler, index: 0)

        return commandEncoder
    }
    
    func drawScene2D(encoder: MTLRenderCommandEncoder) {
        
        // Configure vertex shader
        encoder.setVertexBytes(&vertexUniforms2D,
                               length: MemoryLayout<VertexUniforms>.stride,
                               index: 1)
        
        // Configure fragment shader
        encoder.setFragmentTexture(canvas.scanlineTexture, index: 0)
        encoder.setFragmentBytes(&fragmentUniforms,
                                 length: MemoryLayout<FragmentUniforms>.stride,
                                 index: 1)
        
        // Draw
        quad2D!.drawPrimitives(encoder)
    }
    
    func drawScene3D(encoder: MTLRenderCommandEncoder) {
        
        let paused = parent.c64.paused
        let poweredOff = parent.c64.poweredOff
        let renderBackground = poweredOff || animates != 0 || fullscreen
        let renderForeground = alpha.current > 0.0
        
        // Perform a single animation step
        if animates != 0 { performAnimationStep() }
        
        if renderBackground {
            
            // Update background texture
            if !fullscreen {
                let buffer = parent.c64.vic.noise()
                canvas.updateBgTexture(bytes: buffer!)
            }
            
            // Configure vertex shader
            encoder.setVertexBytes(&vertexUniformsBg,
                                   length: MemoryLayout<VertexUniforms>.stride,
                                   index: 1)
            
            // Configure fragment shader
            if fullscreen {
                fragmentUniforms.alpha = 1.0
                encoder.setFragmentTexture(canvas.bgFullscreenTexture, index: 0)
                encoder.setFragmentTexture(canvas.bgFullscreenTexture, index: 1)
            } else {
                fragmentUniforms.alpha = noise.current
                encoder.setFragmentTexture(canvas.bgTexture, index: 0)
                encoder.setFragmentTexture(canvas.bgTexture, index: 1)
            }
            encoder.setFragmentBytes(&fragmentUniforms,
                                     length: MemoryLayout<FragmentUniforms>.stride,
                                     index: 1)
            
            // Draw
            bgRect!.drawPrimitives(encoder)
        }
        
        if renderForeground {
            
            // Configure vertex shader
            encoder.setVertexBytes(&vertexUniforms3D,
                                   length: MemoryLayout<VertexUniforms>.stride,
                                   index: 1)
            // Configure fragment shader
            fragmentUniforms.alpha = paused ? 0.5 : alpha.current
            encoder.setFragmentTexture(canvas.scanlineTexture, index: 0)
            encoder.setFragmentTexture(canvas.bloomTextureR, index: 1)
            encoder.setFragmentTexture(canvas.bloomTextureG, index: 2)
            encoder.setFragmentTexture(canvas.bloomTextureB, index: 3)
            encoder.setFragmentBytes(&fragmentUniforms,
                                     length: MemoryLayout<FragmentUniforms>.stride,
                                     index: 1)
            
            // Draw (part of) cube
            quad3D!.draw(encoder, allSides: animates != 0)
        }
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
                drawScene2D(encoder: encoder)
            } else {
                drawScene3D(encoder: encoder)
            }
            
            // Commit the command buffer
            encoder.endEncoding()
            buffer.addCompletedHandler { _ in self.semaphore.signal() }
            buffer.present(drawable)
            buffer.commit()
        }
    }
}
