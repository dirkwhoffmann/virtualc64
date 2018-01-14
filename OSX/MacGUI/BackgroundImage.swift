//
//  BackgroundTexture.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

@objc public class BackgroundImage : NSObject {

    var backgroundTexture: MTLTexture!
    
    @objc convenience init(device: MTLDevice)
    {
        self.init()
        
        // By default we create a scaled down version of the desktop wallpaper
        var wallpaper = self.desktopAsImage(device: device)
        wallpaper = wallpaper?.expand(toSize: NSSize(width: 1024, height: 512))
        
        // In case of an error, we fall back to an opaque background
        if wallpaper == nil {
            wallpaper = NSImage(color: .lightGray, size: NSSize(width: 128, height: 128))
        }
        
        // Convert image to texture
        backgroundTexture = wallpaper?.toTexture(device: device)
    }
    
    @objc public func texture() -> MTLTexture {
        
        return backgroundTexture
    }
    
    func desktopAsImage(device: MTLDevice) -> NSImage? {
    
        let windows =
            CGWindowListCopyWindowInfo(CGWindowListOption.optionOnScreenOnly,
                                       CGWindowID(0))! as! [NSDictionary]
        let screenBounds = NSScreen.main?.frame
        
        // Iterate through all windows
        var nr = 0
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
                nr = window["kCGWindowNumber"] as! Int
                break
            }
        }
        
        if (nr == 0) {
            return nil
        }
        
        // Create image
         let cgImage = CGWindowListCreateImage(
            CGRect.null,
            CGWindowListOption(arrayLiteral: CGWindowListOption.optionIncludingWindow),
            CGWindowID(nr),
            [])!
         return NSImage.init(cgImage: cgImage, size: NSZeroSize)
    }
}

