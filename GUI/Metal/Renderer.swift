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

/*
struct C64Texture {
    static let orig = NSSize.init(width: 512, height: 512)
    static let upscaled = NSSize.init(width: 2048, height: 2048)
}
*/

class Renderer: NSObject, MTKViewDelegate {
    
    let mtkView: MTKView
    let device: MTLDevice
    let parent: MyController
    
    // Number of drawn frames since power up
    var frames: Int64 = 0
    
    /* Synchronization semaphore
     * The semaphore is locked in function draw() and released in function
     * endFrame(). It's puprpose is to prevent a new frame from being drawn
     * if the previous isn't finished yet. Not sure if we really need it.
     */
    var semaphore = DispatchSemaphore(value: 1)
    
    //
    // Metal objects
    //

    var library: MTLLibrary! = nil
    var queue: MTLCommandQueue! = nil
    var pipeline: MTLRenderPipelineState! = nil
    var depthState: MTLDepthStencilState! = nil
    var commandBuffer: MTLCommandBuffer! = nil
    var commandEncoder: MTLRenderCommandEncoder! = nil
    var drawable: CAMetalDrawable! = nil
    
    //
    // Metal layers
    //
    
    var metalLayer: CAMetalLayer! = nil
    
    // Current canvas size
    var size: CGSize {
        
        let frameSize = mtkView.frame.size
        let scale = mtkView.layer?.contentsScale ?? 1
        
        return CGSize(width: frameSize.width * scale,
                      height: frameSize.height * scale)
    }
    
    //
    // Buffers and uniforms
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
    
    // Textures
    
    // Background texture
    var bgTexture: MTLTexture! = nil
    
    // Texture to hold the pixel depth information
    var depthTexture: MTLTexture! = nil
    
    // Emulator texture as provided by the emulator.
    // The C64 screen has size 428 x 284 and covers the upper left part of the
    // emulator texture. The emulator texture is updated in function
    // updateTexture() which is called periodically in drawRect().
    var emulatorTexture: MTLTexture! = nil

    // Bloom textures to emulate blooming.
    // To emulate a bloom effect, the C64 texture is first split into it's
    // R, G, and B parts. Each texture is then run through a Gaussian blur
    // filter with a large radius. These blurred textures are passed into
    // the fragment shader as a secondary textures where they are recomposed
    // with the upscaled primary texture.
    var bloomTextureR: MTLTexture! = nil
    var bloomTextureG: MTLTexture! = nil
    var bloomTextureB: MTLTexture! = nil

    // Upscaled emulator texture.
    // In the first texture processing stage, the emulator texture is bumped up
    // by a factor of 4. The user can choose between bypass upscaling which
    // simply replaces each pixel by a 4x4 quad or more sophisticated upscaling
    // algorithms such as xBr.
    var upscaledTexture: MTLTexture! = nil
    
    // Upscaled texture with scanlines.
    // In the second texture processing stage, a scanline effect is applied to
    // the upscaled texture.
    var scanlineTexture: MTLTexture! = nil
    
    // Dotmask texture (variable size).
    // This texture is used by the fragment shader to emulate a dotmask
    // effect.
    var dotMaskTexture: MTLTexture! = nil
    
    // Array holding all available upscalers
    var upscalerGallery = [ComputeKernel?](repeating: nil, count: 3)

    // Array holding all available bloom filters
    var bloomFilterGallery = [ComputeKernel?](repeating: nil, count: 3)

    // Array holding all available scanline filters
    var scanlineFilterGallery = [ComputeKernel?](repeating: nil, count: 3)

    // Array holding dotmask preview images
    var dotmaskImages = [NSImage?](repeating: nil, count: 5)

    //
    // Texture samplers
    //
    
    // Nearest neighbor sampler
    var samplerNearest: MTLSamplerState! = nil

    // Linear interpolation sampler
    var samplerLinear: MTLSamplerState! = nil
    
    // Shader options
    var shaderOptions = Defaults.shaderOptions
    
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
    
    // Animation parameters (DEPRECATED)
    var currentXAngle = Float(0.0)
    var targetXAngle = Float(0.0)
    var deltaXAngle = Float(0.0)
    var currentYAngle = Float(0.0)
    var targetYAngle = Float(0.0)
    var deltaYAngle = Float(0.0)
    var currentZAngle = Float(0.0)
    var targetZAngle = Float(0.0)
    var deltaZAngle = Float(0.0)
    var currentEyeX = Defaults.eyeX
    var targetEyeX = Defaults.eyeX
    var deltaEyeX = Float(0.0)
    var currentEyeY = Defaults.eyeY
    var targetEyeY = Defaults.eyeY
    var deltaEyeY = Float(0.0)
    var currentEyeZ = Defaults.eyeZ
    var targetEyeZ = Defaults.eyeZ
    var deltaEyeZ = Float(0.0)
    var currentAlpha = Float(0.0)
    var targetAlpha = Float(0.0)
    var deltaAlpha = Float(0.0)
        
    // Texture cut-out (normalized)
    var textureRect = CGRect.init(x: 0.0, y: 0.0, width: 0.0, height: 0.0)
 
    // Currently selected texture upscaler
    var upscaler = Defaults.upscaler {
        didSet {
            if upscaler >= upscalerGallery.count || upscalerGallery[upscaler] == nil {
                track("Sorry, the selected GPU upscaler is unavailable.")
                upscaler = 0
            }
        }
    }
        
    // Is set to true when fullscreen mode is entered
    var fullscreen = false
    
    // If true, the 3D renderer is used in fullscreen mode, too
    var keepAspectRatio = Defaults.keepAspectRatio
    
    // If false, the C64 screen is not drawn (background texture will be visible)
    var drawC64texture = false
    
    //
    // Initializing
    //
    
    init(view: MTKView, device: MTLDevice, controller: MyController) {
        
        self.mtkView = view
        self.device = device
        self.parent = controller
        super.init()
        
        // textureRect = computeTextureRect()
        setupMetal()
        
        mtkView.delegate = self
        mtkView.device = device
    }
    
    //
    // Managing textures
    //
    
    func clearBgTexture() {
        
        let w = 512
        let h = 512
        
        let bytes = UnsafeMutablePointer<UInt32>.allocate(capacity: w * h)
        bytes.initialize(repeating: 0xFFFF0000, count: w * h)
        
        updateBgTexture(bytes: bytes)
        bytes.deallocate()
    }
    
    func updateBgTexture(bytes: UnsafeMutablePointer<UInt32>) {
        
        bgTexture.replace(w: 512, h: 512, buffer: bytes)
    }
    
    func updateTexture() {
        
        let buf = parent.c64.vic.screenBuffer()
        precondition(buf != nil)
        
        let pixelSize = 4
        let width = Int(NTSC_PIXELS)
        let height = Int(PAL_RASTERLINES)
        let rowBytes = width * pixelSize
        let imageBytes = rowBytes * height
        let region = MTLRegionMake2D(0, 0, width, height)
        
        emulatorTexture.replace(region: region,
                                mipmapLevel: 0,
                                slice: 0,
                                withBytes: buf!,
                                bytesPerRow: rowBytes,
                                bytesPerImage: imageBytes)
    }
    
    public func updateScreenGeometry() {
    
        var rect: CGRect
        
        if parent.c64.vic.isPAL() {
    
            // PAL border will be 36 pixels wide and 34 pixels heigh
            rect = CGRect.init(x: CGFloat(PAL_LEFT_BORDER_WIDTH - 36),
                                      y: CGFloat(PAL_UPPER_BORDER_HEIGHT - 34),
                                      width: CGFloat(PAL_CANVAS_WIDTH + 2 * 36),
                                      height: CGFloat(PAL_CANVAS_HEIGHT + 2 * 34))
            
        } else {
    
            // NTSC border will be 42 pixels wide and 9 pixels heigh
            rect = CGRect.init(x: CGFloat(NTSC_LEFT_BORDER_WIDTH - 42),
                                      y: CGFloat(NTSC_UPPER_BORDER_HEIGHT - 9),
                                      width: CGFloat(NTSC_CANVAS_WIDTH + 2 * 42),
                                      height: CGFloat(NTSC_CANVAS_HEIGHT + 2 * 9))
        }
        
        let texW = CGFloat(TextureSize.original.width)
        let texH = CGFloat(TextureSize.original.height)
        textureRect = CGRect.init(x: rect.minX / texW,
                                  y: rect.minY / texH,
                                  width: rect.width / texW,
                                  height: rect.height / texH)
        
        // Enable this for debugging (will display the whole texture)
        // textureXStart = 0.0;
        // textureXEnd = 1.0;
        // textureYStart = 0.0;
        // textureYEnd = 1.0;
    
        // Update texture coordinates in vertex buffer
        buildVertexBuffer()
    }
    
    /// Returns the compute kernel of the currently selected pixel upscaler
    func currentUpscaler() -> ComputeKernel {
    
        precondition(upscaler < upscalerGallery.count)
        precondition(upscalerGallery[0] != nil)
        
        return upscalerGallery[upscaler]!
    }

    /// Returns the compute kernel of the currently selected bloom filter
    func currentBloomFilter() -> ComputeKernel {
        
        precondition(shaderOptions.bloom < bloomFilterGallery.count)
        precondition(bloomFilterGallery[0] != nil)
        
        return bloomFilterGallery[Int(shaderOptions.bloom)]!
    }

    /// Returns the compute kernel of the currently selected scanline filter
    func currentScanlineFilter() -> ComputeKernel {
        
        precondition(shaderOptions.scanlines < scanlineFilterGallery.count)
        precondition(scanlineFilterGallery[0] != nil)
        
        return scanlineFilterGallery[Int(shaderOptions.scanlines)]!
    }
    
    func startFrame() {
    
        commandBuffer = queue.makeCommandBuffer()
        precondition(commandBuffer != nil, "Command buffer must not be nil")
    
        // Set uniforms for the fragment shader
        // fillFragmentShaderUniforms(uniformFragment)
        fragmentUniforms.alpha = 1.0
        fragmentUniforms.dotMaskHeight = Int32(dotMaskTexture.height)
        fragmentUniforms.dotMaskWidth = Int32(dotMaskTexture.width)
        fragmentUniforms.scanlineDistance = Int32(size.height / 256)
       
        // Compute the bloom textures
        if shaderOptions.bloom != 0 {
            let bloomFilter = currentBloomFilter()
            bloomFilter.apply(commandBuffer: commandBuffer,
                              textures: [emulatorTexture, bloomTextureR, bloomTextureG, bloomTextureB],
                              options: shaderOptions)
            
            func applyGauss(_ texture: inout MTLTexture, radius: Float) {
                
                if #available(OSX 10.13, *) {
                    let gauss = MPSImageGaussianBlur(device: device, sigma: radius)
                    gauss.encode(commandBuffer: commandBuffer,
                                 inPlaceTexture: &texture, fallbackCopyAllocator: nil)
                }
            }
            applyGauss(&bloomTextureR, radius: shaderOptions.bloomRadiusR)
            applyGauss(&bloomTextureG, radius: shaderOptions.bloomRadiusG)
            applyGauss(&bloomTextureB, radius: shaderOptions.bloomRadiusB)
        }
        
        // Upscale the C64 texture
        let upscaler = currentUpscaler()
        upscaler.apply(commandBuffer: commandBuffer,
                       source: emulatorTexture,
                       target: upscaledTexture)
    
        // Blur the upscaled texture
        if #available(OSX 10.13, *), shaderOptions.blur > 0 {
            let gauss = MPSImageGaussianBlur(device: device,
                                             sigma: shaderOptions.blurRadius)
            gauss.encode(commandBuffer: commandBuffer,
                         inPlaceTexture: &upscaledTexture,
                         fallbackCopyAllocator: nil)
        }
        
        // Emulate scanlines
        let scanlineFilter = currentScanlineFilter()
        scanlineFilter.apply(commandBuffer: commandBuffer,
                                source: upscaledTexture,
                                target: scanlineTexture,
                                options: shaderOptions)
        
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
        commandEncoder.setFragmentTexture(dotMaskTexture, index: 4)
        commandEncoder.setFragmentBytes(&shaderOptions,
                                        length: MemoryLayout<ShaderOptions>.stride,
                                        index: 0)
                
        // Finally, we have to decide for a texture sampler. We use a linear
        // interpolation sampler, if Gaussian blur is enabled, and a nearest
        // neighbor sampler if Gaussian blur is disabled.
        let sampler = shaderOptions.blur > 0 ? samplerLinear : samplerNearest
        commandEncoder.setFragmentSamplerState(sampler, index: 0)

    }
    
    func drawScene2D() {
    
        startFrame()
    
        // Configure vertex shader
        commandEncoder.setVertexBytes(&vertexUniforms2D,
                                      length: MemoryLayout<VertexUniforms>.stride,
                                      index: 1)

        // Configure fragment shader
        commandEncoder.setFragmentTexture(scanlineTexture, index: 0)
        commandEncoder.setFragmentBytes(&fragmentUniforms,
                                        length: MemoryLayout<FragmentUniforms>.stride,
                                        index: 1)
        
        // Draw
        quad2D!.drawPrimitives(commandEncoder)
        
        endFrame()
    }
    
    func drawScene3D() {
    
        let poweredOff = parent.c64.isPoweredOff()

        let animates = self.animatesDeprecated()
        let renderBackground = poweredOff || fullscreen
        // let drawBackground = !fullscreen && (animates || !drawC64texture)
        
        if animates {
            updateAngles()
            buildMatrices3D()
        }

        startFrame()
        
        // Render background
        if renderBackground {
            
            // Update background texture
            if !fullscreen {
                let buffer = parent.c64.vic.noise()
                updateBgTexture(bytes: buffer!)
            }
            
            // Configure vertex shader
            commandEncoder.setVertexBytes(&vertexUniformsBg,
                                          length: MemoryLayout<VertexUniforms>.stride,
                                          index: 1)
            
            // Configure fragment shader
            fragmentUniforms.alpha = 1
            commandEncoder.setFragmentTexture(bgTexture, index: 0)
            commandEncoder.setFragmentTexture(bgTexture, index: 1)
            commandEncoder.setFragmentBytes(&fragmentUniforms,
                                            length: MemoryLayout<FragmentUniforms>.stride,
                                            index: 1)
            
            // Draw
            bgRect!.drawPrimitives(commandEncoder)
        }
        
        // Render cube
        if drawC64texture && !poweredOff {
            
            // Configure vertex shader
            commandEncoder.setVertexBytes(&vertexUniforms3D,
                                          length: MemoryLayout<VertexUniforms>.stride,
                                          index: 1)
            // Configure fragment shader
            fragmentUniforms.alpha = parent.c64.isRunning() ? currentAlpha : 0.5
            commandEncoder.setFragmentTexture(scanlineTexture, index: 0)
            commandEncoder.setFragmentTexture(bloomTextureR, index: 1)
            commandEncoder.setFragmentTexture(bloomTextureG, index: 2)
            commandEncoder.setFragmentTexture(bloomTextureB, index: 3)
            commandEncoder.setFragmentBytes(&fragmentUniforms,
                                            length: MemoryLayout<FragmentUniforms>.stride,
                                            index: 1)
            
            // Draw (part of) cube
              quad3D!.draw(commandEncoder, allSides: animates)
        }
                
        endFrame()
    }

    func endFrame() {
    
        commandEncoder.endEncoding()
    
        commandBuffer.addCompletedHandler { _ in
            self.semaphore.signal()
        }
        
        if drawable != nil {
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }
        
        frames += 1
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
    // Methods from MTKViewDelegate
    //

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {

        reshape(withSize: size)
    }
    
    func draw(in view: MTKView) {
        
        semaphore.wait()

        drawable = metalLayer.nextDrawable()
        if drawable != nil {
            updateTexture()
            if fullscreen && !keepAspectRatio {
                drawScene2D()
            } else {
                drawScene3D()
            }
        }
    }
}
