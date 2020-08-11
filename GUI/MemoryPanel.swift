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
        
    private func updateBankMap() {
    
        switch bankMapScheme {
            
        case 0: // Visible bank (CPU)
            
            memBank[0x0] = memInfo.peekSrc.0
            memBank[0x1] = memInfo.peekSrc.1
            memBank[0x2] = memInfo.peekSrc.2
            memBank[0x3] = memInfo.peekSrc.3
            memBank[0x4] = memInfo.peekSrc.4
            memBank[0x5] = memInfo.peekSrc.5
            memBank[0x6] = memInfo.peekSrc.6
            memBank[0x7] = memInfo.peekSrc.7
            memBank[0x8] = memInfo.peekSrc.8
            memBank[0x9] = memInfo.peekSrc.9
            memBank[0xA] = memInfo.peekSrc.10
            memBank[0xB] = memInfo.peekSrc.11
            memBank[0xC] = memInfo.peekSrc.12
            memBank[0xD] = memInfo.peekSrc.13
            memBank[0xE] = memInfo.peekSrc.14
            memBank[0xF] = memInfo.peekSrc.15
            
        case 1: // Visible bank (VIC)
            
            memBank[0x0] = memInfo.vicPeekSrc.0
            memBank[0x1] = memInfo.vicPeekSrc.1
            memBank[0x2] = memInfo.vicPeekSrc.2
            memBank[0x3] = memInfo.vicPeekSrc.3
            memBank[0x4] = memInfo.vicPeekSrc.4
            memBank[0x5] = memInfo.vicPeekSrc.5
            memBank[0x6] = memInfo.vicPeekSrc.6
            memBank[0x7] = memInfo.vicPeekSrc.7
            memBank[0x8] = memInfo.vicPeekSrc.8
            memBank[0x9] = memInfo.vicPeekSrc.9
            memBank[0xA] = memInfo.vicPeekSrc.10
            memBank[0xB] = memInfo.vicPeekSrc.11
            memBank[0xC] = memInfo.vicPeekSrc.12
            memBank[0xD] = memInfo.vicPeekSrc.13
            memBank[0xE] = memInfo.vicPeekSrc.14
            memBank[0xF] = memInfo.vicPeekSrc.15
            
        case 2: // RAM
            
            for i in 0...15 { memBank[i] = M_RAM }
            
        case 3: // ROM
            
            for i in 0...15 { memBank[i] = M_NONE }
            memBank[0xA] = M_BASIC
            memBank[0xB] = M_BASIC
            memBank[0xD] = M_CHAR
            memBank[0xE] = M_BASIC
            memBank[0xF] = M_KERNAL
            
        case 4: // IO

            for i in 0...15 { memBank[i] = M_NONE }
            memBank[0xD] = M_IO
            
        default:
            fatalError()
        }
    }
    
    private func cacheMemory() {
        
        memInfo = c64.mem.getInfo()
        if bankMap != memInfo.bankMap { layoutIsDirty = true }
        bankMap = memInfo.bankMap
    }
    
    func refreshMemory(count: Int = 0, full: Bool = false) {
        
        cacheMemory()
        
        // Recompute the bank map image if necessary
        if full || (layoutIsDirty && count >= nextLayoutRefresh) {
            
            updateBankMap()
            refreshMemoryLayout()
            memBankTableView.reloadData()
            memTableView.reloadData()
            layoutIsDirty = false
            nextLayoutRefresh = count + 10
        }
            
        memExrom.state = memInfo.exrom ? .on : .off
        memGame.state = memInfo.game ? .on : .off
        memCharen.state = memInfo.charen ? .on : .off
        memHiram.state = memInfo.hiram ? .on : .off
        memLoram.state = memInfo.loram ? .on : .off
        
        memBankTableView.refresh(count: count, full: full)
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
    }
    
    @IBAction func memSourceAction(_ sender: NSPopUpButton!) {
        
        track()
        bankMapScheme = sender.selectedTag()
        track("bankMapScheme = \(bankMapScheme)")
        layoutIsDirty = true
        refreshMemory()
    }
    
    func jumpTo(addr: Int) {
        
        if addr >= 0 && addr <= 0xFFFF {
            
            // selected = addr
            jumpTo(bank: addr >> 12)
            let row = (addr / 16) % 4096
            memTableView.scrollRowToVisible(row)
            memTableView.selectRowIndexes([row], byExtendingSelection: false)
        }
    }
    
    func jumpTo(type: [MemoryType]) {

        for i in 0...15 {

            if type.contains(memBank[i]!) {
                jumpTo(bank: i)
                return
            }
        }
    }
    
    func jumpTo(bank nr: Int) {
        
        track("jump to bank \(nr)")
        
        if nr >= 0 && nr <= 15 {
            
            selectedBank = nr
            memLayoutSlider.integerValue = nr
            memTableView.scrollRowToVisible(0)
            memBankTableView.scrollRowToVisible(nr)
            memBankTableView.selectRowIndexes([nr], byExtendingSelection: false)
            fullRefresh()
        }
    }
    
    // TODO: The following action functions have no effect, yet, because we
    // cannot distinguish different Rom types. Once M_ROM has been eliminated
    // and M_BASIC, M_KERNAL, M_CHAR have different values, the functions can
    // be uncommented.
    
    @IBAction func memPPAction(_ sender: NSButton!) {

        // jumpTo(bank: [M_PP])
    }

    @IBAction func memRamAction(_ sender: NSButton!) {

        // jumpTo(bank: [M_PP, M_RAM])
    }

    @IBAction func memBasicAction(_ sender: NSButton!) {

        // jumpTo(bank: [M_BASIC])
    }

    @IBAction func memCharAction(_ sender: NSButton!) {

        // jumpTo(bank: [M_CHAR])
    }

    @IBAction func memKernalAction(_ sender: NSButton!) {

        // jumpTo(type: [M_KERNAL])
    }

    @IBAction func memIOAction(_ sender: NSButton!) {

        // jumpTo(type: [M_IO])
    }

    @IBAction func memCrtLoAction(_ sender: NSButton!) {

        // jumpTo(type: [M_CRTLO])
    }

    @IBAction func memCrtHiAction(_ sender: NSButton!) {

        // jumpTo(type: [M_CRTHI])
    }
    
    @IBAction func memSliderAction(_ sender: NSSlider!) {

        jumpTo(bank: sender.integerValue)
    }

    @IBAction func memSearchAction(_ sender: NSTextField!) {
        
        track()
        
        let input = sender.stringValue
        let radix = hex ? 16 : 10
        let format = hex ? "%04X" : "%05d"
        
         if let addr = Int(input, radix: radix), input != "" {
             sender.stringValue = String(format: format, addr)
             jumpTo(addr: addr)
         } else {
             sender.stringValue = ""
             // selected = -1
         }
         fullRefresh()
    }
}

extension Inspector {
    
    var memLayoutImage: NSImage? {
                
        // Create image representation in memory
        let size = CGSize.init(width: 256, height: 16)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        
        // Create image data
        for bank in 0...15 {
            
            var color: NSColor
            switch memBank[bank]!.rawValue {
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

        // Mark the processor port area
        if memBank[0]!.rawValue == M_PP.rawValue {
            let ciColor = CIColor(color: MemColors.pp)!
            for y in 0...15 {
                let r = Int(ciColor.red * CGFloat(255 - y*2))
                let g = Int(ciColor.green * CGFloat(255 - y*2))
                let b = Int(ciColor.blue * CGFloat(255 - y*2))
                let a = Int(ciColor.alpha)
                ptr[256*y] = UInt32(r | g << 8 | b << 16 | a << 24)
            }
        }
        
        // Create image
        let image = NSImage.make(data: mask, rect: size)
        let resizedImage = image?.resizeSharp(width: 512, height: 16)
        return resizedImage
    }
}
