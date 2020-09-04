// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct Keycap {
    
    var label1 = ""
    var label2 = ""
    var font = ""
    var size = ""
    var dark = false
    
    var stacked: Bool { return label2 != "" }
    
    var image: NSImage?
        
    init(_ label1: String, _ label2: String = "",
         font: String = "", size: String = "100", dark: Bool = false) {
        
        track()
        
        self.label1 = label1
        self.label2 = label2
        self.font = font
        self.size = size
        self.dark = dark
        
        if label1 != "" { computeImage() }
    }
    
    mutating func computeImage() {
                
        track()
        
        // Key label font sizes
        let large = CGFloat(15)
        let small = CGFloat(10)
        let tiny  = CGFloat(9)
        
        // Start with a background image
        image = NSImage(named: "shape" + size + "x100")?.copy() as? NSImage
        if dark { image?.darken() }

        /*
         if fontname == "" {
         font = NSFont.systemFont(ofSize: 13)
         yoffset = -6
         } else {
         font = NSFont.init(name: "C64ProMono", size: 9)!
         yoffset = -9
         }
         */
                
        let xoffset = (label1 == "RESTORE") ? CGFloat(-6) :  CGFloat(0)

        if stacked {
            
            // Generate a stacked label
            let size = tiny
            image?.imprint(text: label1, x: 6, y: 2, fontSize: size)
            image?.imprint(text: label2, x: 6, y: 9, fontSize: size)
            
        } else {
            
            // Generate a standard label
            let size = (label1.count == 1) ? large : small
            image?.imprint(text: label1, x: 8 + xoffset, y: 2, fontSize: size)
        }
    }
}
