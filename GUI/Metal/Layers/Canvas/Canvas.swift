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
    
    var ressourceManager: RessourceManager { return renderer.ressourceManager }
    var bloomFilter: ComputeKernel! { return ressourceManager.bloomFilter }
    var upscaler: ComputeKernel! { return ressourceManager.upscaler }
    var scanlineFilter: ComputeKernel! { return ressourceManager.scanlineFilter }

    //
    // Textures
    //

    // Background texture
    var bgTexture: MTLTexture! = nil
    var bgFullscreenTexture: MTLTexture! = nil
    
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
    
    //
    // Uniforms
    //
    
    var fragmentUniforms = FragmentUniforms(alpha: 1.0,
                                            dotMaskWidth: 0,
                                            dotMaskHeight: 0,
                                            scanlineDistance: 0)

    //
    // Initializing
    //
    
    override init(renderer: Renderer) {

        super.init(renderer: renderer)
        buildTextures()
        
        // Start with a negative alpha to keep the splash screen for a while
        alpha.set(-2.5)
    }
    
    //
    // Updating textures
    //
        
    func updateBgTexture(bytes: UnsafeMutablePointer<UInt32>) {
        
        bgTexture.replace(w: 512, h: 512, buffer: bytes)
    }
    
    func updateTexture() {
        
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
    
    func render2D(encoder: MTLRenderCommandEncoder) {
        
        // Configure vertex shader
        encoder.setVertexBytes(&renderer.vertexUniforms2D,
                               length: MemoryLayout<VertexUniforms>.stride,
                               index: 1)
        
        // Configure fragment shader
        fragmentUniforms.alpha = 1.0
        fragmentUniforms.dotMaskHeight = Int32(ressourceManager.dotMask.height)
        fragmentUniforms.dotMaskWidth = Int32(ressourceManager.dotMask.width)
        fragmentUniforms.scanlineDistance = Int32(renderer.size.height / 256)
        encoder.setFragmentTexture(scanlineTexture, index: 0)
        encoder.setFragmentTexture(bloomTextureR, index: 1)
        encoder.setFragmentTexture(bloomTextureG, index: 2)
        encoder.setFragmentTexture(bloomTextureB, index: 3)
        encoder.setFragmentTexture(ressourceManager.dotMask, index: 4)
        encoder.setFragmentBytes(&fragmentUniforms,
                                 length: MemoryLayout<FragmentUniforms>.stride,
                                 index: 1)
        
        // Draw
        renderer.quad2D!.drawPrimitives(encoder)
    }
    
    func render3D(encoder: MTLRenderCommandEncoder) {
        
        let paused = c64.paused
        let poweredOff = c64.poweredOff
        let renderBackground = poweredOff || renderer.animates != 0 || renderer.fullscreen
        let renderForeground = renderer.alpha.current > 0.0
        
        // Perform a single animation step
        if renderer.animates != 0 { renderer.performAnimationStep() }
        
        if renderBackground {
            
            // Update background texture
            if !renderer.fullscreen {
                let buffer = c64.vic.noise()
                updateBgTexture(bytes: buffer!)
            }
            
            // Configure vertex shader
            encoder.setVertexBytes(&renderer.vertexUniformsBg,
                                   length: MemoryLayout<VertexUniforms>.stride,
                                   index: 1)
            
            // Configure fragment shader
            if renderer.fullscreen {
                fragmentUniforms.alpha = 1.0
                encoder.setFragmentTexture(bgFullscreenTexture, index: 0)
                encoder.setFragmentTexture(bgFullscreenTexture, index: 1)
            } else {
                fragmentUniforms.alpha = renderer.noise.current
                encoder.setFragmentTexture(bgTexture, index: 0)
                encoder.setFragmentTexture(bgTexture, index: 1)
            }
            encoder.setFragmentBytes(&fragmentUniforms,
                                     length: MemoryLayout<FragmentUniforms>.stride,
                                     index: 1)
            
            // Draw
            renderer.bgRect!.drawPrimitives(encoder)
        }
        
        if renderForeground {
            
            // Configure vertex shader
            encoder.setVertexBytes(&renderer.vertexUniforms3D,
                                   length: MemoryLayout<VertexUniforms>.stride,
                                   index: 1)
            
            // Configure fragment shader
            fragmentUniforms.alpha = paused ? 0.5 : renderer.alpha.current
            fragmentUniforms.dotMaskHeight = Int32(ressourceManager.dotMask.height)
            fragmentUniforms.dotMaskWidth = Int32(ressourceManager.dotMask.width)
            fragmentUniforms.scanlineDistance = Int32(renderer.size.height / 256)
            encoder.setFragmentTexture(scanlineTexture, index: 0)
            encoder.setFragmentTexture(bloomTextureR, index: 1)
            encoder.setFragmentTexture(bloomTextureG, index: 2)
            encoder.setFragmentTexture(bloomTextureB, index: 3)
            encoder.setFragmentTexture(ressourceManager.dotMask, index: 4)
            encoder.setFragmentBytes(&fragmentUniforms,
                                     length: MemoryLayout<FragmentUniforms>.stride,
                                     index: 1)
            
            // Draw (part of) cube
            renderer.quad3D!.draw(encoder, allSides: renderer.animates != 0)
        }
    }
}
