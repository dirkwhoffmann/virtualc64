//
//  Basics.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 03.02.18.
//

import Foundation


// -------------------------------------------------------------------
//                          Logging / Debugging
// -------------------------------------------------------------------

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

// -------------------------------------------------------------------
//                         Drag and Drop
// -------------------------------------------------------------------

/*
struct DragType {
    static let string = NSPasteboard.PasteboardType.string
    static let contents = NSPasteboard.PasteboardType.fileContents
    static let file = NSPasteboard.PasteboardType.fileURL
}
*/

// -------------------------------------------------------------------
//                         Image processing
// -------------------------------------------------------------------

extension NSImage {
    
    func resizeImage(width: CGFloat, height: CGFloat, cutout: NSRect) -> NSImage {
        
        let img = NSImage(size: CGSize(width:width, height:height))
        
        img.lockFocus()
        let ctx = NSGraphicsContext.current
        ctx?.imageInterpolation = .high
        self.draw(in: cutout,
                  from: NSMakeRect(0, 0, size.width, size.height),
                  operation: .copy,
                  fraction: 1)
        img.unlockFocus()
        
        return img
    }
    
    func resizeImage(width: CGFloat, height: CGFloat) -> NSImage {
        
        let cutout = NSMakeRect(0, 0, width, height)
        return resizeImage(width: width, height: height, cutout: cutout)
    }
    
    func makeGlossy() {
        
        let width  = size.width
        let height = size.height
        let glossy = NSImage(named: NSImage.Name(rawValue: "glossy.png"))
        let rect   = NSRect(x: 0, y: 0, width: width, height: height)
        
        lockFocus()
        let sourceOver = NSCompositingOperation.sourceOver
        //         draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        //         glossy!.draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        glossy!.draw(in: rect, from: NSZeroRect, operation: sourceOver, fraction: 1.0)
        unlockFocus()
    }
}

// -------------------------------------------------------------------
//                          C64 Proxy extensions
// -------------------------------------------------------------------

public extension C64Proxy {
    
    func image(data: UnsafeMutablePointer<UInt8>?, width: Int, height: Int) -> NSImage {
        
        var bitmap = data
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
    @objc func autoSnapshotImage(_ item: Int) -> NSImage {
        
        let data = autoSnapshotImageData(item)
        let width = autoSnapshotImageWidth(item)
        let height = autoSnapshotImageHeight(item)
        return image(data: data, width: width, height: height)
    }

    @objc func userSnapshotImage(_ item: Int) -> NSImage {
        
        let data = userSnapshotImageData(item)
        let width = userSnapshotImageWidth(item)
        let height = userSnapshotImageHeight(item)
        return image(data: data, width: width, height: height)
    }

}

/*
public extension SnapshotProxy {
    
    @objc func image() -> NSImage {
        
        var data = imageData()
        let width = imageWidth()
        let height = imageHeight()
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &data,
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
}
*/
