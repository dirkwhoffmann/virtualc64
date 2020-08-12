// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    private var selectedSprite: Int { return sprSelector.indexOfSelectedItem }

    private func cacheVIC() {

        vicInfo = c64.vic.getInfo()
        sprInfo = c64.vic.getSpriteInfo(selectedSprite)
    }
    
    func refreshVIC(count: Int = 0, full: Bool = false) {
        
        track()
        
        cacheVIC()
        
        if full {
            
        }
    }
}

// OLD (DEPRECATED)
extension MyController {
    
    func refreshVIC() {
        
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(sprite)
        
        // Note: It is likely that getInfo() captured the VIC state when the
        // emualtion thread is waiting. As a result, the displayed values for
        // cycle and rasterline do not seem randomly distributed. To make the
        // debugger output look realistic, we fake the output ...
        /*
        if c64.isRunning {
            info.rasterline = UInt16((vicRasterline.intValue + 173) % 263)
            info.cycle = UInt8((vicCycle.intValue + 17) % 64)
            info.xCounter = UInt16((vicXCounter.intValue + 136) % 512)
        }
        */
        
        let borderColorCell = vicBorderColor.cell as? NSButtonCell
        let bgColorCell0 = vicBackgroundColor0.cell as? NSButtonCell
        let bgColorCell1 = vicBackgroundColor1.cell as? NSButtonCell
        let bgColorCell2 = vicBackgroundColor2.cell as? NSButtonCell
        let bgColorCell3 = vicBackgroundColor3.cell as? NSButtonCell
        let spriteColorCell = spriteColor.cell as? NSButtonCell
        let spriteExtraColorCell1 = spriteExtraColor1.cell as? NSButtonCell
        let spriteExtraColorCell2 = spriteExtraColor2.cell as? NSButtonCell

        vicRasterline.intValue = Int32(info.rasterLine)
        vicCycle.intValue = Int32(info.rasterCycle)
        vicXCounter.intValue = Int32(info.xCounter)
        vicBadLine.state = info.badLine ? .on : .off
        vicDisplayMode.selectItem(withTag: Int(info.displayMode.rawValue))
        borderColorCell?.backgroundColor = c64.vic.color(Int(info.borderColor))
        bgColorCell0?.backgroundColor = c64.vic.color(Int(info.bgColor0))
        bgColorCell1?.backgroundColor = c64.vic.color(Int(info.bgColor1))
        bgColorCell2?.backgroundColor = c64.vic.color(Int(info.bgColor2))
        bgColorCell3?.backgroundColor = c64.vic.color(Int(info.bgColor3))
        vicScreenGeometry.selectItem(withTag: Int(info.screenGeometry.rawValue))
        vicDx.intValue = Int32(info.dx)
        vicDXStepper.intValue = Int32(info.dx)
        vicDy.intValue = Int32(info.dy)
        vicDYStepper.intValue = Int32(info.dy)
        vicMemoryBankAddr.selectItem(withTag: Int(info.memoryBankAddr))
        vicScreenMemoryAddr.selectItem(withTag: Int(info.screenMemoryAddr))
        vicCharacterMemoryAddr.selectItem(withTag: Int(info.charMemoryAddr))
        vicIrqRasterline.intValue = Int32(info.irqRasterline)
        // vicIrqLine.state = info.irqLine ? .on : .off
        
        spriteEnabled.state = sinfo.enabled ? .on : .off
        spriteX.intValue = Int32(sinfo.x)
        spriteXStepper.intValue = Int32(sinfo.x)
        spriteY.intValue = Int32(sinfo.y)
        spriteYStepper.intValue = Int32(sinfo.y)
        // spritePtr.intValue = Int32(sinfo.ptr)
        // spritePtrStepper.intValue = Int32(sinfo.ptr)
        spriteColorCell?.backgroundColor = c64.vic.color(Int(sinfo.color))
        spriteIsMulticolor.selectItem(withTag: sinfo.multicolor ? 1 : 0)
        spriteExtraColorCell1?.backgroundColor = c64.vic.color(Int(sinfo.extraColor1))
        spriteExtraColorCell2?.backgroundColor = c64.vic.color(Int(sinfo.extraColor2))
        spriteExpandX.state = sinfo.expandX ? .on : .off
        spriteExpandY.state = sinfo.expandY ? .on : .off
        spritePriority.selectItem(withTag: sinfo.priority ? 1 : 0)
        spriteCollidesWithSprite.state = sinfo.collidesWithSprite ? .on : .off
        // spriteSpriteIrqEnabled.state = info.spriteCollisionIrqEnabled ? .on : .off
        spriteCollidesWithBackground.state = sinfo.collidesWithBackground ? .on : .off
        // spriteBackgroundIrqEnabled.state  = info.backgroundCollisionIrqEnabled ? .on : .off
        
        // Unhide some color cells in multicolor modes
        var hidden = (info.displayMode.rawValue & 0x10) == 0
        vicBackgroundColor1.isHidden = hidden
        vicBackgroundColor2.isHidden = hidden
        vicBackgroundColor3.isHidden = hidden
        
        hidden = !sinfo.multicolor
        spriteExtraColor1.isHidden = hidden
        spriteExtraColor2.isHidden = hidden
    }
    
    private var sprite: Int { return spriteSelector.indexOfSelectedItem }

    //
    // Action methods
    //
    
    // Sprites
    
    func spriteSelectAction(_ sender: Any!) {
        
        refreshVIC()
    }
}
