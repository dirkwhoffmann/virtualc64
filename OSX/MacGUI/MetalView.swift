//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//
// TODO:
// eyeX,eyeY,eyeZ -> eye : float3

import Foundation
import Metal
import MetalKit
// import MetalPerformanceShaders

struct Sizeof {
    static let float = 4
    static let matrix4x4 = 16 * 4
}

struct C64Texture {
    static let orig = NSSize.init(width: 512, height: 512)
    static let upscaled = NSSize.init(width: 2048, height: 2048)
}

public class MetalView: MTKView {
    
    @IBOutlet weak var controller: MyController!
    
    /// Number of drawn frames since power up
    var frames: UInt64 = 0
    
    // Synchronization semaphore
    var semaphore: DispatchSemaphore!
    
    // Metal objects
    var library: MTLLibrary! = nil
    var queue: MTLCommandQueue! = nil
    var pipeline: MTLRenderPipelineState! = nil
    var depthState: MTLDepthStencilState! = nil
    var commandBuffer: MTLCommandBuffer! = nil
    var commandEncoder: MTLRenderCommandEncoder! = nil
    var drawable: CAMetalDrawable! = nil
    
    // Metal layer
    var metalLayer: CAMetalLayer! = nil
    var layerWidth = CGFloat(0.0)
    var layerHeight = CGFloat(0.0)
    var layerIsDirty = true
    
    // Buffers
    var positionBuffer: MTLBuffer! = nil
    var uniformBuffer2D: MTLBuffer! = nil
    var uniformBuffer3D: MTLBuffer! = nil
    var uniformBufferBg: MTLBuffer! = nil
    var uniformFragment: MTLBuffer! = nil
    
    // Textures
    
    /// Background image behind the cube
    var bgTexture: MTLTexture! = nil
    
    /// Raw texture data provided by the emulator
    /// Texture is updated in updateTexture which is called periodically in
    /// drawRect
    var emulatorTexture: MTLTexture! = nil
    
    /// Upscaled emulator texture
    /// In the first post-processing stage, the emulator texture is bumped up
    /// by factor 4. The user can choose between bypass upscaling which simply
    /// replaces each pixel by a 4x4 quad or more sophisticated upscaling
    /// algorithms such as xBr.
    var upscaledTexture: MTLTexture! = nil
    
    /// Filtered emulator texture
    /// In the second post-processing stage, the upscaled texture is blurred.
    /// The user can choose between bypass blurring which simply copies the
    /// pixels as they are or real blurring algorithm. To achieve high
    /// performance, blurring is done via Metals High Performance Shader
    /// framework.
    var filteredTexture: MTLTexture! = nil
    
    // Texture to hold the pixel depth information
    var depthTexture: MTLTexture! = nil

    // Array holding all available upscalers
    var upscalers = [ComputeKernel?](repeating: nil, count: 3)
 
    // Array holding all available filters
    var filters = [ComputeKernel?](repeating: nil, count: 5)
    
    // Shader parameters
    var scanlines = EmulatorDefaults.scanlines
    var scanlineBrightness = EmulatorDefaults.scanlineBrightness
    var scanlineWeight = EmulatorDefaults.scanlineWeight
    var bloomFactor = EmulatorDefaults.bloomFactor
    var dotMask = EmulatorDefaults.dotMask
    var maskBrightness = EmulatorDefaults.maskBrightness
    
    var blurFactor = EmulatorDefaults.blur {
        didSet {
            if (1 < filters.count && filters[1] != nil) {
                let gaussFilter = filters[1] as! GaussFilter
                gaussFilter.sigma = blurFactor
            }
        }
    }
    
    // Animation parameters
    var currentXAngle = Float(0.0)
    var targetXAngle = Float(0.0)
    var deltaXAngle = Float(0.0)
    var currentYAngle = Float(0.0)
    var targetYAngle = Float(0.0)
    var deltaYAngle = Float(0.0)
    var currentZAngle = Float(0.0)
    var targetZAngle = Float(0.0)
    var deltaZAngle = Float(0.0)
    var currentEyeX = Float(0.0)
    var targetEyeX = Float(0.0)
    var deltaEyeX = Float(0.0)
    var currentEyeY = Float(0.0)
    var targetEyeY = Float(0.0)
    var deltaEyeY = Float(0.0)
    var currentEyeZ = Float(0.0)
    var targetEyeZ = Float(0.0)
    var deltaEyeZ = Float(0.0)
    var currentAlpha = Float(0.0)
    var targetAlpha = Float(0.0)
    var deltaAlpha = Float(0.0)
        
    /// Texture cut-out (normalized)
    var textureRect = CGRect.init(x: 0.0, y: 0.0, width: 0.0, height: 0.0)
 
    /// Currently selected texture upscaler
    var videoUpscaler = EmulatorDefaults.upscaler {
        didSet {
            if videoUpscaler >= upscalers.count || upscalers[videoUpscaler] == nil {
                track("Sorry, the selected GPU upscaler is unavailable.")
                videoUpscaler = 0
            }
        }
    }

    // Currently selected texture filter
    var videoFilter = EmulatorDefaults.filter {
        didSet {
            if videoFilter >= filters.count || filters[videoFilter] == nil {
                track("Sorry, the selected GPU filter is unavailable.")
                videoFilter = 0
            }
        }
    }
    
    //! If true, no GPU drawing is performed (for performance profiling olny)
    var enableMetal = false
    
    //! Is set to true when fullscreen mode is entered (usually enables the 2D renderer)
    var fullscreen = false
    
    //! If true, the 3D renderer is also used in fullscreen mode
    var fullscreenKeepAspectRatio = true
    
    //! If false, the C64 screen is not drawn (background texture will be visible)
    var drawC64texture = false
        
    required public init(coder: NSCoder) {
    
        super.init(coder: coder)
    }
    
    required public override init(frame frameRect: CGRect, device: MTLDevice?) {
        
        super.init(frame: frameRect, device: device)
    }
    
    override open func awakeFromNib() {

        track()
        
        // Create semaphore
        semaphore = DispatchSemaphore(value: 1);
        
        // Check if machine is capable to run the Metal graphics interface
        checkForMetal()
    
        // Register for drag and drop
        setupDragAndDrop()
    }
    
    override public var acceptsFirstResponder: Bool
    {
        get { return true }
    }
    
    //! Adjusts view height by a certain number of pixels
    func adjustHeight(_ height: CGFloat) {
    
        var newFrame = frame
        newFrame.origin.y -= height
        newFrame.size.height += height
        frame = newFrame
    }
    
    //! Shrinks view vertically by the height of the status bar
    public func shrink() { adjustHeight(-24.0) }
    
    //! Expand view vertically by the height of the status bar
    public func expand() { adjustHeight(24.0) }

    public func updateScreenGeometry() {
    
        var rect: CGRect
        
        if controller.c64.vic.isPAL() {
    
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
        
        textureRect = CGRect.init(x: rect.minX / C64Texture.orig.width,
                                  y: rect.minY / C64Texture.orig.height,
                                  width: rect.width / C64Texture.orig.width,
                                  height: rect.height / C64Texture.orig.height)
        
        // Enable this for debugging (will display the whole texture)
        // textureXStart = 0.0;
        // textureXEnd = 1.0;
        // textureYStart = 0.0;
        // textureYEnd = 1.0;
    
        // Update texture coordinates in vertex buffer
        buildVertexBuffer()
    }
    
    func updateTexture() {
    
        /*
        if c64proxy == nil {
            return
        }
        */
        
        let buf = controller.c64.vic.screenBuffer()
        precondition(buf != nil)
        
        let pixelSize = 4
        let width = Int(NTSC_PIXELS)
        let height = Int(PAL_RASTERLINES)
        let rowBytes = width * pixelSize
        let imageBytes = rowBytes * height
        let region = MTLRegionMake2D(0,0,width,height)
            
        emulatorTexture.replace(region: region,
                                mipmapLevel: 0,
                                slice: 0,
                                withBytes: buf!,
                                bytesPerRow: rowBytes,
                                bytesPerImage: imageBytes)
    }
    
    //! Returns the compute kernel of the currently selected pixel upscaler
    func currentUpscaler() -> ComputeKernel {
    
        precondition(videoUpscaler < upscalers.count)
        precondition(upscalers[0] != nil)
        
        return upscalers[videoUpscaler]!
    }
    
    //! Returns the compute kernel of the currently selected texture filer
    func currentFilter() -> ComputeKernel {
        
        precondition(videoFilter < filters.count)
        precondition(filters[0] != nil)
        
        return filters[videoFilter]!
    }
    
    func startFrame() {
    
        commandBuffer = queue.makeCommandBuffer()
        precondition(commandBuffer != nil, "Command buffer must not be nil")
    
        // Set uniforms
        fillFragmentShaderUniforms(uniformFragment)
        
        // Upscale the C64 texture
        let upscaler = currentUpscaler()
        upscaler.apply(commandBuffer: commandBuffer,
                       source: emulatorTexture,
                       target: upscaledTexture)
    
        // Filter the upscaled texture (apply blur effect)
        let filter = currentFilter()
        filter.apply(commandBuffer: commandBuffer,
                     source: upscaledTexture,
                     target: filteredTexture)
       
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
        commandEncoder.setFragmentTexture(bgTexture, index: 0)
        commandEncoder.setFragmentSamplerState(filter.getsampler(), index: 0)
        commandEncoder.setFragmentBuffer(uniformFragment, offset: 0, index: 0)
        commandEncoder.setVertexBuffer(positionBuffer, offset: 0, index: 0)
    }
    
    func drawScene2D() {
    
        startFrame()
    
        // Render quad
        commandEncoder.setFragmentTexture(filteredTexture, index: 0)
        commandEncoder.setVertexBuffer(uniformBuffer2D, offset: 0, index: 1)
        commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                      vertexStart: 42,
                                      vertexCount: 6,
                                      instanceCount: 1)
        endFrame()
    }
    
    func drawScene3D() {
    
        let animates = self.animates()
        let drawBackground = !fullscreen && (animates || !drawC64texture)
        
        if animates {
            updateAngles()
            buildMatrices3D()
        }

        startFrame()
    
        // Make texture transparent if emulator is halted
        let alpha = controller.c64.isHalted() ? 0.5 : currentAlpha
        fillAlpha(uniformBuffer3D, alpha)
        
        // Render background
        if drawBackground {
            commandEncoder.setFragmentTexture(bgTexture, index: 0)
            commandEncoder.setVertexBuffer(uniformBufferBg, offset: 0, index: 1)
            commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                          vertexStart: 0,
                                          vertexCount: 6,
                                          instanceCount: 1)
        }
        
        // Render cube
        if drawC64texture {
            commandEncoder.setFragmentTexture(filteredTexture, index: 0)
            commandEncoder.setVertexBuffer(uniformBuffer3D, offset: 0, index: 1)
            commandEncoder.drawPrimitives(type: MTLPrimitiveType.triangle,
                                          vertexStart: 6,
                                          vertexCount: (animates ? 24 : 6),
                                          instanceCount: 1)
        }
                
        endFrame()
    }

    func endFrame() {
    
        commandEncoder.endEncoding()
    
        commandBuffer.addCompletedHandler { cb in
            self.semaphore.signal()
        }
        
        if (drawable != nil) {
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }
        
        frames += 1
    }
    
    override public func setFrameSize(_ newSize: NSSize) {
        
        super.setFrameSize(newSize)
        layerIsDirty = true
    }
    
    func reshape(withFrame frame: CGRect) {
    
        reshape();
        /*
        if let scale = NSScreen.main?.backingScaleFactor {
            
            var size = bounds.size
            size.width *= scale
            size.height *= scale
            
            metalLayer.drawableSize = drawableSize
            reshape()
        }
        */
    }

    func reshape() {

        let drawableSize = metalLayer.drawableSize
        
        if layerWidth == drawableSize.width && layerHeight == drawableSize.height {
            return
        }
    
        layerWidth = drawableSize.width
        layerHeight = drawableSize.height
    
        // NSLog("MetalLayer::reshape (%f,%f)", drawableSize.width, drawableSize.height);
    
        // Rebuild matrices
        buildMatricesBg()
        buildMatrices2D()
        buildMatrices3D()
    
        // Rebuild depth buffer
        buildDepthBuffer()
    }
    
    override public func draw(_ rect: NSRect) {
        
        if !enableMetal {
            return
        }

        // Wait until it's save to go ...
        // let result semaphore.wait (timeout: .distantFuture)
        semaphore.wait()
        
        // Refresh size dependent items if needed
        if layerIsDirty {
            reshape(withFrame: frame)
            layerIsDirty = false
        }
    
        // Draw scene
        drawable = metalLayer.nextDrawable()
        if (drawable != nil) {
            updateTexture()
            if fullscreen && !fullscreenKeepAspectRatio {
                drawScene2D()
            } else {
                drawScene3D()
            }
        }
    }
   
    public func cleanup() {
    
        track()
    }
    
}

