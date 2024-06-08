// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "C64.h"

namespace vc64 {

void
VICII::cacheInfo(VICIIInfo &result) const
{
    {   SYNCHRONIZED

        u8 ctrl1 = reg.current.ctrl1;
        u8 ctrl2 = reg.current.ctrl2;

        result.scanline = c64.scanline;
        result.rasterCycle = c64.rasterCycle;
        result.yCounter = yCounter;
        result.xCounter = xCounter;
        result.vc = vc;
        result.vcBase = vcBase;
        result.rc = rc;
        result.vmli = vmli;

        result.ctrl1 = ctrl1;
        result.ctrl2 = ctrl2;
        result.dy = ctrl1 & 0x07;
        result.dx = ctrl2 & 0x07;
        result.denBit = DENbit();
        result.badLine = badLine;
        result.displayState = displayState;
        result.vblank = vblank;
        result.screenGeometry = getScreenGeometry();
        result.frameFF = flipflops.current;
        result.displayMode = reg.current.mode;
        result.borderColor = reg.current.colors[COLREG_BORDER];
        result.bgColor0 = reg.current.colors[COLREG_BG0];
        result.bgColor1 = reg.current.colors[COLREG_BG1];
        result.bgColor2 = reg.current.colors[COLREG_BG2];
        result.bgColor3 = reg.current.colors[COLREG_BG3];

        result.memSelect = memSelect;
        result.ultimax = ultimax;
        result.memoryBankAddr = bankAddr;
        result.screenMemoryAddr = (u16)(VM13VM12VM11VM10() << 6);
        result.charMemoryAddr = (CB13CB12CB11() << 10) % 0x4000;

        result.irqLine = rasterIrqLine;
        result.imr = imr;
        result.irr = irr;

        result.latchedLPX = latchedLPX;
        result.latchedLPY = latchedLPY;
        result.lpLine = lpLine;
        result.lpIrqHasOccurred = lpIrqHasOccurred;

        for (int i = 0; i < 8; i++) {

            spriteInfo[i].enabled = GET_BIT(reg.current.sprEnable, i);
            spriteInfo[i].x = reg.current.sprX[i];
            spriteInfo[i].y = reg.current.sprY[i];
            spriteInfo[i].color = reg.current.colors[COLREG_SPR0 + i];
            spriteInfo[i].extraColor1 = reg.current.colors[COLREG_SPR_EX1];
            spriteInfo[i].extraColor2 = reg.current.colors[COLREG_SPR_EX2];
            spriteInfo[i].multicolor = GET_BIT(reg.current.sprMC, i);
            spriteInfo[i].expandX = GET_BIT(reg.current.sprExpandX, i);
            spriteInfo[i].expandY = GET_BIT(reg.current.sprExpandY, i);
            spriteInfo[i].priority = GET_BIT(reg.current.sprPriority, i);
            spriteInfo[i].ssCollision = GET_BIT(spriteSpriteCollision, i);
            spriteInfo[i].sbCollision = GET_BIT(spriteBackgroundColllision, i);
        }
    }
}

void
VICII::cacheStats(VICIIStats &result) const
{
    {   SYNCHRONIZED

    }
}

void
VICII::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Bank address");
        os << hex(bankAddr) << std::endl;
        os << tab("Screen memory");
        os << hex(u16(VM13VM12VM11VM10() << 6)) << std::endl;
        os << tab("Character memory");
        os << hex(u16((CB13CB12CB11() << 10) % 0x4000)) << std::endl;
        os << tab("X scroll");
        os << dec(reg.current.ctrl2 & 0x07) << std::endl;
        os << tab("Y scroll");
        os << dec(reg.current.ctrl1 & 0x07) << std::endl;
        os << tab("Control register 1");
        os << hex(reg.current.ctrl1) << std::endl;
        os << tab("Control register 2");
        os << hex(reg.current.ctrl2) << std::endl;
        os << tab("Display mode");
        os << DisplayModeEnum::key(reg.current.mode) << std::endl;

        os << tab("Bad Line");
        os << bol(badLine) << std::endl;
        os << tab("DENwasSetIn30");
        os << bol(DENwasSetInLine30) << std::endl;
        os << tab("VC");
        os << hex(vc) << std::endl;
        os << tab("VCBASE");
        os << hex(vcBase) << std::endl;
        os << tab("RC");
        os << hex(rc) << std::endl;
        os << tab("VMLI");
        os << hex(vmli) << std::endl;
        os << tab("BA Line");
        os << bol(baLine.current(), "low", "high") << std::endl;
        os << tab("MainFrameFF");
        os << bol(flipflops.current.main, "set", "cleared") << std::endl;
        os << tab("VerticalFrameFF");
        os << bol(flipflops.current.vertical, "set", "cleared") << std::endl;
        os << tab("DisplayState");
        os << bol(displayState, "on", "off") << std::endl;
        os << tab("SpriteDisplay");
        os << hex(spriteDisplay) << " / " << hex(spriteDisplayDelayed) << std::endl;
        os << tab("SpriteDma");
        os << hex(spriteDmaOnOff) << std::endl;
        os << tab("Y expansion");
        os << hex(expansionFF) << std::endl;
        os << tab("expansionFF");
        os << hex(expansionFF) << std::endl;
    }

    if (category == Category::Registers) {

        string addr[8] = {
            "$D000 - $D007", "$D008 - $D00F", "$D010 - $D017", "$D018 - $D01F",
            "$D020 - $D027", "$D028 - $D02F", "$D030 - $D037", "$D038 - $D03F" };

        for (isize i = 0; i < 6; i++) {
            os << tab(addr[i]);
            for (isize j = 0; j < 8; j++) {
                os << hex(spypeek((u16)(8 * i + j))) << " ";
            }
            os << std::endl;
        }
    }
}

void
VICII::clearStats()
{
    if (VICII_STATS) {

        double canvasTotal = stats.canvasFastPath + stats.canvasSlowPath;
        double spriteTotal = stats.spriteFastPath + stats.spriteSlowPath;
        double exitTotal = stats.quickExitHit + stats.quickExitMiss;

        msg("Canvas: Fast path: %ld Slow path: %ld Ratio: %f\n",
            stats.canvasFastPath,
            stats.canvasSlowPath,
            canvasTotal != 0 ? stats.canvasFastPath / canvasTotal : -1);

        msg("Sprites: Fast path: %ld Slow path: %ld Ratio: %f\n",
            stats.spriteFastPath,
            stats.spriteSlowPath,
            spriteTotal != 0 ? stats.spriteFastPath / spriteTotal : -1);

        msg("Exits: Hit: %ld Miss: %ld Ratio: %f\n",
            stats.quickExitHit,
            stats.quickExitMiss,
            exitTotal != 0 ? stats.quickExitHit / exitTotal : -1);

        memset(&stats, 0, sizeof(stats));
    }
}

SpriteInfo
VICII::getSpriteInfo(isize nr)
{
    return spriteInfo[nr];
}

i64
VICII::getOption(Option option) const
{
    switch (option) {

        case OPT_VICII_REVISION:        return config.awaiting;
        case OPT_VICII_POWER_SAVE:      return config.powerSave;
        case OPT_VICII_GRAY_DOT_BUG:    return config.grayDotBug;
        case OPT_GLUE_LOGIC:            return config.glueLogic;
        case OPT_VICII_HIDE_SPRITES:    return config.hideSprites;
        case OPT_VICII_SS_COLLISIONS:   return config.checkSSCollisions;
        case OPT_VICII_SB_COLLISIONS:   return config.checkSBCollisions;

        default:
            fatalError;
    }
}

void
VICII::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_VICII_REVISION:

            if (!VICIIRevisionEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, VICIIRevisionEnum::keyList());
            }
            return;

        case OPT_VICII_POWER_SAVE:
        case OPT_VICII_GRAY_DOT_BUG:
        case OPT_VICII_HIDE_SPRITES:
        case OPT_VICII_SS_COLLISIONS:
        case OPT_VICII_SB_COLLISIONS:

            return;

        case OPT_GLUE_LOGIC:

            if (!GlueLogicEnum::isValid(value)) {
                throw Error(ERROR_OPT_INV_ARG, GlueLogicEnum::keyList());
            }
            return;

        default:
            throw Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
VICII::setOption(Option opt, i64 value)
{
    checkOption(opt, value);
    
    switch (opt) {

        case OPT_VICII_REVISION:

            config.awaiting = VICIIRevision(value);

            // If the emulator is powered off, perform the change immediately
            if (isPoweredOff()) updateRevision();

            return;

        case OPT_VICII_POWER_SAVE:

            config.powerSave = bool(value);
            return;

        case OPT_VICII_GRAY_DOT_BUG:

            config.grayDotBug = bool(value);
            return;

        case OPT_VICII_HIDE_SPRITES:

            config.hideSprites = bool(value);
            return;

        case OPT_VICII_SS_COLLISIONS:

            config.checkSSCollisions = bool(value);
            return;

        case OPT_VICII_SB_COLLISIONS:

            config.checkSBCollisions = bool(value);
            return;

        case OPT_GLUE_LOGIC:

            config.glueLogic = GlueLogic(value);
            return;

        default:
            fatalError;
    }
}

}
