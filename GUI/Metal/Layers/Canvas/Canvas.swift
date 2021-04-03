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
        // let width = Int(NTSC_WIDTH)
        // let height = Int(PAL_HEIGHT)
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
}
