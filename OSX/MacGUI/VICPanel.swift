//
//  VICPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    func refreshVIC() {
        
        var info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(spriteSelector.indexOfSelectedItem)
        
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
        spriteIsMulticolor.state = sinfo.multicolor ? .on : .off
        spriteExtraColorCell1.backgroundColor = c64.vic.color(Int(sinfo.extraColor1))
        spriteExtraColorCell2.backgroundColor = c64.vic.color(Int(sinfo.extraColor2))
        spriteExpandX.state = sinfo.expandX ? .on : .off
        spriteExpandY.state = sinfo.expandY ? .on : .off
        spritePriority.state = sinfo.priority ? .on : .off
        spriteCollidesWithSprite.state = sinfo.collidesWithSprite ? .on : .off
        spriteSpriteIrqEnabled.state = sinfo.spriteCollisionIrqEnabled ? .on : .off
        spriteCollidesWithBackground.state = sinfo.collidesWithBackground ? .on : .off
        spriteBackgroundIrqEnabled.state  = sinfo.backgroundCollisionIrqEnabled ? .on : .off
    }
    
    func spriteNr() -> Int {
        return spriteSelector.indexOfSelectedItem
    }
    
    
    //
    // Action methods
    //
    
    func pokeVicReg(_ register:(UInt16), _ value:(UInt8)) {
        c64.mem.pokeIO(0xD000 + register, value: value)
    }
    
    @IBAction func selectSpriteAction(_ sender: Any!) {
        
        let sender = sender as! NSSegmentedControl
        let selectedSprite = sender.indexOfSelectedItem
        track("selectedSprite = \(selectedSprite)")
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
    
    func _dxAction(_ value: Int) {
        
        let info = c64.vic.getInfo()
        let oldValue = Int(info.dx)
        
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
        _dxAction(Int(sender.intValue))
    }

    func _dyAction(_ value: Int) {
        
        let info = c64.vic.getInfo()
        let oldValue = Int(info.dy)
        
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
        _dyAction(Int(sender.intValue))
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
        _spriteEnableAction((spriteSelector.indexOfSelectedItem, sender.intValue != 0))
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
            c64.vic.setSpriteX(sprite, value: Int32(newValue))
            refreshVIC()
        }
    }
    
    @IBAction func spriteXAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _spriteXAction((spriteSelector.indexOfSelectedItem, UInt16(sender.intValue)))
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
            c64.vic.setSpriteY(sprite, value: Int32(newValue))
            refreshVIC()
        }
    }
    
    @IBAction func spriteYAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        _spriteYAction((spriteSelector.indexOfSelectedItem, UInt16(sender.intValue)))
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
            c64.vic.setSpriteStretchXFlag(sprite, value: newValue)
            refreshVIC()
        }
    }
    
    @IBAction func spriteExpandXAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteExpandXAction((spriteSelector.indexOfSelectedItem, sender.intValue != 0))
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
            c64.vic.setSpriteStretchYFlag(sprite, value: newValue)
            refreshVIC()
        }
    }
    
    @IBAction func spriteExpandYAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _spriteExpandYAction((spriteSelector.indexOfSelectedItem, sender.intValue != 0))
    }
    
    func _spriteMulticolorAction(_ value: (Int,Bool)) {
        
        let info = c64.vic.getSpriteInfo(value.0)
        let oldValue = info.multicolor
        let actionName = oldValue ? "Set Multicolor Bit" : "Clear Multicolor Bit"
        
        if (value.1 != oldValue) {
            undoManager?.registerUndo(withTarget: self) {
                me in me._spriteMulticolorAction((value.0,oldValue))
            }
            undoManager?.setActionName(actionName)
            c64.vic.setSpriteMulticolorFlag(value.0, value: value.1)
            refreshVIC()
        }
    }
    
    @IBAction func spriteMulticolorAction(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        let value = sender.selectedTag() != 0
        _spriteMulticolorAction((spriteNr(), value))
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
        _spritePriorityAction((spriteNr(), value))
    }
    
    
    
    
    
    
    func _colorAction(_ value: (Int,Int,UInt8)) {
        
        track("\(value)")

        let colorField = value.0
        let spriteNr = value.1
        let newColor = value.2
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(spriteNr)
        
        var oldColor: UInt8
        switch (colorField) {
        case 0: // Border color
            oldColor = info.borderColor
            break
        case 1: // Background color 0
            oldColor = info.backgroundColor0
            break
        case 2: // Background color 1
            oldColor = info.backgroundColor1
            break
        case 3: // Background color 2
            oldColor = info.backgroundColor2
            break
        case 4: // Background color 3
            oldColor = info.backgroundColor3
            break
        case 5: // Sprite extra color 1
            oldColor = sinfo.extraColor1
            break
        case 6: // Sprite extra color 2
            oldColor = sinfo.extraColor2
            break
        case 7: // Sprite color
            oldColor = sinfo.color
            break
        default:
            assert(false)
            oldColor = 0
            break
        }
        
        if (oldColor == newColor) {
            return
        }
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._colorAction((colorField, spriteNr, newColor))
        }
        undoManager?.setActionName("Set Color")
        
        switch (colorField) {
        case 0,1,2,3,4,5,6:
            pokeVicReg(0x20 + UInt16(colorField), newColor)
            break
        case 7:
            pokeVicReg(0x27 + UInt16(spriteNr), newColor)
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
        let colorField = sender.tag
        let spriteNr = spriteSelector.indexOfSelectedItem
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(spriteNr)

        var newColor: UInt8
        switch (colorField) {
        case 0: // Border color
            newColor = (info.borderColor + 1) % 16
            break
        case 1: // Background color 0
            newColor = (info.backgroundColor0 + 1) % 16
            break
        case 2: // Background color 1
            newColor = (info.backgroundColor1 + 1) % 16
            break
        case 3: // Background color 2
            newColor = (info.backgroundColor2 + 1) % 16
            break
        case 4: // Background color 3
            newColor = (info.backgroundColor3 + 1) % 16
            break
        case 5: // Sprite extra color 1
            newColor = (sinfo.extraColor1 + 1) % 16
            break
        case 6: // Sprite extra color 2
            newColor = (sinfo.extraColor2 + 1) % 16
            break
        case 7: // Sprite color
            newColor = (sinfo.color + 1) % 16
            break
        default:
            assert(false)
            newColor = 0
            break
        }
    
        _colorAction((colorField, spriteNr, newColor))
    }
    
}
