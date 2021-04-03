// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import MetalPerformanceShaders

class Canvas: Layer {
    
    var bloomFilter: ComputeKernel! { return ressourceManager.bloomFilter }
    var upscaler: ComputeKernel! { return ressourceManager.upscaler }
    var scanlineFilter: ComputeKernel! { return ressourceManager.scanlineFilter }

    //
    // Textures
    //
    
    /* Emulator texture as provided by the emulator. The C64 screen is 428
     * pixels wide and 284 high and covers the upper left part of the emulator
     * texture. The emulator texture is updated in function updateTexture()
     * which is called periodically in drawRect().
     */
    var emulatorTexture: MTLTexture! = nil

    /* Bloom textures. To emulate a bloom effect, the emulator texture is first
     * split into it's R, G, and B parts. Each texture is then run through a
     * Gaussian blur filter with a large radius. These blurred textures are
     * passed into the fragment shader as a secondary textures where they are
     * recomposed with the upscaled primary texture.
     */
    var bloomTextureR: MTLTexture! = nil
    var bloomTextureG: MTLTexture! = nil
    var bloomTextureB: MTLTexture! = nil

    /* Upscaled emulator texture. In the first texture processing stage, the
     * emulator texture is bumped up by a factor of 4. The user can choose
     * between bypass upscaling which simply replaces each pixel by a 4x4 quad
     * or more sophisticated upscaling algorithms such as xBr.
     */
    var upscaledTexture: MTLTexture! = nil
    
    /* Upscaled texture with scanlines. In the second texture processing stage,
     * a scanline effect is applied to the upscaled texture.
     */
    var scanlineTexture: MTLTexture! = nil
    
    // Part of the texture that is currently visible
    var textureRect = CGRect.init() { didSet { buildVertexBuffers() } }

    //
    // Buffers and Uniforms
    //
    
    var quad2D: Node?
    var quad3D: Quad?
    
    var vertexUniforms2D = VertexUniforms(mvp: matrix_identity_float4x4)
    var vertexUniforms3D = VertexUniforms(mvp: matrix_identity_float4x4)
    var fragmentUniforms = FragmentUniforms(alpha: 1.0,
                                            white: 0.0,
                                            dotMaskWidth: 0,
                                            dotMaskHeight: 0,
                                            scanlineDistance: 0)

    //
    // Initializing
    //
    
    override init(renderer: Renderer) {

        super.init(renderer: renderer)
        buildVertexBuffers()
        buildTextures()
        
        /* We start with a negative alpha value to give it some time until
         * it becomes greater than 0. During this time, the splash screen will
         * be fully visible. */
        alpha.set(-1.0)
    }
    
    func buildVertexBuffers() {
                
        quad2D = Node.init(device: device,
                           x: -1.0, y: -1.0, z: 0.0, w: 2.0, h: 2.0,
                           t: textureRect)
        
        quad3D = Quad.init(device: device,
                           x1: -0.64, y1: -0.48, z1: -0.64,
                           x2: 0.64, y2: 0.48, z2: 0.64,
                           t: textureRect)
    }

    func buildTextures() {
        
        track()
        
        // Texture usages
        let r: MTLTextureUsage = [ .shaderRead ]
        let rwt: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget ]
        let rwtp: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget, .pixelFormatView ]
                
        // Emulator texture (long frames)
        emulatorTexture = device.makeTexture(size: TextureSize.original, usage: r)
        assert(emulatorTexture != nil, "Failed to create emulatorTexture")
        
        // Build bloom textures
        bloomTextureR = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureG = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureB = device.makeTexture(size: TextureSize.original, usage: rwt)
        assert(bloomTextureR != nil, "Failed to create bloomTextureR")
        assert(bloomTextureG != nil, "Failed to create bloomTextureG")
        assert(bloomTextureB != nil, "Failed to create bloomTextureB")
        
        // Upscaled texture
        upscaledTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        scanlineTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        assert(upscaledTexture != nil, "Failed to create upscaledTexture")
        assert(scanlineTexture != nil, "Failed to create scanlineTexture")

        var w = emulatorTexture.width
        var h = emulatorTexture.height
        track("Emulator texture created: \(w) x \(h)")
        
        w = upscaledTexture.width
        h = upscaledTexture.height
        track("Upscaled texture created: \(w) x \(h)")
    }
    
    //
    // Screenshots
    //
    
    func screenshot(source: ScreenshotSource) -> NSImage? {

        switch source {
            
        case .entire:
            return screenshot(texture: emulatorTexture, rect: largestVisibleNormalized)
        case .entireUpscaled:
            return screenshot(texture: upscaledTexture, rect: largestVisibleNormalized)
        case .visible:
            return screenshot(texture: emulatorTexture, rect: textureRect)
        case .visibleUpscaled:
            return screenshot(texture: upscaledTexture, rect: textureRect)
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
    // Updating
    //
        
    override func update(frames: Int64) {
            
        super.update(frames: frames)
        updateTexture()
    }
    
    func updateTexture() {
                
        if c64.poweredOff {

            let w = TextureSize.upscaled.width
            let h = TextureSize.upscaled.height
            scanlineTexture.replace(w: w, h: h, buffer: c64.vic.noise())
            return
        }
        
        let buf = c64.vic.stableEmuTexture()
        precondition(buf != nil)
        
        let pixelSize = 4
        let rowBytes = TEX_WIDTH * pixelSize
        let imageBytes = rowBytes * TEX_HEIGHT
        let region = MTLRegionMake2D(0, 0, TEX_WIDTH, TEX_HEIGHT)
        
        emulatorTexture.replace(region: region,
                                mipmapLevel: 0,
                                slice: 0,
                                withBytes: buf!,
                                bytesPerRow: rowBytes,
                                bytesPerImage: imageBytes)
    }
    
    //
    // Rendering
    //
    
    func makeCommandBuffer(buffer: MTLCommandBuffer) {
                        
        if c64.poweredOff { return }
        
        func applyGauss(_ texture: inout MTLTexture, radius: Float) {
            
            if #available(OSX 10.13, *) {
                let gauss = MPSImageGaussianBlur(device: device, sigma: radius)
                gauss.encode(commandBuffer: buffer,
                             inPlaceTexture: &texture, fallbackCopyAllocator: nil)
            }
        }
        
        // Compute the bloom textures
        if renderer.shaderOptions.bloom != 0 {
            
            bloomFilter.apply(commandBuffer: buffer,
                              textures: [emulatorTexture,
                                         bloomTextureR,
                                         bloomTextureG,
                                         bloomTextureB],
                              options: &renderer.shaderOptions,
                              length: MemoryLayout<ShaderOptions>.stride)
            
            applyGauss(&bloomTextureR, radius: renderer.shaderOptions.bloomRadiusR)
            applyGauss(&bloomTextureG, radius: renderer.shaderOptions.bloomRadiusG)
            applyGauss(&bloomTextureB, radius: renderer.shaderOptions.bloomRadiusB)
        }
        
        // Run the upscaler
        upscaler.apply(commandBuffer: buffer,
                       source: emulatorTexture,
                       target: upscaledTexture,
                       options: nil,
                       length: 0)
        
        // Blur the upscaled texture
        if renderer.shaderOptions.blur > 0 {
            
            let sigma = renderer.shaderOptions.blurRadius
            let gauss = MPSImageGaussianBlur(device: device, sigma: sigma)
            
            gauss.encode(commandBuffer: buffer,
                         inPlaceTexture: &upscaledTexture,
                         fallbackCopyAllocator: nil)
        }
        
        // Add scanlines
        scanlineFilter.apply(commandBuffer: buffer,
                             source: upscaledTexture,
                             target: scanlineTexture,
                             options: &renderer.shaderOptions,
                             length: MemoryLayout<ShaderOptions>.stride)
    }
    
    func setupFragmentShader(encoder: MTLRenderCommandEncoder) {
        
        // Setup textures
        encoder.setFragmentTexture(scanlineTexture, index: 0)
        encoder.setFragmentTexture(bloomTextureR, index: 1)
        encoder.setFragmentTexture(bloomTextureG, index: 2)
        encoder.setFragmentTexture(bloomTextureB, index: 3)
        encoder.setFragmentTexture(ressourceManager.dotMask, index: 4)

        // Select the texture sampler
        if renderer.shaderOptions.blur > 0 {
            encoder.setFragmentSamplerState(ressourceManager.samplerLinear, index: 0)
        } else {
            encoder.setFragmentSamplerState(ressourceManager.samplerNearest, index: 0)
        }
        
        // Setup uniforms
        fragmentUniforms.alpha = c64.paused ? 0.5 : alpha.current
        fragmentUniforms.dotMaskHeight = Int32(ressourceManager.dotMask.height)
        fragmentUniforms.dotMaskWidth = Int32(ressourceManager.dotMask.width)
        fragmentUniforms.scanlineDistance = Int32(renderer.size.height / 256)
        encoder.setFragmentBytes(&renderer.shaderOptions,
                                 length: MemoryLayout<ShaderOptions>.stride,
                                 index: 0)
        encoder.setFragmentBytes(&fragmentUniforms,
                                 length: MemoryLayout<FragmentUniforms>.stride,
                                 index: 1)
    }
    
    func render(encoder: MTLRenderCommandEncoder, flat: Bool) {
        
        flat ? render2D(encoder: encoder) : render3D(encoder: encoder)
    }
    
    func render2D(encoder: MTLRenderCommandEncoder) {
        
        // Configure the vertex shader
        encoder.setVertexBytes(&vertexUniforms2D,
                               length: MemoryLayout<VertexUniforms>.stride,
                               index: 1)
        
        // Configure the fragment shader
        setupFragmentShader(encoder: encoder)

        // Draw rectangle
        quad2D!.drawPrimitives(encoder)
    }
    
    func render3D(encoder: MTLRenderCommandEncoder) {
                
        // Perform a single animation step
        if renderer.animates != 0 { renderer.performAnimationStep() }
        
        // Configure vertex shader
        encoder.setVertexBytes(&vertexUniforms3D,
                               length: MemoryLayout<VertexUniforms>.stride,
                               index: 1)
        
        // Configure fragment shader
        setupFragmentShader(encoder: encoder)
        
        // Draw (part of) the cube
        quad3D!.draw(encoder, allSides: renderer.animates != 0)
    }
}
