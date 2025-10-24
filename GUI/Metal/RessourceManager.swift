// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class RessourceManager {
    
    let device: MTLDevice
    let renderer: Renderer
    
    var library: MTLLibrary! = nil
 
    //
    // Textures
    //
    
    // Pixel depth information
    var depthTexture: MTLTexture! = nil
    
    // Dotmask gallery
    var dotMaskGallery = [MTLTexture?](repeating: nil, count: 5)

    // The currently selected dot mask
    var dotMask: MTLTexture! = nil
    
    // Dotmask preview images
    var dotmaskImages = [NSImage?](repeating: nil, count: 5)

    //
    // Shaders
    //
    
    // Shader galleries
    var upscalerGallery = [ComputeKernel?](repeating: nil, count: 3)
    var bloomFilterGallery = [ComputeKernel?](repeating: nil, count: 2)
    var scanlineFilterGallery = [ComputeKernel?](repeating: nil, count: 3)
    
    // The currently selected shaders
    var upscaler: ComputeKernel!
    var bloomFilter: ComputeKernel!
    var scanlineFilter: ComputeKernel!
    
    //
    // Samplers
    //
    
    // Nearest neighbor sampler
    var samplerNearest: MTLSamplerState! = nil

    // Linear interpolation sampler
    var samplerLinear: MTLSamplerState! = nil
    
    //
    // Initializing
    //
    
    init(view: MTKView, device: MTLDevice, renderer: Renderer) {
        
        self.device = device
        self.renderer = renderer
        
        buildSamplers()
        buildDotMasks()
        buildKernels()
    }
    
    internal func buildDepthBuffer() {
        
        let width = Int(renderer.size.width)
        let height = Int(renderer.size.height)
        
        precondition(width != 0)
        precondition(height != 0)
        
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.depth32Float,
            width: width,
            height: height,
            mipmapped: false)
        descriptor.resourceOptions = MTLResourceOptions.storageModePrivate
        descriptor.usage = MTLTextureUsage.renderTarget
        
        depthTexture = device.makeTexture(descriptor: descriptor)
        precondition(depthTexture != nil, "Failed to create depth texture")
    }
    
    internal func buildSamplers() {
        
        let descriptor = MTLSamplerDescriptor()
        descriptor.sAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.tAddressMode = MTLSamplerAddressMode.clampToEdge
        descriptor.mipFilter = MTLSamplerMipFilter.notMipmapped
        
        // Nearest neighbor sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.linear
        descriptor.magFilter = MTLSamplerMinMagFilter.linear
        samplerLinear = device.makeSamplerState(descriptor: descriptor)
        
        // Linear sampler
        descriptor.minFilter = MTLSamplerMinMagFilter.nearest
        descriptor.magFilter = MTLSamplerMinMagFilter.nearest
        samplerNearest = device.makeSamplerState(descriptor: descriptor)
    }
    
    internal func buildDotMasks() {
     
        let brightness = renderer.shaderOptions.dotMaskBrightness
        
        let max  = UInt8(85 + brightness * 170)
        let base = UInt8((1 - brightness) * 85)
        let none = UInt8(30 + (1 - brightness) * 55)
        
        let R = UInt32(r: max, g: base, b: base)
        let G = UInt32(r: base, g: max, b: base)
        let B = UInt32(r: base, g: base, b: max)
        let M = UInt32(r: max, g: base, b: max)
        let W = UInt32(r: max, g: max, b: max)
        let N = UInt32(r: none, g: none, b: none)
        
        let maskSize = [
            CGSize(width: 1, height: 1),
            CGSize(width: 3, height: 1),
            CGSize(width: 4, height: 1),
            CGSize(width: 3, height: 9),
            CGSize(width: 4, height: 8)
        ]
        
        let maskData = [
            
            [ W ],
            [ M, G, N ],
            [ R, G, B, N ],
            [ M, G, N,
              M, G, N,
              N, N, N,
              N, M, G,
              N, M, G,
              N, N, N,
              G, N, M,
              G, N, M,
              N, N, N],
            [ R, G, B, N,
              R, G, B, N,
              R, G, B, N,
              N, N, N, N,
              B, N, R, G,
              B, N, R, G,
              B, N, R, G,
              N, N, N, N]
        ]
        
        for n in 0 ... 4 {
            
            // Create image representation in memory
            let cap = Int(maskSize[n].width) * Int(maskSize[n].height)
            let mask = calloc(cap, MemoryLayout<UInt32>.size)!
            let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
            for i in 0 ... cap - 1 {
                ptr[i] = maskData[n][i]
            }
            
            // Create image
            let image = NSImage.make(data: mask, rect: maskSize[n])
            
            // Convert image into a texture
            dotMaskGallery[n] = image?.toTexture(device: device)
            
            // Create preview image
            dotmaskImages[n] = image?.resizeSharp(width: 12, height: 12)
        }
    }
    
    internal func buildKernels() {
        
        let uc = (TextureSize.upscaled.width, TextureSize.upscaled.height)

        // Create shader library
        library = device.makeDefaultLibrary()
        renderer.metalAssert(library != nil, "The Shader Library could not be built.")
        
        // Build upscalers
        upscalerGallery[0] = BypassUpscaler(device: device, library: library, cutout: uc)
        upscalerGallery[1] = EPXUpscaler(device: device, library: library, cutout: uc)
        upscalerGallery[2] = XBRUpscaler(device: device, library: library, cutout: uc)
        upscaler = upscalerGallery[0]
        
        // Build bloom filters
        bloomFilterGallery[0] = BypassFilter(device: device, library: library, cutout: uc)
        bloomFilterGallery[1] = SplitFilter(device: device, library: library, cutout: uc)
        bloomFilter = bloomFilterGallery[0]
        
        // Build scanline filters
        scanlineFilterGallery[0] = BypassFilter(device: device, library: library, cutout: uc)
        scanlineFilterGallery[1] = SimpleScanlines(device: device, library: library, cutout: uc)
        scanlineFilterGallery[2] = BypassFilter(device: device, library: library, cutout: uc)
        scanlineFilter = scanlineFilterGallery[0]
    }

    //
    // Selecting dot masks
    //
    
    @discardableResult
    func selectDotMask(_ nr: Int) -> Bool {
        
        if nr >= 0 && nr < dotMaskGallery.count && dotMaskGallery[nr] != nil {
            dotMask = dotMaskGallery[nr]!
            return true
        }
        return false
    }
    
    //
    // Selecting kernels
    //
    
    @discardableResult
    func selectUpscaler(_ nr: Int) -> Bool {
        
        if nr >= 0 && nr < upscalerGallery.count && upscalerGallery[nr] != nil {
            upscaler = upscalerGallery[nr]!
            return true
        }
        return false
    }
    
    @discardableResult
    func selectBloomFilter(_ nr: Int) -> Bool {
        
        if nr >= 0 && nr < bloomFilterGallery.count && bloomFilterGallery[nr] != nil {
            bloomFilter = bloomFilterGallery[nr]!
            return true
        }
        return false
    }

    @discardableResult
    func selectScanlineFilter(_ nr: Int) -> Bool {
        
        if nr >= 0 && nr < scanlineFilterGallery.count && scanlineFilterGallery[nr] != nil {
            scanlineFilter = scanlineFilterGallery[nr]!
            return true
        }
        return false
    }
    
    //
    // Accessing the library
    //
    
    func makeFunction(name: String) -> MTLFunction? {

        return library.makeFunction(name: name)
    }
}
