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

    /// Indicates if the SHIFT key is pressed
    var shift = false
    
    /// List holding the key characters
    var keyText =
        ["\u{2190}","1","2","3","4","5","6","7","8","9","0","+","-","\u{00a3}","HME","DEL","",
         "","Q","W","E","R","T","Y","U","I","O","P","@","*","\u{2191}","","",
         "","","A","S","D","F","G","H","J","K","L",":",";","=","","",
         "","","Z","X","C","V","B","N","M","<",">","?","","\u{21c5}","\u{21c6}","",
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
        
        // Assign images for keys that do not change appearance
        keyView[17]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_ctrl"))
        keyView[31]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_restore"))
        keyView[33]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_runstop"))
        keyView[34]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_shiftlock"))
        keyView[47]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_return"))
        keyView[49]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_commodore"))
        keyView[50]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_shift"))
        keyView[61]!.image = NSImage.init(named: NSImage.Name(rawValue: "key_shift"))

        updateImages()
    }
    
    func updateImages() {
        
        // Render images for all keys with standard symbols on it
        for nr in 0 ... 65 {
            let text = keyText[nr]
            if text != "" {
                keyImage[nr] = buttonImage(text: text as NSString, wideKey: false)
                keyView[nr]!.image = keyImage[nr]
            }
        }
        
        // Render function keys
        keyView[16]!.image = buttonImage(text: (shift ? " f 2" : " f 1") as NSString, wideKey: true)
        keyView[32]!.image = buttonImage(text: (shift ? " f 4" : " f 3") as NSString, wideKey: true)
        keyView[48]!.image = buttonImage(text: (shift ? " f 6" : " f 5") as NSString, wideKey: true)
        keyView[64]!.image = buttonImage(text: (shift ? " f 8" : " f 7") as NSString, wideKey: true)

        
        // Assign images for keys with a custom design
        let clr = NSImage.init(named: NSImage.Name(rawValue: "key_clr"))
        let home = NSImage.init(named: NSImage.Name(rawValue: "key_home"))
        let inst = NSImage.init(named: NSImage.Name(rawValue: "key_inst"))
        let del = NSImage.init(named: NSImage.Name(rawValue: "key_del"))
        let crsr_up = NSImage.init(named: NSImage.Name(rawValue: "key_crsr_up"))
        let crsr_down = NSImage.init(named: NSImage.Name(rawValue: "key_crsr_down"))
        let crsr_left = NSImage.init(named: NSImage.Name(rawValue: "key_crsr_left"))
        let crsr_right = NSImage.init(named: NSImage.Name(rawValue: "key_crsr_right"))
        keyView[14]!.image = shift ? clr : home
        keyView[15]!.image = shift ? inst : del
        keyView[62]!.image = shift ? crsr_up : crsr_down
        keyView[63]!.image = shift ? crsr_left : crsr_right
    }
    
    func buttonImage(text: NSString, wideKey: Bool) -> NSImage {
        
        // let background = NSImage(named: NSImage.Name(rawValue: wideKey ? "key_wide" : "key"))!
        let background = NSImage(named: NSImage.Name(rawValue: "key"))!
        let width = wideKey ? 48.0 : 32.0
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
