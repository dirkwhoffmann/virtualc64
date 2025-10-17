// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct MemColors {

    static let unmapped = NSColor.gray
    static let pp       = NSColor(r: 0xFF, g: 0xFF, b: 0xFF, a: 0xFF)
    static let ram      = NSColor(r: 0x99, g: 0xFF, b: 0x99, a: 0xFF)
    static let cartlo   = NSColor(r: 0xCC, g: 0x99, b: 0xFF, a: 0xFF)
    static let carthi   = NSColor(r: 0xFF, g: 0x99, b: 0xFF, a: 0xFF)
    static let kernal   = NSColor(r: 0x99, g: 0xCC, b: 0xFF, a: 0xFF)
    static let basic    = NSColor(r: 0xFF, g: 0x99, b: 0x99, a: 0xFF)
    static let char     = NSColor(r: 0xFF, g: 0xFF, b: 0x99, a: 0xFF)
    static let io       = NSColor(r: 0x99, g: 0xFF, b: 0xFF, a: 0xFF)
}

extension Inspector {
    
    var mem: MemoryProxy? { return emu?.mem }

    private func updateBankMap() {
    
        switch memSource.selectedTag() {
            
        case 0: // Visible bank (CPU)
            
            bankType[0x0] = memInfo.peekSrc.0
            bankType[0x1] = memInfo.peekSrc.1
            bankType[0x2] = memInfo.peekSrc.2
            bankType[0x3] = memInfo.peekSrc.3
            bankType[0x4] = memInfo.peekSrc.4
            bankType[0x5] = memInfo.peekSrc.5
            bankType[0x6] = memInfo.peekSrc.6
            bankType[0x7] = memInfo.peekSrc.7
            bankType[0x8] = memInfo.peekSrc.8
            bankType[0x9] = memInfo.peekSrc.9
            bankType[0xA] = memInfo.peekSrc.10
            bankType[0xB] = memInfo.peekSrc.11
            bankType[0xC] = memInfo.peekSrc.12
            bankType[0xD] = memInfo.peekSrc.13
            bankType[0xE] = memInfo.peekSrc.14
            bankType[0xF] = memInfo.peekSrc.15
            
        case 1: // Visible bank (VIC)
            
            bankType[0x0] = memInfo.vicPeekSrc.0
            bankType[0x1] = memInfo.vicPeekSrc.1
            bankType[0x2] = memInfo.vicPeekSrc.2
            bankType[0x3] = memInfo.vicPeekSrc.3
            bankType[0x4] = memInfo.vicPeekSrc.4
            bankType[0x5] = memInfo.vicPeekSrc.5
            bankType[0x6] = memInfo.vicPeekSrc.6
            bankType[0x7] = memInfo.vicPeekSrc.7
            bankType[0x8] = memInfo.vicPeekSrc.8
            bankType[0x9] = memInfo.vicPeekSrc.9
            bankType[0xA] = memInfo.vicPeekSrc.10
            bankType[0xB] = memInfo.vicPeekSrc.11
            bankType[0xC] = memInfo.vicPeekSrc.12
            bankType[0xD] = memInfo.vicPeekSrc.13
            bankType[0xE] = memInfo.vicPeekSrc.14
            bankType[0xF] = memInfo.vicPeekSrc.15
            
        case 2: // RAM
            
            for i in 0...15 { bankType[i] = .RAM }
            
        case 3: // ROM
            
            for i in 0...15 { bankType[i] = .NONE }
            bankType[0xA] = .BASIC
            bankType[0xB] = .BASIC
            bankType[0xD] = .CHAR
            bankType[0xE] = .KERNAL
            bankType[0xF] = .KERNAL
            
        case 4: // IO

            for i in 0...15 { bankType[i] = .NONE }
            bankType[0xD] = .IO
            
        default:
            fatalError()
        }
    }
    
    private func cacheMemory() {
        
        if let emu = emu {
            
            let oldBankMap = memInfo?.bankMap
            memInfo = emu.paused ? emu.mem.info : emu.mem.cachedInfo
            if oldBankMap != memInfo.bankMap { layoutIsDirty = true }
        }
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
        
        if memTab.selectedTabViewItem?.label == "Heatmap" {
            memHeatmapView.update()
        }
    }
    
    private func refreshMemoryLayout() {
        
        let size = NSSize(width: 16, height: 16)
        
        memLayoutButton.image   = memLayoutImage
        memRamButton.image    = NSImage(color: MemColors.ram, size: size)
        memKernalButton.image = NSImage(color: MemColors.kernal, size: size)
        memBasicButton.image  = NSImage(color: MemColors.basic, size: size)
        memCharButton.image   = NSImage(color: MemColors.char, size: size)
        memPPButton.image     = NSImage(color: MemColors.pp, size: size)
        memIOButton.image     = NSImage(color: MemColors.io, size: size)
        memCartLoButton.image = NSImage(color: MemColors.cartlo, size: size)
        memCartHiButton.image = NSImage(color: MemColors.carthi, size: size)
    }
    
    @IBAction func memSourceAction(_ sender: NSPopUpButton!) {
        
        layoutIsDirty = true
        refreshMemory(full: true)
    }
    
    func jumpTo(addr: Int) {
                
        if addr >= 0 && addr <= 0xFFFF {
            
            jumpTo(bank: addr >> 12)
            let row = (addr & 0xFFF) / 16
            memTableView.scrollRowToVisible(row)
            memTableView.selectRowIndexes([row], byExtendingSelection: false)
        }
    }

    func jumpTo(type: [vc64.MemType]) {

        for i in 0...15 where type.contains(bankType[i]!) {
            jumpTo(bank: i)
            return
        }
    }

    func jumpTo(bank nr: Int) {
                
        if nr >= 0 && nr <= 15 {
            
            displayedBank = nr
            memLayoutSlider.integerValue = nr
            memTableView.scrollRowToVisible(0)
            memBankTableView.scrollRowToVisible(nr)
            memBankTableView.selectRowIndexes([nr], byExtendingSelection: false)
            fullRefresh()
        }
    }
        
    @IBAction func memPPAction(_ sender: NSButton!) {

        jumpTo(type: [.PP])
    }

    @IBAction func memRamAction(_ sender: NSButton!) {

        jumpTo(type: [.PP, .RAM])
    }

    @IBAction func memBasicAction(_ sender: NSButton!) {

        jumpTo(type: [.BASIC])
    }

    @IBAction func memCharAction(_ sender: NSButton!) {

        jumpTo(type: [.CHAR])
    }

    @IBAction func memKernalAction(_ sender: NSButton!) {

        jumpTo(type: [.KERNAL])
    }

    @IBAction func memIOAction(_ sender: NSButton!) {

        jumpTo(type: [.IO])
    }

    @IBAction func memCrtLoAction(_ sender: NSButton!) {

        jumpTo(type: [.CRTLO])
    }

    @IBAction func memCrtHiAction(_ sender: NSButton!) {

        jumpTo(type: [.CRTHI])
    }
    
    @IBAction func memSliderAction(_ sender: NSSlider!) {

        jumpTo(bank: min(sender.integerValue, 15))
    }

    @IBAction func memSearchAction(_ sender: NSTextField!) {
                
        let input = sender.stringValue
        let radix = hex ? 16 : 10
        let format = hex ? "%04X" : "%05d"
        
         if let addr = Int(input, radix: radix), input != "" {
             sender.stringValue = String(format: format, addr)
             jumpTo(addr: addr)
         } else {
             sender.stringValue = ""
         }
         fullRefresh()
    }
}

extension Inspector {
    
    var memLayoutImage: NSImage? {
                
        // Create image representation in memory
        let width = 256
        let height = 16
        let size = CGSize(width: width, height: height)
        let cap = width * height
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)
        let c = 3
        let banks = 16
        let dx = width / banks

        // Create image data
        for bank in 0..<banks {
            
            var color: NSColor

            switch bankType[bank] {
            case .NONE: color = MemColors.unmapped
            case .PP: color = MemColors.ram
            case .RAM: color = MemColors.ram
            case .BASIC: color = MemColors.basic
            case .CHAR: color = MemColors.char
            case .KERNAL: color = MemColors.kernal
            case .IO: color = MemColors.io
            case .CRTLO: color = MemColors.cartlo
            case .CRTHI: color = MemColors.carthi
            default: color = MemColors.unmapped
            }

            let ciColor = CIColor(color: color)!
            for y in 0..<height {
                for i in 0..<dx {
                    let r = Int(ciColor.red * CGFloat(255 - y*c))
                    let g = Int(ciColor.green * CGFloat(255 - y*c))
                    let b = Int(ciColor.blue * CGFloat(255 - y*c))
                    let a = Int(ciColor.alpha * CGFloat(255))
                    let abgr = UInt32(r | g << 8 | b << 16 | a << 24)
                    ptr[width*y+dx*bank+i] = abgr
                }
            }
        }

        // Mark the processor port area
        if bankType[0] == .PP {
            let ciColor = CIColor(color: MemColors.pp)!
            for y in 0..<height {
                let r = Int(ciColor.red * CGFloat(255 - y*c))
                let g = Int(ciColor.green * CGFloat(255 - y*c))
                let b = Int(ciColor.blue * CGFloat(255 - y*c))
                let a = Int(ciColor.alpha * CGFloat(255))
                let abgr = UInt32(r | g << 8 | b << 16 | a << 24)
                ptr[width*y] = abgr
            }
        }
        
        // Create image
        let image = NSImage.make(data: mask, rect: size)
        let resizedImage = image?.resizeSharp(width: 512, height: 16)
        return resizedImage
    }
}
