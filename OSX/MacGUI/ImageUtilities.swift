//
//  ImageUtilities.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

// --------------------------------------------------------------------------------
//                            Extensions to MTLTexture
// --------------------------------------------------------------------------------

extension MTLTexture {
    
    func toCGImage(_ x1: Float, _ y1: Float, _ x2: Float, _ y2: Float) -> CGImage? {
    
        let x = Int(Float(self.width) * x1)         // (x,y) : upper left corner
        let y = Int(Float(self.height) * y1)        //         of texture cutout
        let w = Int(Float(self.width) * (x2 - x1))  // (w,h) : width and height
        let h = Int(Float(self.height) * (y2 - y1)) //         of texture cutout
        let bytesPerRow = w * 4
        
        // Allocate memory
        guard let data = malloc(bytesPerRow * h) else { return nil; }
        
        // Fill memory with texture data
        self.getBytes(data,
                      bytesPerRow: bytesPerRow,
                      from: MTLRegionMake2D(x, y, w, h),
                      mipmapLevel: 0)
    
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
                                      data: data,
                                      size: bytesPerRow * h,
                                      releaseData: dealloc) else { return nil; }
        
        // Create image
        let image = CGImage(width: w, height: h,
                            bitsPerComponent: 8, bitsPerPixel: 32, bytesPerRow: bytesPerRow,
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

// --------------------------------------------------------------------------------
//                              Extensions to NSImage
// --------------------------------------------------------------------------------

public extension NSImage {
    
    convenience init(color: NSColor, size: NSSize) {
        
        self.init(size: size)
        lockFocus()
        color.drawSwatch(in: NSRect(origin: .zero, size: size))
        unlockFocus()
    }

    @objc func expand(toSize size: NSSize) -> NSImage? {
 
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
    
    @objc func toTexture(device: MTLDevice) -> MTLTexture? {
 
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


// --------------------------------------------------------------------------------
//                            Extensions to MyMetalView
// --------------------------------------------------------------------------------


public extension MetalView
{

    // --------------------------------------------------------------------------------
    //                                 Image handling
    // --------------------------------------------------------------------------------

    
    @objc func screenshot() -> NSImage?
    {
        print("MyMetalViewHelper::screenshot")
        
        return emulatorTexture.toNSImage(Float(textureRect.minX),
                                         Float(textureRect.minY),
                                         Float(textureRect.maxX),
                                         Float(textureRect.maxY))
    }
    
    @objc func createBackgroundTexture() -> MTLTexture? {

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
            let owner = window["kCGWindowOwnerName"] as! String
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
            
            // Skip all windows with a name other than "Desktop picture - ..."
            let name = window["kCGWindowName"] as! String
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
