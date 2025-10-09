// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import MetalPerformanceShaders

class Canvas: Layer {
    
    var bloomFilter: ComputeKernel! { return ressourceManager.bloomFilter }
    var upscaler: ComputeKernel! { return ressourceManager.upscaler }
    var scanlineFilter: ComputeKernel! { return ressourceManager.scanlineFilter }
    
    // Used to determine if the GPU texture needs to be updated
    var prevNr = 0
    
    // Used to determine if the GPU texture needs to be updated (DEPRECATED)
    var prevBuffer: UnsafeMutablePointer<UInt32>?
    
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
    var textureRect = CGRect.zero { didSet { buildVertexBuffers() } }
    
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
        alpha.set(-1.5)
    }
    
    func buildVertexBuffers() {
        
        quad2D = Node(device: device,
                      x: -1.0, y: -1.0, z: 0.0, w: 2.0, h: 2.0,
                      t: textureRect)
        
        quad3D = Quad(device: device,
                      x1: -0.64, y1: -0.48, z1: -0.64,
                      x2: 0.64, y2: 0.48, z2: 0.64,
                      t: textureRect)
    }
    
    func buildTextures() {
        
        // Texture usages
        let r: MTLTextureUsage = [ .shaderRead ]
        let rwt: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget ]
        let rwtp: MTLTextureUsage = [ .shaderRead, .shaderWrite, .renderTarget, .pixelFormatView ]
        
        // Emulator texture
        emulatorTexture = device.makeTexture(size: TextureSize.original, usage: r)
        renderer.metalAssert(emulatorTexture != nil,
                             "Failed to create emulatorTexture.")
        
        // Build bloom textures
        bloomTextureR = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureG = device.makeTexture(size: TextureSize.original, usage: rwt)
        bloomTextureB = device.makeTexture(size: TextureSize.original, usage: rwt)
        renderer.metalAssert(bloomTextureR != nil,
                             "The bloom texture (R channel) could not be allocated.")
        renderer.metalAssert(bloomTextureG != nil,
                             "The bloom texture (G channel) could not be allocated.")
        renderer.metalAssert(bloomTextureB != nil,
                             "The bloom texture (B channel) could not be allocated.")
        
        // Upscaled texture
        upscaledTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        scanlineTexture = device.makeTexture(size: TextureSize.upscaled, usage: rwtp)
        renderer.metalAssert(upscaledTexture != nil,
                             "The upscaling texture could not be allocated.")
        renderer.metalAssert(scanlineTexture != nil,
                             "The scanline texture could not be allocated.")
    }
    
    //
    // Updating
    //
    
    override func update(frames: Int64) {
        
        super.update(frames: frames)
        
        // Grab the current texture
        updateTexture()
        
        // Let the emulator compute the next frame
        emu?.wakeUp()
    }
    
    func updateTexture() {
        
        precondition(scanlineTexture != nil)
        precondition(emulatorTexture != nil)
        
        guard let emu = emu else { return }
        
        var buffer: UnsafePointer<UInt32>!
        var nr = 0
        
        // Prevent the stable texture from changing
        emu.videoPort.lockTexture()
        
        // Get a pointer to most recent texture
        emu.videoPort.texture(&buffer, nr: &nr)
        
        // Check for duplicated or dropped frames
        if nr != prevNr + 1 {
            
            debug(.vsync, "Frame sync mismatch (\(prevNr) -> \(nr))")
        }
        prevNr = nr
        
        // Update the GPU texture
        let w = Constants.texWidth
        let h = Constants.texHeight
        emulatorTexture.replace(w: w, h: h, buffer: buffer)
        
        // Release the texture lock
        emu.videoPort.unlockTexture()
    }
    
    //
    // Rendering
    //
    
    func makeCommandBuffer(buffer: MTLCommandBuffer) {
        
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
            
            applyGauss(&bloomTextureR, radius: renderer.shaderOptions.bloomRadius)
            applyGauss(&bloomTextureG, radius: renderer.shaderOptions.bloomRadius)
            applyGauss(&bloomTextureB, radius: renderer.shaderOptions.bloomRadius)
        }
        
        // Compute the upscaled texture
        upscaler.apply(commandBuffer: buffer,
                       source: emulatorTexture,
                       target: upscaledTexture,
                       options: nil,
                       length: 0)
        
        // Blur the upscaled texture
        if renderer.shaderOptions.blur > 0 {
            
            applyGauss(&upscaledTexture, radius: renderer.shaderOptions.blurRadius)
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
        fragmentUniforms.alpha = (emu?.paused ?? true) ? 0.5 : alpha.current
        fragmentUniforms.white = renderer.white.current
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
    
    func render(_ encoder: MTLRenderCommandEncoder, flat: Bool) {
        
        if flat { render2D(encoder: encoder) } else { render3D(encoder: encoder) }
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

//
// Screenshots
//

extension Canvas {
    
    func screenshot(source: ScreenshotSource, cutout: ScreenshotCutout, width: Int? = nil, height: Int? = nil) -> NSImage? {
        
        // print("screenshot(source: \(source), cutout: \(cutout), width: \(width), height: \(height)")
        
        switch source {
            
        case .framebuffer: return framebuffer
        case .emulator: return screenshot(texture: emulatorTexture, cutout: cutout, width: width, height: height)
        case .upscaler: return screenshot(texture: upscaledTexture, cutout: cutout, width: width, height: height)
        }
    }
    
    func screenshot(texture: MTLTexture, cutout: ScreenshotCutout, width: Int? = nil, height: Int? = nil) -> NSImage? {
        
        // print("screenshot(cutout: \(cutout), width: \(width), height: \(height)")
        
        func region(cgRect rect: CGRect) -> MTLRegion {
            
            // Compute scaling factors
            let w = CGFloat(texture.width)
            let h = CGFloat(texture.height)
            
            // Assemble the region
            let origin = MTLOrigin(x: Int(rect.minX * w), y: Int(rect.minY * h), z: 0)
            let size = MTLSize(width: Int(rect.width * w), height: Int(rect.height * h), depth: 1)
            return MTLRegion(origin: origin, size: size)
        }
        
        func region(x1: Int, x2: Int, y1: Int, y2: Int) -> MTLRegion {
            
            // Revert to the entire texture if a zero rect is given
            if x1 == x2 || y1 == y2 { return region(cgRect: largestVisibleNormalized) }
            
            // Assemble the region
            let origin = MTLOrigin(x: x1, y: y1, z: 0)
            let size = MTLSize(width: x2 - x1 + 1, height: y2 - y1 + 1, depth: 1)
            return MTLRegion(origin: origin, size: size)
        }
        
        switch cutout {
            
        case .visible:
            
            let mtlreg = region(cgRect: visibleNormalized)
            return screenshot(texture: texture, region: mtlreg)
            
        case .entire:
            
            let mtlreg = region(cgRect: largestVisibleNormalized)
            return screenshot(texture: texture, region: mtlreg)
            
        case .automatic, .custom:
            
            // Find the uses area inside the emulator texture
            var x1 = 0, x2 = 0, y1 = 0, y2 = 0
            emu?.videoPort.innerArea(&x1, x2: &x2, y1: &y1, y2: &y2)
            
            // Compute width and height
            var w = x2 - x1 + 1, h = y2 - y1 + 1
            
            // Scale to texture coordinates
            x1 *= 2; x2 *= 2; w *= 2; y1 *= 4; y2 *= 4; h *= 4
            
            // Compute the center
            let cx = Int((x1 + x2) / 2)
            let cy = Int((y1 + y2) / 2)
            
            // Readjust the rectangle in custom mode
            if cutout == .custom {
                
                x1 = cx - width! / 2; x2 = x1 + width!
                y1 = cy - height! / 2; y2 = y1 + height!
            }
            
            // Apply minimum width and height
            /*
             if cutout == .custom { w = 0; h = 0 }
             if w < width ?? 0 { x1 = cx - width! / 2; x2 = x1 + width! }
             if h < height ?? 0 { y1 = cy - height! / 2; y2 = y1 + height! }
             */
            
            let mtlreg = region(x1: x1, x2: x2, y1: y1, y2: y2)
            return screenshot(texture: texture, region: mtlreg)
        }
    }
    
    private func screenshot(texture: MTLTexture, region: MTLRegion) -> NSImage? {
        
        texture.blit()
        return NSImage.make(texture: texture, region: region)
    }
    
    private var framebuffer: NSImage? {
        
        guard let drawable = renderer.metalLayer.nextDrawable() else { return nil }
        
        // Create target texture
        let texture = device.makeTexture(size: drawable.texture.size, usage: [.shaderRead, .shaderWrite])!
        
        // Use the blitter to copy the framebuffer back from the GPU
        let queue = renderer.device.makeCommandQueue()!
        let commandBuffer = queue.makeCommandBuffer()!
        let blitEncoder = commandBuffer.makeBlitCommandEncoder()!
        blitEncoder.copy(from: drawable.texture,
                         sourceSlice: 0,
                         sourceLevel: 0,
                         sourceOrigin: MTLOrigin(x: 0, y: 0, z: 0),
                         sourceSize: MTLSize(width: texture.width,
                                             height: texture.height,
                                             depth: 1),
                         to: texture,
                         destinationSlice: 0,
                         destinationLevel: 0,
                         destinationOrigin: MTLOrigin(x: 0, y: 0, z: 0))
        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
        
        // Convert the texture into an NSImage
        let alpha = CGImageAlphaInfo.premultipliedFirst.rawValue
        let leEn32 = CGBitmapInfo.byteOrder32Little.rawValue
        let bitmapInfo =  CGBitmapInfo(rawValue: alpha | leEn32)
        
        return NSImage.make(texture: texture, bitmapInfo: bitmapInfo)
    }
}
