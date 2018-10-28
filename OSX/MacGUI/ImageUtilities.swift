//
//  ImageUtilities.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

//
// Extensions to MTLTexture
//

extension MTLTexture {
    
    func toCGImage(_ x1: Float, _ y1: Float, _ x2: Float, _ y2: Float) -> CGImage? {
    
        let x = Int(Float(self.width) * x1)         // (x,y) : upper left corner
        let y = Int(Float(self.height) * y1)        //         of texture cutout
        let w = Int(Float(self.width) * (x2 - x1))  // (w,h) : width and height
        let h = Int(Float(self.height) * (y2 - y1)) //         of texture cutout
        let bytesPerRow = w * 4
        let size = bytesPerRow * h
        
        // Allocate memory
        guard let data = malloc(size) else { return nil; }
        
        // Fill memory with texture data
        self.getBytes(data,
                      bytesPerRow: bytesPerRow,
                      from: MTLRegionMake2D(x, y, w, h),
                      mipmapLevel: 0)
    
        // Copy data over to a new buffer of double horizontal width
        let w2 = 2 * w
        let h2 = h
        let bytesPerRow2 = 2 * bytesPerRow
        let size2 = bytesPerRow2 * h2
        guard let data2 = malloc(size2) else { return nil; }
        let ptr = data.assumingMemoryBound(to: UInt32.self)
        let ptr2 = data2.assumingMemoryBound(to: UInt32.self)
        for i in 0 ... (w * h) {
            ptr2[2 * i] = ptr[i]
            ptr2[2 * i + 1] = ptr[i]
        }
        
        let pColorSpace = CGColorSpaceCreateDeviceRGB()

        let rawBitmapInfo =
            CGImageAlphaInfo.noneSkipLast.rawValue |
                CGBitmapInfo.byteOrder32Big.rawValue
        let bitmapInfo:CGBitmapInfo = CGBitmapInfo(rawValue: rawBitmapInfo)
        
        // Create data provider
        let dealloc: CGDataProviderReleaseDataCallback
            = { (info: UnsafeMutableRawPointer?, data: UnsafeRawPointer, size: Int) -> () in
            // As far as I know, Core Foundation objects are automatically memory managed
            return
        }
        guard let provider = CGDataProvider(dataInfo: nil,
                                      data: data2,
                                      size: size2,
                                      releaseData: dealloc) else { return nil; }
        
        // Create image
        let image = CGImage(width: w2, height: h2,
                            bitsPerComponent: 8, bitsPerPixel: 32, bytesPerRow: bytesPerRow2,
                            space: pColorSpace, bitmapInfo: bitmapInfo,
                            provider: provider, decode: nil, shouldInterpolate: false,
                            intent: CGColorRenderingIntent.defaultIntent)
        
        return image
    }

    func toNSImage(_ x1: Float, _ y1: Float, _ x2: Float, _ y2: Float) -> NSImage? {
    
        guard let cgImage = self.toCGImage(x1,y1,x2,y2) else { return nil }
        
        let size = NSSize(width: cgImage.width, height: cgImage.height)
        let image = NSImage(cgImage: cgImage, size: size)
        
        return image
    }
    
}


//
// Extensions to NSImage
//

public extension NSImage {
    
    convenience init(color: NSColor, size: NSSize) {
        
        self.init(size: size)
        lockFocus()
        color.drawSwatch(in: NSRect(origin: .zero, size: size))
        unlockFocus()
    }

    func expand(toSize size: NSSize) -> NSImage? {
 
        let newImage = NSImage.init(size: size)
    
        NSGraphicsContext.saveGraphicsState()
        newImage.lockFocus()

        let t = NSAffineTransform()
        t.translateX(by: 0.0, yBy: size.height)
        t.scaleX(by: 1.0, yBy: -1.0)
        t.concat()
        
        let inRect = NSMakeRect(0,0,size.width,size.height)
        let fromRect = NSMakeRect(0,0,self.size.width, self.size.height)
        let operation = NSCompositingOperation.copy
        self.draw(in: inRect, from: fromRect, operation: operation, fraction: 1.0)
        
        newImage.unlockFocus()
        NSGraphicsContext.restoreGraphicsState()
        
        return newImage;
    }
    
    func toTexture(device: MTLDevice) -> MTLTexture? {
 
        // let imageRect = NSMakeRect(0, 0, self.size.width, self.size.height);
        let imageRef = self.cgImage(forProposedRect: nil, context: nil, hints: nil)
        
        // Create a suitable bitmap context for extracting the bits of the image
        let width = imageRef!.width
        let height = imageRef!.height
    
        if (width == 0 || height == 0) { return nil; }
        
        // Allocate memory
        guard let data = malloc(height * width * 4) else { return nil; }
        let rawBitmapInfo =
            CGImageAlphaInfo.noneSkipLast.rawValue |
                CGBitmapInfo.byteOrder32Big.rawValue
        let bitmapContext = CGContext(data: data,
                                      width: width,
                                      height: height,
                                      bitsPerComponent: 8,
                                      bytesPerRow: 4 * width,
                                      space: CGColorSpaceCreateDeviceRGB(),
                                      bitmapInfo: rawBitmapInfo)

        bitmapContext?.translateBy(x: 0.0, y: CGFloat(height))
        bitmapContext?.scaleBy(x: 1.0, y: -1.0)
        bitmapContext?.draw(imageRef!, in: CGRect.init(x: 0, y: 0, width: width, height: height))
        // CGContextDrawImage(bitmapContext!, CGRectMake(0, 0, CGFloat(width), CGFloat(height)), imageRef)
        
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: MTLPixelFormat.rgba8Unorm,
            width: width,
            height: height,
            mipmapped: false)
        let texture = device.makeTexture(descriptor: textureDescriptor)
        let region = MTLRegionMake2D(0, 0, width, height)
        texture?.replace(region: region, mipmapLevel: 0, withBytes: data, bytesPerRow: 4 * width)

        free(data);
        return texture;
    }
}


//
// Extensions to MetalView
//


public extension MetalView
{

    //
    // Image handling
    //

    func screenshot() -> NSImage?
    {
        track()
        
        /*
        return emulatorTexture.toNSImage(Float(textureRect.minX),
                                         Float(textureRect.minY),
                                         Float(textureRect.maxX),
                                         Float(textureRect.maxY))
        */
        
        // Use the blitter to copy the texture data back from the GPU
        let queue = filteredTexture.device.makeCommandQueue()!
        let commandBuffer = queue.makeCommandBuffer()!
        let blitEncoder = commandBuffer.makeBlitCommandEncoder()!
        blitEncoder.synchronize(texture: filteredTexture, slice: 0, level: 0)
        blitEncoder.endEncoding()
        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
        
        return filteredTexture.toNSImage(Float(textureRect.minX),
                                         Float(textureRect.minY),
                                         Float(textureRect.maxX),
                                         Float(textureRect.maxY))
    }
    
    func createBackgroundTexture() -> MTLTexture? {

        // 1. step: Grab the current wallpaper as an NSImage
        let windows =
            CGWindowListCopyWindowInfo(CGWindowListOption.optionOnScreenOnly,
                                       CGWindowID(0))! as! [NSDictionary]
        let screenBounds = NSScreen.main?.frame
        
        // Iterate through all windows
        var cgImage: CGImage?
        for i in 0 ..< windows.count {
            
            let window = windows[i]
            
            // Skip all windows that are not owned by the dock
            let owner = window["kCGWindowOwnerName"] as? String
            if owner != "Dock" {
                continue
            }
            
            // Skip all windows that do not have the same bounds as the main screen
            let bounds = window["kCGWindowBounds"] as! NSDictionary
            let width  = bounds["Width"] as! CGFloat
            let height = bounds["Height"] as! CGFloat
            if (width != screenBounds?.width || height != screenBounds?.height) {
                continue
            }
            
            // Skip all windows without having a name
            guard let name = window["kCGWindowName"] as? String else {
                continue
            }
                
            // Skip all windows with a name other than "Desktop picture - ..."
            if name.hasPrefix("Desktop Picture") {
                
                // Found it!
                cgImage = CGWindowListCreateImage(
                    CGRect.null,
                    CGWindowListOption(arrayLiteral: CGWindowListOption.optionIncludingWindow),
                    CGWindowID(window["kCGWindowNumber"] as! Int),
                    [])!
                break
            }
        }
        
        // Create image
        var wallpaper: NSImage?
        if cgImage != nil {
            wallpaper = NSImage.init(cgImage: cgImage!, size: NSZeroSize)
            wallpaper = wallpaper?.expand(toSize: NSSize(width: 1024, height: 512))
        } else {
            // Fall back to an opaque gray background
            let size = NSSize(width: 128, height: 128)
            wallpaper = NSImage(color: .lightGray, size: size)
        }
        
        // Return image as texture
        return wallpaper?.toTexture(device: device!)
    }
}
