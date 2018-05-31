//
//  VICPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 24.05.18.
//

import Foundation

extension MyController {
    
    func refreshVIC() {
        
        let info = c64.vic.getInfo()
        let sinfo = c64.vic.getSpriteInfo(spriteSelector.indexOfSelectedItem)
        
        vicRasterline.intValue = Int32(info.rasterline)
        vicCycle.intValue = Int32(info.cycle)
        vicXCounter.intValue = Int32(info.xCounter)
        vicBadLine.state = info.badLine ? .on : .off
        vicDisplayMode.selectItem(withTag: Int(info.displayMode.rawValue))
        if let cell = vicBorderColor.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(info.borderColor))
        }
        if let cell = vicBackgroundColor0.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(info.backgroundColor0))
        }
        if let cell = vicBackgroundColor1.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(info.backgroundColor1))
        }
        if let cell = vicBackgroundColor2.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(info.backgroundColor2))
        }
        if let cell = vicBackgroundColor3.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(info.backgroundColor3))
        }
        vicScreenGeometry.selectItem(withTag: Int(info.screenGeometry.rawValue))
        vicDx.intValue = Int32(info.dx)
        vicDy.intValue = Int32(info.dy)
        vicVerticalFrameFlipflop.state = info.verticalFrameFlipflop ? .on : .off
        vicHorizontalFrameFlipflop.state = info.horizontalFrameFlipflop ? .on : .off
        vicMemoryBankAddr.selectItem(withTag: Int(info.memoryBankAddr))
        vicScreenMemoryAddr.selectItem(withTag: Int(info.screenMemoryAddr))
        vicCharacterMemoryAddr.selectItem(withTag: Int(info.characterMemoryAddr))
        vicIrqRasterline.intValue = Int32(info.irqRasterline)
        vicIrqLine.state = info.irqLine ? .on : .off
        
        spriteEnabled.state = sinfo.enabled ? .on : .off
        spriteX.intValue = Int32(sinfo.x)
        spriteY.intValue = Int32(sinfo.y)
        if let cell = spriteColor.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(sinfo.color))
        }
        spriteIsMulticolor.state = sinfo.multicolor ? .on : .off
        if let cell = spriteExtraColor1.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(sinfo.extraColor1))
        }
        if let cell = spriteExtraColor2.cell as? NSTextFieldCell {
            cell.backgroundColor = c64.vic.color(Int(sinfo.extraColor2))
        }
        spriteExpandX.state = sinfo.expandX ? .on : .off
        spriteExpandY.state = sinfo.expandY ? .on : .off
        spritePriority.state = sinfo.priority ? .on : .off
        spriteCollidesWithSprite.state = sinfo.collidesWithSprite ? .on : .off
        spriteSpriteIrqEnabled.state = sinfo.spriteCollisionIrqEnabled ? .on : .off
        spriteCollidesWithBackground.state = sinfo.collidesWithBackground ? .on : .off
        spriteBackgroundIrqEnabled.state  = sinfo.backgroundCollisionIrqEnabled ? .on : .off
    }
}
