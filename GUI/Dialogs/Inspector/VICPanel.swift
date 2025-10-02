// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    private var selectedSprite: Int { return sprSelector.indexOfSelectedItem }

    private func cacheVIC() {

        if let emu = emu {

            vicInfo = emu.paused ? emu.vic.info : emu.vic.cachedInfo
            sprInfo = emu.vic.getSpriteInfo(selectedSprite)
        }
    }
    
    func refreshVIC(count: Int = 0, full: Bool = false) {

        if let emu = emu {
            
            cacheVIC()

            if full {

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
                sprY.assignFormatter(fmt9)
            }

            vicYCounter.integerValue = Int(vicInfo.yCounter)
            vicXCounter.integerValue = Int(vicInfo.xCounter)
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
            vicBorderColor.color = emu.vic.color(Int(vicInfo.borderColor))
            vicBgColor0.color = emu.vic.color(Int(vicInfo.bgColor0))
            vicBgColor1.color = emu.vic.color(Int(vicInfo.bgColor1))
            vicBgColor2.color = emu.vic.color(Int(vicInfo.bgColor2))
            vicBgColor3.color = emu.vic.color(Int(vicInfo.bgColor3))
            vicMemSelect.integerValue = Int(vicInfo.memSelect)
            vicUltimax.state = vicInfo.ultimax ? .on : .off
            vicMemoryBankAddr.selectItem(withTag: Int(vicInfo.memoryBankAddr))
            vicScreenMemoryAddr.selectItem(withTag: Int(vicInfo.screenMemoryAddr))
            vicCharMemoryAddr.selectItem(withTag: Int(vicInfo.charMemoryAddr))
            vicIrqRasterline.integerValue = Int(vicInfo.irqLine)
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
            sprColor.color = emu.vic.color(Int(sprInfo.color))
            sprExtra1.color = emu.vic.color(Int(sprInfo.extraColor1))
            sprExtra2.color = emu.vic.color(Int(sprInfo.extraColor2))
            
            // Stencils
            let layers = config.vicCutLayers
            cutEnable.state = (layers & 0x1000) != 0 ? .on : .off
            cutBorder.state = (layers & 0x800) != 0 ? .on : .off
            cutForeground.state = (layers & 0x400) != 0 ? .on : .off
            cutBackground.state = (layers & 0x200) != 0 ? .on : .off
            cutSprites.state = (layers & 0x100) != 0 ? .on : .off
            cutSprite7.state = (layers & 0x80) != 0 ? .on : .off
            cutSprite6.state = (layers & 0x40) != 0 ? .on : .off
            cutSprite5.state = (layers & 0x20) != 0 ? .on : .off
            cutSprite4.state = (layers & 0x10) != 0 ? .on : .off
            cutSprite3.state = (layers & 0x08) != 0 ? .on : .off
            cutSprite2.state = (layers & 0x04) != 0 ? .on : .off
            cutSprite1.state = (layers & 0x02) != 0 ? .on : .off
            cutSprite0.state = (layers & 0x01) != 0 ? .on : .off
            cutOpacity.integerValue = config.vicCutOpacity

            cutOpacity.isEnabled = (layers & 0x1000) != 0
            cutBorder.isEnabled = (layers & 0x1000) != 0
            cutForeground.isEnabled = (layers & 0x1000) != 0
            cutBackground.isEnabled = (layers & 0x1000) != 0
            cutSprites.isEnabled = (layers & 0x1000) != 0
            cutSprite7.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite6.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite5.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite4.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite3.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite2.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite1.isEnabled = (layers & 0x1100) == 0x1100
            cutSprite0.isEnabled = (layers & 0x1100) == 0x1100
        }
    }
    
    @IBAction func selectSpriteAction(_ sender: Any!) {

        fullRefresh()
    }
    
    //
    // Action methods
    //
        
    @IBAction func cutOpacityAction(_ sender: NSSlider!) {
        
        config.vicCutOpacity = sender.integerValue
    }
    
    func addLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers | mask
    }
    func removeLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers & ~mask
    }

    @IBAction func cutEnableAction(_ sender: NSButton!) {

        if sender.state == .on { addLayer(0x1000) } else { removeLayer(0x1000) }
    }
    
    @IBAction func cutBorderAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x800) } else { removeLayer(0x800) }
    }

    @IBAction func cutForegroundAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x400) } else { removeLayer(0x400) }
    }

    @IBAction func cutBackgroundAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x200) } else { removeLayer(0x200) }
    }

    @IBAction func cutSpritesAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x100) } else { removeLayer(0x100) }
    }

    @IBAction func cutSingleSpriteAction(_ sender: NSButton!) {
    
        let sprite = sender.tag
        if sender.state == .on { addLayer(1 << sprite) } else { removeLayer(1 << sprite) }
    }
}
