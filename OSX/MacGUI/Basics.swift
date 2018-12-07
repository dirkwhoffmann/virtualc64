//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//


import Foundation


//
// Logging / Debugging
// 

public func track(_ message: String = "",
                  path: String = #file, function: String = #function, line: Int = #line ) {
    
    if let file = URL.init(string: path)?.deletingPathExtension().lastPathComponent {
        if (message == "") {
            print("\(file).\(line)::\(function)")
        } else {
            print("\(file).\(line)::\(function): \(message)")
        }
    }
}


//
// Handling URLs
//

extension URL {
    
    /*
     func makeWithTimeStamp(path: String, name: String, suffix: String) -> URL? {
     
     func makeWithUniqueTimeStamp(path: String, name: String, suffix: String) -> URL? {
     
     func make(path: String, name: String, suffix: String) -> URL? {
        
        var number = ""
        
        let pathURL = NSURL.init(fileURLWithPath: path)
        let url = pathURL.appendingPathComponent(name + suffix)
        return url
    }
    */
}


//
// Handling images
//

/*
extension NSBitmapImageRep {
    var png: Data? {
        return representation(using: .png, properties: [:])
    }
    var jpg: Data? {
        return representation(using: .jpeg, properties: [:])
    }
}
*/

extension Data {
    var bitmap: NSBitmapImageRep? {
        return NSBitmapImageRep(data: self)
    }
}
extension NSImage {
    
    /*
    var pngRepresentation: Data? {
        return tiffRepresentation?.bitmap?.png
    }
    var jpgRepresentation: Data? {
        return tiffRepresentation?.bitmap?.jpg
    }
    */
    
    func representation(using: NSBitmapImageRep.FileType) -> Data? {
        
        let bitmap = tiffRepresentation?.bitmap
        return bitmap?.representation(using: using, properties: [:])
    }
    
    func resizeImage(width: CGFloat, height: CGFloat,
                     cutout: NSRect,
                     interpolation: NSImageInterpolation = .high) -> NSImage {
        
        let img = NSImage(size: CGSize(width:width, height:height))
        
        img.lockFocus()
        let ctx = NSGraphicsContext.current
        ctx?.imageInterpolation = interpolation // NSImageInterpolation.none // .high
        self.draw(in: cutout,
                  from: NSMakeRect(0, 0, size.width, size.height),
                  operation: .copy,
                  fraction: 1)
        img.unlockFocus()
        
        return img
    }
    
    func resizeImage(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSMakeRect(0, 0, width, height)
        return resizeImage(width: width, height: height,
                           cutout: cutout)
    }

    func resizeImageSharp(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSMakeRect(0, 0, width, height)
        return resizeImage(width: width, height: height,
                           cutout: cutout,
                           interpolation: .none)
    }
    
    func makeGlossy() {
        
        let width  = size.width
        let height = size.height
        let glossy = NSImage(named: NSImage.Name(rawValue: "glossy"))
        let rect   = NSRect(x: 0, y: 0, width: width, height: height)
        
        lockFocus()
        let sourceOver = NSCompositingOperation.sourceOver
        draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        glossy!.draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        unlockFocus()
    }
}


//
// C64 Proxy extensions
//

public extension C64Proxy {
    
    func image(data: UnsafeMutablePointer<UInt8>?, size: NSSize) -> NSImage {
        
        var bitmap = data
        let width = Int(size.width)
        let height = Int(size.height)
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &bitmap,
                                        pixelsWide: width,
                                        pixelsHigh: height,
                                        bitsPerSample: 8,
                                        samplesPerPixel: 4,
                                        hasAlpha: true,
                                        isPlanar: false,
                                        colorSpaceName: NSColorSpaceName.calibratedRGB,
                                        bytesPerRow: 4*width,
                                        bitsPerPixel: 32)
        let image = NSImage(size: (imageRep?.size)!)
        image.addRepresentation(imageRep!)
        image.makeGlossy()
        
        return image
    }
    
    func autoSnapshotImage(_ item: Int) -> NSImage {
        
        let data = autoSnapshotImageData(item)
        return image(data: data, size: autoSnapshotImageSize(item))
    }

    func userSnapshotImage(_ item: Int) -> NSImage {
        
        let data = userSnapshotImageData(item)
       return image(data: data, size: userSnapshotImageSize(item))
    }
}
