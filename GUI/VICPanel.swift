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
         
            vicRasterLine.assignFormatter(fmt9)
            vicRasterCycle.assignFormatter(fmt9)
            vicYCounter.assignFormatter(fmt9)
            vicXCounter.assignFormatter(fmt9)
            vicVC.assignFormatter(fmt10)
            vicVCBase.assignFormatter(fmt10)
            vicRC.assignFormatter(fmt3)
            vicVMLI.assignFormatter(fmt6)
            vicCtrl1.assignFormatter(fmt8)
            vicCtrl2.assignFormatter(fmt8)
            vicDy.assignFormatter(fmt3)
            vicDx.assignFormatter(fmt3)
            vicMemSelect.assignFormatter(fmt8)
            vicIrqRasterline.assignFormatter(fmt8)
            vicImr.assignFormatter(fmt8)
            vicIrr.assignFormatter(fmt8)
            latchedLPX.assignFormatter(fmt8)
            latchedLPY.assignFormatter(fmt8)
            sprX.assignFormatter(fmt9)
            sprY.assignFormatter(fmt8)
        }
                
        if c64.isRunning {

            // If the emulator is running, it is likely that the VIC state has
            // been captured while the emualtion thread is waiting. As a result,
            // the displayed values for rasterlines and rasterline cycles do not
            // seem distributed randomly. To make the debugger output look
            // realistic, we fake the output...
            
            vicRasterLine.integerValue = (vicRasterLine.integerValue + 173) % 263
            vicRasterCycle.integerValue = (vicRasterCycle.integerValue + 17) % 64
            vicYCounter.integerValue = (vicYCounter.integerValue + 173) % 263
            vicXCounter.integerValue = (vicXCounter.integerValue + 136) % 512

        } else {

            vicRasterLine.integerValue = Int(vicInfo.rasterLine)
            vicRasterCycle.integerValue = Int(vicInfo.rasterCycle)
            vicYCounter.integerValue = Int(vicInfo.yCounter)
            vicXCounter.integerValue = Int(vicInfo.xCounter)
        }
        
        vicVC.integerValue = Int(vicInfo.vc)
        vicVCBase.integerValue = Int(vicInfo.vcBase)
        vicRC.integerValue = Int(vicInfo.rc)
        vicVMLI.integerValue = Int(vicInfo.vmli)
        vicCtrl1.integerValue = Int(vicInfo.ctrl1)
        vicCtrl2.integerValue = Int(vicInfo.ctrl2)
        vicDy.integerValue = Int(vicInfo.dx)
        vicDx.integerValue = Int(vicInfo.dy)
        vicDenBit.state = vicInfo.denBit ? .on : .off
        vicBadLine.state = vicInfo.badLine ? .on : .off
        vicDisplayState.state = vicInfo.displayState ? .on : .off
        vicVBlank.state = vicInfo.vblank ? .on : .off
        vicScreenGeometry.selectItem(withTag: Int(vicInfo.screenGeometry.rawValue))
        vicVFlop.state = vicInfo.frameFF.vertical ? .on : .off
        vicHFlop.state = vicInfo.frameFF.main ? .on : .off
        vicDisplayMode.selectItem(withTag: Int(vicInfo.displayMode.rawValue))
        vicBorderColor.color = c64.vic.color(Int(vicInfo.borderColor))
        vicBgColor0.color = c64.vic.color(Int(vicInfo.bgColor0))
        vicBgColor1.color = c64.vic.color(Int(vicInfo.bgColor1))
        vicBgColor2.color = c64.vic.color(Int(vicInfo.bgColor2))
        vicBgColor3.color = c64.vic.color(Int(vicInfo.bgColor3))
        vicMemSelect.integerValue = Int(vicInfo.memSelect)
        vicUltimax.state = vicInfo.ultimax ? .on : .off
        vicMemoryBankAddr.selectItem(withTag: Int(vicInfo.memoryBankAddr))
        vicScreenMemoryAddr.selectItem(withTag: Int(vicInfo.screenMemoryAddr))
        vicCharMemoryAddr.selectItem(withTag: Int(vicInfo.charMemoryAddr))
        vicIrqRasterline.integerValue = Int(vicInfo.irqRasterline)
        vicImr.integerValue = Int(vicInfo.imr)
        vicImrLP.state = (vicInfo.imr & 0b1000) != 0 ? .on : .off
        vicImrSS.state = (vicInfo.imr & 0b0100) != 0 ? .on : .off
        vicImrSB.state = (vicInfo.imr & 0b0010) != 0 ? .on : .off
        vicImrRaster.state = (vicInfo.imr & 0b0001) != 0 ? .on : .off
        vicIrr.integerValue = Int(vicInfo.irr)
        vicIrrIrq.state = (vicInfo.irr & 0b10000000) != 0 ? .on : .off
        vicIrrLP.state = (vicInfo.irr & 0b1000) != 0 ? .on : .off
        vicIrrSS.state = (vicInfo.irr & 0b0100) != 0 ? .on : .off
        vicIrrSB.state = (vicInfo.irr & 0b0010) != 0 ? .on : .off
        vicIrrRaster.state = (vicInfo.irr & 0b0001) != 0 ? .on : .off
        latchedLPX.integerValue = Int(vicInfo.latchedLPX)
        latchedLPY.integerValue = Int(vicInfo.latchedLPY)
        vicLpLine.state = vicInfo.lpLine ? .on : .off
        vicLpIrqHasOccurred.state = vicInfo.lpIrqHasOccurred ? .on : .off

        sprX.integerValue = Int(sprInfo.x)
        sprY.integerValue = Int(sprInfo.y)
        sprEnabled.state = sprInfo.enabled ? .on : .off
        sprExpandX.state = sprInfo.expandX ? .on : .off
        sprExpandY.state = sprInfo.expandY ? .on : .off
        sprPriority.state = sprInfo.priority ? .on : .off
        sprMulticolor.state = sprInfo.multicolor ? .on : .off
        sprSSCollision.state = sprInfo.ssCollision ? .on : .off
        sprSBCollision.state = sprInfo.sbCollision ? .on : .off
        sprColor.color = c64.vic.color(Int(sprInfo.color))
        sprExtra1.color = c64.vic.color(Int(sprInfo.extraColor1))
        sprExtra2.color = c64.vic.color(Int(sprInfo.extraColor2))
    }
    
    @IBAction func selectSpriteAction(_ sender: Any!) {

        fullRefresh()
    }
}

/*
extension MyController {
    
    func refreshVIC() {
        
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(sprite)
        
        // Note: It is likely that getInfo() captured the VIC state when the
        // emualtion thread is waiting. As a result, the displayed values for
        // cycle and rasterline do not seem randomly distributed. To make the
        // debugger output look realistic, we fake the output ...
 
        if c64.isRunning {
            info.rasterline = UInt16((vicRasterline.intValue + 173) % 263)
            info.cycle = UInt8((vicCycle.intValue + 17) % 64)
            info.xCounter = UInt16((vicXCounter.intValue + 136) % 512)
        }
        
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
        spriteCollidesWithSprite.state = sinfo.ssCollision ? .on : .off
        // spriteSpriteIrqEnabled.state = info.spriteCollisionIrqEnabled ? .on : .off
        spriteCollidesWithBackground.state = sinfo.sbCollision ? .on : .off
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
}
*/
