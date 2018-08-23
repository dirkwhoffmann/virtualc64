/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, 2018
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

VICInfo
VIC::getInfo()
{
    VICInfo info;
    
    uint8_t ctrl1 = control1.current();
    uint8_t ctrl2 = control2.current();

    info.rasterline = c64->rasterline;
    info.cycle = c64->rasterlineCycle;
    info.xCounter = xCounter;
    info.badLine = badLineCondition;
    info.ba = (BAlow == 0);
    info.displayMode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
    info.borderColor = borderColor.current() & 0xF;
    info.backgroundColor0 = bgColor[0].current() & 0xF;
    info.backgroundColor1 = bgColor[1].current() & 0xF;
    info.backgroundColor2 = bgColor[2].current() & 0xF;
    info.backgroundColor3 = bgColor[3].current() & 0xF;
    info.screenGeometry = getScreenGeometry();
    info.dx = control2.current() & 0x07;
    info.dy = control1.current() & 0x07;
    info.verticalFrameFlipflop = p.verticalFrameFF;
    info.horizontalFrameFlipflop = p.mainFrameFF;
    info.memoryBankAddr = bankAddr;
    info.screenMemoryAddr = VM13VM12VM11VM10() << 6;
    info.characterMemoryAddr = (CB13CB12CB11() << 10) % 0x4000;
    info.imr = imr;
    info.irr = irr;
    info.spriteCollisionIrqEnabled =  GET_BIT(imr, 2);
    info.backgroundCollisionIrqEnabled = GET_BIT(imr, 1);
    info.rasterIrqEnabled = GET_BIT(imr, 1);
    info.irqRasterline = rasterInterruptLine();
    info.irqLine = (imr & irr) != 0;
    
    return info;
}

SpriteInfo
VIC::getSpriteInfo(unsigned i)
{
    SpriteInfo info;
    
    info.enabled = GET_BIT(spriteOnOff.current(), i);
    info.x = p.spriteX[i];
    info.y = iomem[1 + 2*i];
    info.color = sprColor[i].current() & 0xF;
    info.multicolor = GET_BIT(iomem[0x1C], i);
    info.extraColor1 = sprExtraColor1.current() & 0xF;
    info.extraColor2 = sprExtraColor2.current() & 0xF;
    info.expandX = GET_BIT(p.spriteXexpand, i);
    info.expandY = GET_BIT(iomem[0x17], i);
    info.priority = GET_BIT(iomem[0x1B], i);
    info.collidesWithSprite = GET_BIT(iomem[0x1E], i);
    info.collidesWithBackground = GET_BIT(iomem[0x1F], i);
    
    return info;
}

void
VIC::setMemoryBankAddr(uint16_t addr)
{
    assert(addr % 0x4000 == 0);
    
    c64->suspend();
    bankAddr = addr;
    c64->resume();
}

void
VIC::setScreenMemoryAddr(uint16_t addr)
{
    assert((addr & ~0x3C00) == 0);
    
    c64->suspend();
    addr >>= 6;
    iomem[0x18] = (iomem[0x18] & ~0xF0) | (addr & 0xF0);
    c64->resume();
}

void
VIC::setCharacterMemoryAddr(uint16_t addr)
{
    assert((addr & ~0x3800) == 0);
    
    c64->suspend();
    addr >>= 10;
    iomem[0x18] = (iomem[0x18] & ~0x0E) | (addr & 0x0E);
    c64->resume();
}

void
VIC::setDisplayMode(DisplayMode m)
{
    c64->suspend();
    control1.write((control1.current() & ~0x60) | (m & 0x60));
    control2.write((control2.current() & ~0x10) | (m & 0x10));
    c64->resume();
}

void
VIC::setNumberOfRows(unsigned rs)
{
    assert(rs == 24 || rs == 25);
    
    c64->suspend();
    uint8_t value = control1.current();
    WRITE_BIT(value, 3, rs == 25);
    control1.write(value);
    c64->resume();
}

void
VIC::setNumberOfColumns(unsigned cs)
{
    assert(cs == 38 || cs == 40);
    
    c64->suspend();
    uint8_t value = control2.current();
    WRITE_BIT(value, 3, cs == 40);
    control2.write(value);
    c64->resume();
}

ScreenGeometry
VIC::getScreenGeometry(void)
{
    unsigned rows = GET_BIT(control1.current(), 3) ? 25 : 24;
    unsigned cols = GET_BIT(control2.current(), 3) ? 40 : 38;
    
    if (cols == 40) {
        return rows == 25 ? COL_40_ROW_25 : COL_40_ROW_24;
    } else {
        return rows == 25 ? COL_38_ROW_25 : COL_38_ROW_24;
    }
}

void
VIC::setScreenGeometry(ScreenGeometry mode)
{
    c64->suspend();
    setNumberOfRows((mode == COL_40_ROW_25 || mode == COL_38_ROW_25) ? 25 : 24);
    setNumberOfColumns((mode == COL_40_ROW_25 || mode == COL_40_ROW_24) ? 40 : 38);
    c64->resume();
}

void
VIC::setVerticalRasterScroll(uint8_t offset)
{
    assert(offset < 8);
    
    c64->suspend();
    control1.write((control1.current() & 0xF8) | (offset & 0x07));
    c64->resume();
}

void
VIC::setHorizontalRasterScroll(uint8_t offset)
{
    assert(offset < 8);
    
    c64->suspend();
    control2.write((control2.current() & 0xF8) | (offset & 0x07));
    c64->resume();
}

void
VIC::setRasterInterruptLine(uint16_t line)
{
    c64->suspend();
    iomem[0x12] = line & 0xFF;
    if (line > 0xFF) {
        control1.write(control1.current() | 0x80);
    } else {
        control1.write(control1.current() & 0x7F);
    }
    c64->resume();
}

void
VIC::setRasterInterruptEnable(bool b)
{
    c64->suspend();
    WRITE_BIT(imr, 1, b);
    c64->resume();
}

void
VIC::toggleRasterInterruptFlag()
{
    c64->suspend();
    TOGGLE_BIT(imr, 1);
    c64->resume();
}


//
// Sprites
//

void
VIC::setSpriteX(unsigned nr, uint16_t x)
{
    assert(nr < 8);
    x = MIN(x, 511);
    
    c64->suspend();
    p.spriteX[nr] = x;
    iomem[2*nr] = x & 0xFF;
    if (x & 0x100) SET_BIT(iomem[0x10],nr); else CLR_BIT(iomem[0x10],nr);
    c64->resume();
}

void
VIC::setSpriteY(unsigned nr, uint8_t y)
{
    assert(nr < 8);
    
    c64->suspend();
    iomem[1+2*nr] = y;
    c64->resume();
}

void
VIC::setSpriteColor(unsigned nr, uint8_t color)
{
    assert(nr < 8);
    
    c64->suspend();
    sprColor[nr].write(color);
    c64->resume();
}

void
VIC::setSpriteEnabled(uint8_t nr, bool b)
{
    c64->suspend();
    WRITE_BIT(iomem[0x15], nr, b);
    c64->resume();
}

void
VIC::toggleSpriteEnabled(uint8_t nr)
{
    c64->suspend();
    TOGGLE_BIT(iomem[0x15], nr);
    c64->resume();
}

void
VIC::setIrqOnSpriteBackgroundCollision(bool b)
{
    c64->suspend();
    WRITE_BIT(imr, 1, b);
    c64->resume();
}

void
VIC::toggleIrqOnSpriteBackgroundCollision()
{
    c64->suspend();
    TOGGLE_BIT(imr, 1);
    c64->resume();
}

void
VIC::setIrqOnSpriteSpriteCollision(bool b)
{
    c64->suspend();
    WRITE_BIT(imr, 2, b);
    c64->resume();
}

void
VIC::toggleIrqOnSpriteSpriteCollision()
{
    c64->suspend();
    TOGGLE_BIT(imr, 2);
    c64->resume();
}

void
VIC::setSpritePriority(unsigned nr, bool b)
{
    assert(nr < 8);
    
    c64->suspend();
    WRITE_BIT(iomem[0x1B], nr, b);
    c64->resume();
}

void
VIC::toggleSpritePriority(unsigned nr)
{
    assert(nr < 8);
    
    c64->suspend();
    TOGGLE_BIT(iomem[0x1B], nr);
    c64->resume();
}

void
VIC::setSpriteMulticolor(unsigned nr, bool b)
{
    assert(nr < 8);
    
    c64->suspend();
    WRITE_BIT(iomem[0x1C], nr, b);
    c64->resume();
}

void
VIC::toggleMulticolorFlag(unsigned nr)
{
    assert(nr < 8);
    
    c64->suspend();
    TOGGLE_BIT(iomem[0x1C], nr);
    c64->resume();
}

void
VIC::setSpriteStretchY(unsigned nr, bool b)
{
    assert(nr < 8);
    
    c64->suspend();
    WRITE_BIT(iomem[0x17], nr, b);
    c64->resume();
}

void
VIC::spriteToggleStretchYFlag(unsigned nr)
{
    assert(nr < 8);
    
    c64->suspend();
    TOGGLE_BIT(iomem[0x17], nr);
    c64->resume();
}

void
VIC::setSpriteStretchX(unsigned nr, bool b)
{
    assert(nr < 8);
    
    c64->suspend();
    WRITE_BIT(p.spriteXexpand, nr, b);
    c64->resume();
}

void
VIC::spriteToggleStretchXFlag(unsigned nr)
{
    assert(nr < 8);

    c64->suspend();
    TOGGLE_BIT(p.spriteXexpand, nr);
    c64->resume();
}

void
VIC::setShowIrqLines(bool show)
{
    c64->suspend();
    markIRQLines = show;
    c64->resume();
}

void
VIC::setShowDmaLines(bool show)
{
    c64->suspend();
    markDMALines = show;
    c64->resume();
}

void
VIC::setHideSprites(bool hide)
{
    c64->suspend();
    hideSprites = !hide;
    c64->resume();
}

void
VIC::setSpriteSpriteCollisionFlag(bool b)
{
    c64->suspend();
    spriteSpriteCollisionEnabled = b;
    c64->resume();
}

void
VIC::toggleSpriteSpriteCollisionFlag()
{
    c64->suspend();
    spriteSpriteCollisionEnabled = !spriteSpriteCollisionEnabled;
    c64->resume();
}

void
VIC::setSpriteBackgroundCollisionFlag(bool b)
{
    c64->suspend();
    spriteBackgroundCollisionEnabled = b;
    c64->resume();
}

void
VIC::toggleSpriteBackgroundCollisionFlag()
{
    c64->suspend();
    spriteBackgroundCollisionEnabled = !spriteBackgroundCollisionEnabled;
    c64->resume();
}



