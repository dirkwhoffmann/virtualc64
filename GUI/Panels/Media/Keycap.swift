// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct Keycap {
    
    // Fonts
    static let proMono = NSFont(name: "C64ProMono", size: 9)
    static let systemLarge = NSFont.systemFont(ofSize: 15)
    static let systemSmall = NSFont.systemFont(ofSize: 9)

    var label1 = ""
    var label2 = ""
    var font = ""
    var shape = ""
    var dark = false
    
    var stacked: Bool { return label2 != "" }
    
    var image: NSImage?
        
    init(_ label1: String, _ label2: String = "",
         font: String = "", shape: String = "100x100", dark: Bool = false) {
                
        self.label1 = label1
        self.label2 = label2
        self.font = font
        self.shape = shape
        self.dark = dark
        
        computeImage()
    }
    
    mutating func computeImage() {
                                
        // Start with a background image
        image = NSImage(named: "shape" + shape)?.copy() as? NSImage
        if dark { image?.darken() }
                        
        if stacked {
            
            // Generate a stacked label
            image?.imprint(label1, dx: 0, dy: 5, font: Keycap.systemSmall)
            image?.imprint(label2, dx: 0, dy: -5, font: Keycap.systemSmall)
            
        } else {
            
            // Generate a standard label
            if font == "C64" {
                image?.imprint(label1, dx: 0, dy: 0, font: Keycap.proMono!)
            } else if label1.count == 1 {
                image?.imprint(label1, dx: 0, dy: 0, font: Keycap.systemLarge)
            } else {
                image?.imprint(label1, dx: 0, dy: 0, font: Keycap.systemSmall)
            }
        }
    }
}
