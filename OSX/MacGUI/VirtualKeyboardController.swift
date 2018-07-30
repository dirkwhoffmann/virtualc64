//
//  VirtualKeyboardController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 30.07.18.
//

import Foundation

class VirtualKeyboardController : NSWindowController
{
    /// Reference to the parent controller
    var parent: MyController!
    
    /// Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    /// Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    /// List holding the key characters
    var keyText = ["\u{2190}","1","2","3","4","5","6","7","8","9","0","+","-","\u{00a3}","HME","DEL","F1",
                   "Ctrl","Q","W","E","R","T","Y","U","I","O","P","@","*","","Restore","F3",
                   "","","A","S","D","F","G","H","J","K","L",":",";","=","\u{21b5}","F5",
                   "","\u{21e7}","Z","X","C","V","B","N","M","<",">","?","\u{21e7}","\u{21c5}","\u{21c6}","F7",
                   ""]
    
    override open func showWindow(_ sender: Any?) {
        
        parent = sender as! MyController
        super.showWindow(sender)
    }
    
    override func windowDidLoad() {
        
        // Setup key references
        for tag in 0 ... 65 {
            keyView[tag] = window!.contentView!.viewWithTag(tag) as? NSButton
        }
        
        updateImages()
    }
    
    func updateImages() {
        
        // Clear old images
        for nr in 0 ... 65 {
            keyImage[nr] = nil
        }
        
        // Create images for all mapped keys
        for nr in 0 ... 65 {
            let text = keyText[nr]
            keyImage[nr] = buttonImage(text: text as NSString)
            keyView[nr]?.image = keyImage[nr]
        }
    }
    
    func buttonImage(text: NSString) -> NSImage {
        
        let background = NSImage(named: NSImage.Name(rawValue: "key"))!
        let width = 32.0
        let height = 32.0
        let imageRect = CGRect(x: 0, y: 0, width: width, height: height)
        let textRect1 = CGRect(x: 11, y: -4, width: width-7, height: height-2)
        // let textRect2 = CGRect(x: 14, y: -10, width: width-14, height: height-10)
        let textStyle = NSMutableParagraphStyle.default.mutableCopy() as! NSMutableParagraphStyle
        let font1 = NSFont.systemFont(ofSize: 15)
        // let font2 = NSFont.systemFont(ofSize: 16)
        let textFontAttributes1 = [
            NSAttributedStringKey.font: font1,
            NSAttributedStringKey.foregroundColor: NSColor.black,
            NSAttributedStringKey.paragraphStyle: textStyle
        ]
        /*
        let textFontAttributes2 = [
            NSAttributedStringKey.font: font2,
            NSAttributedStringKey.foregroundColor: NSColor.black,
            NSAttributedStringKey.paragraphStyle: textStyle
        ]
         */
        let outImage = NSImage(size: NSSize.init(width: width, height: height))
        let rep:NSBitmapImageRep = NSBitmapImageRep(bitmapDataPlanes: nil,
                                                    pixelsWide: Int(width),
                                                    pixelsHigh: Int(height),
                                                    bitsPerSample: 8,
                                                    samplesPerPixel: 4,
                                                    hasAlpha: true,
                                                    isPlanar: false,
                                                    colorSpaceName: NSColorSpaceName.calibratedRGB,
                                                    bytesPerRow: 0,
                                                    bitsPerPixel: 0)!
        outImage.addRepresentation(rep)
        outImage.lockFocus()
        background.draw(in: imageRect)
        text.draw(in: textRect1, withAttributes: textFontAttributes1)
        outImage.unlockFocus()
        return outImage
    }
    
    @IBAction func pressVirtualC64Key(_ sender: Any!) {
        
        let tag = (sender as! NSButton).tag
        track("Key = \(tag)")
    }
        
}
