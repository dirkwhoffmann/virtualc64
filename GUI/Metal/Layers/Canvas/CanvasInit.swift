// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Canvas {
    
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
        
        // Background texture used in window mode
        bgTexture = device.makeTexture(size: TextureSize.background, usage: r)
        assert(bgTexture != nil, "Failed to create bgTexture")
        
        // Background texture used in fullscreen mode
        let c1 = (0x00, 0x00, 0x00, 0xFF)
        let c2 = (0x44, 0x44, 0x44, 0xFF)
        bgFullscreenTexture = device.makeTexture(size: TextureSize.background, gradient: [c1, c2], usage: r)
        assert(bgFullscreenTexture != nil, "Failed to create bgFullscreenTexture")

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
}
