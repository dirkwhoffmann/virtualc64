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
#include "C64.h"

namespace vc64 {

void
SimonsBasic::_reset(bool hard)
{
    bankIn(0);
    bankIn(1);
}

u8
SimonsBasic::peekIO1(u16 addr)
{
    if (addr == 0xDE00) {
        expansionPort.setCartridgeMode(CRTMODE_8K);
    }
    return Cartridge::peekIO1(addr);
}

u8
SimonsBasic::spypeekIO1(u16 addr) const
{
    return Cartridge::spypeekIO1(addr);
}

void
SimonsBasic::pokeIO1(u16 addr, u8 value)
{
    if (addr == 0xDE00) {
        expansionPort.setCartridgeMode(CRTMODE_16K);
    }
}

}
