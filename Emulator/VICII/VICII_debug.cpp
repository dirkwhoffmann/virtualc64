// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"


ScreenGeometry
VICII::getScreenGeometry(void)
{
    unsigned rows = GET_BIT(reg.current.ctrl1, 3) ? 25 : 24;
    unsigned cols = GET_BIT(reg.current.ctrl2, 3) ? 40 : 38;
    
    if (cols == 40) {
        return rows == 25 ? COL_40_ROW_25 : COL_40_ROW_24;
    } else {
        return rows == 25 ? COL_38_ROW_25 : COL_38_ROW_24;
    }
}


//
// Sprites
//

void
VICII::setShowIrqLines(bool show)
{
    suspend();
    markIRQLines = show;
    resume();
}

void
VICII::setShowDmaLines(bool show)
{
    suspend();
    markDMALines = show;
    resume();
}

void
VICII::setHideSprites(bool hide)
{
    suspend();
    hideSprites = hide;
    resume();
}

void
VICII::setSpriteSpriteCollisionFlag(bool b)
{
    suspend();
    spriteSpriteCollisionEnabled = b;
    resume();
}

void
VICII::toggleSpriteSpriteCollisionFlag()
{
    suspend();
    spriteSpriteCollisionEnabled = !spriteSpriteCollisionEnabled;
    resume();
}

void
VICII::setSpriteBackgroundCollisionFlag(bool b)
{
    suspend();
    spriteBackgroundCollisionEnabled = b;
    resume();
}

void
VICII::toggleSpriteBackgroundCollisionFlag()
{
    suspend();
    spriteBackgroundCollisionEnabled = !spriteBackgroundCollisionEnabled;
    resume();
}



