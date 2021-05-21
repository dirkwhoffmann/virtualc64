// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSColorWell {
    
    func setColor(_ rgb: (Double, Double, Double) ) {
        
        color = NSColor.init(r: rgb.0, g: rgb.1, b: rgb.2)
    }

    func setColor(_ bgr: UInt32) {
        
        let r = Int((bgr >> 0) & 0xFF)
        let g = Int((bgr >> 8) & 0xFF)
        let b = Int((bgr >> 16) & 0xFF)

        color = NSColor.init(r: r, g: g, b: b)
    }
}

extension NSColor {
    
    static var warningColor: NSColor {
        return NSColor(named: NSColor.Name("warningColor"))!
    }
    
    static var recordingColor: NSColor {
        return NSColor(named: NSColor.Name("recordingColor"))!
    }

    convenience init(r: Int, g: Int, b: Int, a: Int = 255) {
        
        self.init(red: CGFloat(r) / 255,
                  green: CGFloat(g) / 255,
                  blue: CGFloat(b) / 255,
                  alpha: CGFloat(a) / 255)
    }
    
    convenience init(r: Double, g: Double, b: Double, a: Double = 1.0) {

        self.init(red: CGFloat(r),
                  green: CGFloat(g),
                  blue: CGFloat(b),
                  alpha: CGFloat(a))
    }

    convenience init(_ rgb: (Double, Double, Double)) {
        
        self.init(r: rgb.0, g: rgb.1, b: rgb.2)
    }
    
    convenience init(rgba: UInt32) {

        let a = CGFloat(rgba & 0xFF)
        let b = CGFloat((rgba >> 8) & 0xFF)
        let g = CGFloat((rgba >> 16) & 0xFF)
        let r = CGFloat((rgba >> 24) & 0xFF)

        self.init(red: r / 255.0, green: g / 255.0, blue: b / 255.0, alpha: a / 255.0)
    }

    convenience init(abgr: UInt32) {

        let r = CGFloat(abgr & 0xFF)
        let g = CGFloat((abgr >> 8) & 0xFF)
        let b = CGFloat((abgr >> 16) & 0xFF)
        let a = CGFloat((abgr >> 24) & 0xFF)

        self.init(red: r / 255.0, green: g / 255.0, blue: b / 255.0, alpha: a / 255.0)
    }

    func rgb() -> (Int, Int, Int) {
        
        let r = Int(redComponent * 255)
        let g = Int(greenComponent * 255)
        let b = Int(blueComponent * 255)
        
        return (r, g, b)
    }
    
    func rgba() -> (Int, Int, Int, Int) {
        
        let r = Int(redComponent * 255)
        let g = Int(greenComponent * 255)
        let b = Int(blueComponent * 255)
        
        return (r, g, b, 255)
    }
    
    var gpuColor: Int {
        
        let r = Int(redComponent * 255)
        let g = Int(greenComponent * 255)
        let b = Int(blueComponent * 255)

        return (0xFF << 24) | (b << 16) | (g << 8) | r
    }
}
