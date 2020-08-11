// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct MemColors {

    static let unmapped = NSColor.gray
    static let pp       = NSColor.init(r: 0xFF, g: 0xFF, b: 0x33, a: 0xFF)
    static let ram      = NSColor.init(r: 0x33, g: 0xFF, b: 0x33, a: 0xFF)
    static let cartlo   = NSColor.init(r: 0x99, g: 0x33, b: 0xFF, a: 0xFF)
    static let carthi   = NSColor.init(r: 0xFF, g: 0x33, b: 0xFF, a: 0xFF)
    static let kernal   = NSColor.init(r: 0x33, g: 0xFF, b: 0xFF, a: 0xFF)
    static let basic    = NSColor.init(r: 0x33, g: 0x99, b: 0xFF, a: 0xFF)
    static let char     = NSColor.init(r: 0x33, g: 0x33, b: 0xFF, a: 0xFF)
    static let io       = NSColor.init(r: 0xFF, g: 0x33, b: 0x99, a: 0xFF)
}

extension Inspector {
    
    var mem: MemoryProxy { return c64.mem }
    
    var memLayoutImage: NSImage? {
                
        // Create image representation in memory
        let size = CGSize.init(width: 256, height: 16)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        
        let src = [ memInfo.peekSrc.0, memInfo.peekSrc.1,
                    memInfo.peekSrc.2, memInfo.peekSrc.3,
                    memInfo.peekSrc.4, memInfo.peekSrc.5,
                    memInfo.peekSrc.6, memInfo.peekSrc.7,
                    memInfo.peekSrc.8, memInfo.peekSrc.9,
                    memInfo.peekSrc.10, memInfo.peekSrc.11,
                    memInfo.peekSrc.12, memInfo.peekSrc.13,
                    memInfo.peekSrc.14, memInfo.peekSrc.15 ]
        
        // Create image data
        for bank in 0...15 {
            
            var color: NSColor
            switch src[bank].rawValue {
            case M_NONE.rawValue: color = MemColors.unmapped
            case M_PP.rawValue: color = MemColors.ram
            case M_RAM.rawValue: color = MemColors.ram
            case M_ROM.rawValue where bank <= 0xB: color = MemColors.basic
            case M_ROM.rawValue where bank == 0xD: color = MemColors.char
            case M_ROM.rawValue where bank >= 0xE: color = MemColors.kernal
            case M_IO.rawValue: color = MemColors.io
            case M_CRTLO.rawValue: color = MemColors.cartlo
            case M_CRTHI.rawValue: color = MemColors.carthi
            default: color = MemColors.unmapped
            }
            
            let ciColor = CIColor(color: color)!
            for y in 0...15 {
                for i in 0...15 {
                    let r = Int(ciColor.red * CGFloat(255 - y*2))
                    let g = Int(ciColor.green * CGFloat(255 - y*2))
                    let b = Int(ciColor.blue * CGFloat(255 - y*2))
                    let a = Int(ciColor.alpha)
                    ptr[256*y+16*bank+i] = UInt32(r | g << 8 | b << 16 | a << 24)
                }
            }
        }

        let ciColor = CIColor(color: MemColors.pp)!
        for y in 0...15 {
            let r = Int(ciColor.red * CGFloat(255 - y*2))
            let g = Int(ciColor.green * CGFloat(255 - y*2))
            let b = Int(ciColor.blue * CGFloat(255 - y*2))
            let a = Int(ciColor.alpha)
            ptr[256*y] = UInt32(r | g << 8 | b << 16 | a << 24)
            ptr[256*y+1] = UInt32(r | g << 8 | b << 16 | a << 24)
        }
        
        // Create image
        let image = NSImage.make(data: mask, rect: size)
        let resizedImage = image?.resizeSharp(width: 512, height: 16)
        return resizedImage
    }
    
    private func cacheMemory() {
        
        memInfo = c64.mem.getInfo()
        if bankMap != memInfo.bankMap { memoryLayoutIsDirty = true }
        bankMap = memInfo.bankMap
    }
    
    func refreshMemory(count: Int = 0, full: Bool = false) {
        
        cacheMemory()
        
        // Recompute the bank map image if necessary
        if full || (memoryLayoutIsDirty && (count % 5) == 1) {
            
            refreshMemoryLayout()
        }
            
        memTableView.refresh(count: count, full: full)
    }
    
    private func refreshMemoryLayout() {
        
        let size = NSSize(width: 16, height: 16)
        
        memLayoutButton.image   = memLayoutImage
        memRamButton.image    = NSImage.init(color: MemColors.ram, size: size)
        memKernalButton.image = NSImage.init(color: MemColors.kernal, size: size)
        memBasicButton.image  = NSImage.init(color: MemColors.basic, size: size)
        memCharButton.image   = NSImage.init(color: MemColors.char, size: size)
        memPPButton.image     = NSImage.init(color: MemColors.pp, size: size)
        memIOButton.image     = NSImage.init(color: MemColors.io, size: size)
        memCartLoButton.image = NSImage.init(color: MemColors.cartlo, size: size)
        memCartHiButton.image = NSImage.init(color: MemColors.carthi, size: size)
        
        memoryLayoutIsDirty = false
    }
    
    @IBAction func memSourceAction(_ sender: NSPopUpButton!) {
        
        track()
    }
    
    @IBAction func memSearchAction(_ sender: NSTextField!) {
        
        track()
        /*
        let input = sender.stringValue
        if let addr = Int(input, radix: 16), input != "" {
            sender.stringValue = String(format: "%06X", addr)
            setSelected(addr)
        } else {
            sender.stringValue = ""
            selected = -1
        }
        fullRefresh()
        */
    }
}
