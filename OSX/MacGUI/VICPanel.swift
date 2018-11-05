//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {
    
    func refreshVIC() {
        
        var info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(sprite)
        
        // Note: It is likely that getInfo() captured the VIC state when the emualtion
        // thread is waiting. As a result, the displayed values for cycle and rasterline
        // do not seem randomly distributed. To make the debugger output look realistic,
        // we fake the output a little bit ...
        if c64.isRunning() {
            info.rasterline = UInt16((vicRasterline.intValue + 173) % 263)
            info.cycle = UInt8((vicCycle.intValue + 17) % 64)
            info.xCounter = UInt16((vicXCounter.intValue + 136) % 512)
        }

        let borderColorCell = vicBorderColor.cell as! NSButtonCell
        let bgColorCell0 = vicBackgroundColor0.cell as! NSButtonCell
        let bgColorCell1 = vicBackgroundColor1.cell as! NSButtonCell
        let bgColorCell2 = vicBackgroundColor2.cell as! NSButtonCell
        let bgColorCell3 = vicBackgroundColor3.cell as! NSButtonCell
        let spriteColorCell = spriteColor.cell as! NSButtonCell
        let spriteExtraColorCell1 = spriteExtraColor1.cell as! NSButtonCell
        let spriteExtraColorCell2 = spriteExtraColor2.cell as! NSButtonCell

        vicRasterline.intValue = Int32(info.rasterline)
        vicCycle.intValue = Int32(info.cycle)
        vicXCounter.intValue = Int32(info.xCounter)
        vicBadLine.state = info.badLine ? .on : .off
        vicDisplayMode.selectItem(withTag: Int(info.displayMode.rawValue))
        borderColorCell.backgroundColor = c64.vic.color(Int(info.borderColor))
        bgColorCell0.backgroundColor = c64.vic.color(Int(info.backgroundColor0))
        bgColorCell1.backgroundColor = c64.vic.color(Int(info.backgroundColor1))
        bgColorCell2.backgroundColor = c64.vic.color(Int(info.backgroundColor2))
        bgColorCell3.backgroundColor = c64.vic.color(Int(info.backgroundColor3))
        vicScreenGeometry.selectItem(withTag: Int(info.screenGeometry.rawValue))
        vicDx.intValue = Int32(info.dx)
        vicDXStepper.intValue = Int32(info.dx)
        vicDy.intValue = Int32(info.dy)
        vicDYStepper.intValue = Int32(info.dy)
        vicMemoryBankAddr.selectItem(withTag: Int(info.memoryBankAddr))
        vicScreenMemoryAddr.selectItem(withTag: Int(info.screenMemoryAddr))
        vicCharacterMemoryAddr.selectItem(withTag: Int(info.characterMemoryAddr))
        vicIrqRasterline.intValue = Int32(info.irqRasterline)
        vicIrqLine.state = info.irqLine ? .on : .off
        
        spriteEnabled.state = sinfo.enabled ? .on : .off
        spriteX.intValue = Int32(sinfo.x)
        spriteY.intValue = Int32(sinfo.y)
        spriteColorCell.backgroundColor = c64.vic.color(Int(sinfo.color))
        spriteIsMulticolor.selectItem(withTag: sinfo.multicolor ? 1 : 0)
        spriteExtraColorCell1.backgroundColor = c64.vic.color(Int(sinfo.extraColor1))
        spriteExtraColorCell2.backgroundColor = c64.vic.color(Int(sinfo.extraColor2))
        spriteExpandX.state = sinfo.expandX ? .on : .off
        spriteExpandY.state = sinfo.expandY ? .on : .off
        spritePriority.state = sinfo.priority ? .on : .off
        spriteCollidesWithSprite.state = sinfo.collidesWithSprite ? .on : .off
        spriteSpriteIrqEnabled.state = info.spriteCollisionIrqEnabled ? .on : .off
        spriteCollidesWithBackground.state = sinfo.collidesWithBackground ? .on : .off
        spriteBackgroundIrqEnabled.state  = info.backgroundCollisionIrqEnabled ? .on : .off
        
        // Unhide some color cells in multicolor modes
        var hidden = (info.displayMode.rawValue & 0x10) == 0
        vicBackgroundColor1.isHidden = hidden
        vicBackgroundColor2.isHidden = hidden
        vicBackgroundColor3.isHidden = hidden
        
        hidden = !sinfo.multicolor
        spriteExtraColor1.isHidden = hidden
        spriteExtraColor2.isHidden = hidden
    }
    
    private var sprite: Int {
        get { return spriteSelector.indexOfSelectedItem }
    }
        
    
    //
    // Action methods
    //
    
    @IBAction func selectSpriteAction(_ sender: Any!) {
        
        refreshVIC()
    }
    
    func _displayModeAction(_ value: DisplayMode) {
        
        track("value = \(value)")
        
        let info = c64.vic.getInfo()
        let oldValue = info.displayMode
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._displayModeAction(oldValue)
            }
            undoManager?.setActionName("Set Display Mode")
            c64.vic.setDisplayMode(value)
            refreshVIC()
        }
    }
    
    @IBAction func displayModeAction(_ sender: Any!) {
        
        track()
        
        let sender = sender as! NSPopUpButton
        let value = UInt32(sender.selectedTag())
        _displayModeAction(DisplayMode(rawValue: value))
    }
    
    func _screenGeometryAction(_ value: ScreenGeometry) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.screenGeometry
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._screenGeometryAction(oldValue)
            }
            undoManager?.setActionName("Set Screen Geometry")
            c64.vic.setScreenGeometry(value)
            refreshVIC()
        }
    }
    
    @IBAction func screenGeometryAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt32(sender.selectedTag())
        _screenGeometryAction(ScreenGeometry(rawValue: value))
    }

    func _memoryBankAction(_ value: UInt16) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.memoryBankAddr
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._memoryBankAction(oldValue)
            }
            undoManager?.setActionName("Set Memory Bank")
            c64.vic.setMemoryBankAddr(value)
            refreshVIC()
        }
    }
    
    @IBAction func memoryBankAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt16(sender.selectedTag())
        _memoryBankAction(value)
    }
 
    func _screenMemoryAction(_ value: UInt16) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.screenMemoryAddr
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._screenMemoryAction(oldValue)
            }
            undoManager?.setActionName("Set Screen Memory Address")
            c64.vic.setScreenMemoryAddr(value)
            refreshVIC()
        }
    }
    
    @IBAction func screenMemoryAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt16(sender.selectedTag())
        _screenMemoryAction(value)
    }

    func _characterMemoryAction(_ value: UInt16) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.characterMemoryAddr
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._characterMemoryAction(oldValue)
            }
            undoManager?.setActionName("Set Character Memory Address")
            c64.vic.setCharacterMemoryAddr(value)
            refreshVIC()
        }
    }
    
    @IBAction func characterMemoryAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = UInt16(sender.selectedTag())
        _characterMemoryAction(value)
    }
    
    func _dxAction(_ value: UInt8) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.dx
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._dxAction(value)
            }
            undoManager?.setActionName("Set X Scroll")
            c64.vic.setHorizontalRasterScroll(value)
            refreshVIC()
        }
    }
    
    @IBAction func dxAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _dxAction(UInt8(sender.intValue))
    }
    
    @IBAction func dxStepperAction(_ sender: Any!) {
        
        let sender = sender as! NSStepper
        _dxAction(UInt8(sender.intValue & 0x07))
    }

    func _dyAction(_ value: UInt8) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.dy
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._dyAction(value)
            }
            undoManager?.setActionName("Set Y Scroll")
            c64.vic.setVerticalRasterScroll(value)
            refreshVIC()
        }
    }
    
    @IBAction func dyAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _dyAction(UInt8(sender.intValue))
    }
    
    @IBAction func dyStepperAction(_ sender: Any!) {
        
        let sender = sender as! NSStepper
        _dyAction(UInt8(sender.intValue & 0x07))
    }
    
    // Sprites
    
    func spriteSelectAction(_ sender: Any!) {
        
        refreshVIC()
    }
    
    func _spriteEnableAction(_ value: (Int,Bool)) {
        
        let sprite = value.0
        let info = c64.vic.getSpriteInfo(sprite)
        let oldValue = info.enabled
        let newValue = value.1

        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteEnableAction((sprite, oldValue))
            }
            undoManager?.setActionName(newValue ? "Enable Sprite" : "Disable Sprite")
            c64.vic.setSpriteEnabled(sprite, value: newValue)
            refreshVIC()
        }
    }
    
    @IBAction func spriteEnableAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteEnableAction((sprite, sender.intValue != 0))
    }
 
    func _spriteXAction(_ value: (Int,UInt16)) {
        
        let sprite = value.0
        let info = c64.vic.getSpriteInfo(sprite)
        let oldValue = info.x
        let newValue = value.1
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteXAction(value)
            }
            undoManager?.setActionName("Set Sprite X Coordinate")
            c64.vic.setSpriteX(sprite, value: Int(newValue))
            refreshVIC()
        }
    }
    
    @IBAction func spriteXAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _spriteXAction((sprite, UInt16(sender.intValue)))
    }
    
    func _spriteYAction(_ value: (Int,UInt16)) {
        
        let sprite = value.0
        let info = c64.vic.getSpriteInfo(sprite)
        let oldValue = info.x
        let newValue = value.1
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteYAction(value)
            }
            undoManager?.setActionName("Set Sprite Y Coordinate")
            c64.vic.setSpriteY(sprite, value: Int(newValue))
            refreshVIC()
        }
    }
    
    @IBAction func spriteYAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _spriteYAction((sprite, UInt16(sender.intValue)))
    }
    
    func _spriteExpandXAction(_ value: (Int,Bool)) {
        
        let sprite = value.0
        let info = c64.vic.getSpriteInfo(sprite)
        let oldValue = info.expandX
        let newValue = value.1
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteExpandXAction((sprite, oldValue))
            }
            undoManager?.setActionName(newValue ? "Expand Sprite" : "Shrink Sprite")
            c64.vic.setSpriteStretchX(sprite, value: newValue)
            refreshVIC()
        }
    }
    
    @IBAction func spriteExpandXAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteExpandXAction((sprite, sender.intValue != 0))
    }
    
    func _spriteExpandYAction(_ value: (Int,Bool)) {
        
        let sprite = value.0
        let info = c64.vic.getSpriteInfo(sprite)
        let oldValue = info.expandY
        let newValue = value.1
        
        if (newValue != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteExpandYAction((sprite, oldValue))
            }
            undoManager?.setActionName(newValue ? "Expand Sprite" : "Shrink Sprite")
            c64.vic.setSpriteStretchY(sprite, value: newValue)
            refreshVIC()
        }
    }
    
    @IBAction func spriteExpandYAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteExpandYAction((sprite, sender.intValue != 0))
    }
    
    func _spriteMulticolorAction(_ value: (Int,Bool)) {
        
        // track("\(value.0) \(value.1)")
        let info = c64.vic.getSpriteInfo(value.0)
        let oldValue = info.multicolor
        let actionName = oldValue ? "Set Multicolor Bit" : "Clear Multicolor Bit"
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteMulticolorAction((value.0,oldValue))
            }
            undoManager?.setActionName(actionName)
            c64.vic.setSpriteMulticolor(value.0, value: value.1)
            refreshVIC()
        }
    }
    
    @IBAction func spriteMulticolorAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = sender.selectedTag() != 0
        _spriteMulticolorAction((sprite, value))
    }
 
    func _spritePriorityAction(_ value: (Int,Bool)) {
        
        let info = c64.vic.getSpriteInfo(value.0)
        let oldValue = info.multicolor
        let actionName = oldValue ? "Set Priority Bit" : "Clear Priority Bit"
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spritePriorityAction((value.0,oldValue))
            }
            undoManager?.setActionName(actionName)
            c64.vic.setSpritePriority(value.0, value: value.1)
            refreshVIC()
        }
    }
    
    @IBAction func spritePriorityAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = sender.selectedTag() != 0
        _spritePriorityAction((sprite, value))
    }
    
    func _spriteSpriteCollisionIrqAction(_ value: Bool) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.spriteCollisionIrqEnabled
        let actionName = oldValue ? "Enable IRQ" : "Disable IRQ"
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteSpriteCollisionIrqAction(oldValue)
            }
            undoManager?.setActionName(actionName)
            c64.vic.setIrqOnSpriteSpriteCollision(value)
            refreshVIC()
        }
    }
    
    @IBAction func spriteSpriteCollisionIrqAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteSpriteCollisionIrqAction(sender.intValue != 0)
    }
    
    func _spriteBackgroundCollisionIrqAction(_ value: Bool) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.backgroundCollisionIrqEnabled
        let actionName = oldValue ? "Enable IRQ" : "Disable IRQ"
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteBackgroundCollisionIrqAction(oldValue)
            }
            undoManager?.setActionName(actionName)
            c64.vic.setIrqOnSpriteBackgroundCollision(value)
            refreshVIC()
        }
    }
    
    @IBAction func spriteBackgroundCollisionIrqAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteBackgroundCollisionIrqAction(sender.intValue != 0)
    }
    
    func _rasterIrqEnabledAction(_ value: Bool) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.rasterIrqEnabled
        let actionName = oldValue ? "Enable IRQ" : "Disable IRQ"
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._rasterIrqEnabledAction(oldValue)
            }
            undoManager?.setActionName(actionName)
            c64.vic.setRasterInterruptEnabled(value)
            refreshVIC()
        }
    }
    
    @IBAction func rasterIrqEnabledAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _rasterIrqEnabledAction(sender.intValue != 0)
    }
    
    func _irqRasterlineAction(_ value: UInt16) {
        
        let info = c64.vic.getInfo()
        let oldValue = info.irqRasterline
        
        if (value != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._irqRasterlineAction(value)
            }
            undoManager?.setActionName("Set IRQ Rasterline")
            c64.vic.setRasterInterruptLine(value)
            refreshVIC()
        }
    }
    
    @IBAction func irqRasterlineAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _irqRasterlineAction(UInt16(sender.intValue))
    }
    
    private func color(colorTag: Int, spriteNr: Int) -> UInt8 {
        
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(spriteNr)
        
        switch (colorTag) {
        case 0: return info.borderColor
        case 1: return info.backgroundColor0
        case 2: return info.backgroundColor1
        case 3: return info.backgroundColor2
        case 4: return info.backgroundColor3
        case 5: return sinfo.extraColor1
        case 6: return sinfo.extraColor2
        case 7: return sinfo.color
        default: assert(false); return 0
        }
    }
    
    func _colorAction(_ value: (Int,Int,UInt8)) {
        
        let oldColor = color(colorTag: value.0, spriteNr: value.1)
        if (oldColor == value.2) {
            return
        }
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._colorAction((value.0, value.1, oldColor))
        }
        undoManager?.setActionName("Set Color")
        
        switch (value.0) {
        case 0,1,2,3,4,5,6:
            c64.mem.pokeIO(0xD020 + UInt16(value.0), value: value.2)
            break
        case 7:
            c64.mem.pokeIO(0xD027 + UInt16(sprite), value: value.2)
            break
        default:
            assert(false)
            break
        }
        
        refreshVIC()
    }
    
    @IBAction func colorAction(_ sender: Any!) {
        
        track()
        
        let sender = sender as! NSButton
        let oldColor = color(colorTag: sender.tag, spriteNr: sprite)
        _colorAction((sender.tag, sprite, (oldColor + 1) % 16))
    }
    
}
