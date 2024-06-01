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
#include "Emulator.h"

namespace vc64 {

void
FinalIII::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << std::endl;

        os << tab("QD Pin");
        os << dec(qD) << std::endl;
        os << tab("Freeze Button Pressed");
        os << dec(freeezeButtonIsPressed) << std::endl;
    }
}

void
FinalIII::resetCartConfig()
{
    expansionPort.setCartridgeMode(CRTMODE_16K);
}

void
FinalIII::_reset(bool hard)
{
    qD = true;
    bankIn(0);
}

u8
FinalIII::peekIO1(u16 addr)
{
    // I/O space 1 mirrors $1E00 to $1EFF from ROML
    return peekRomL(addr & 0x1FFF);
}

u8
FinalIII::spypeekIO1(u16 addr) const
{
    return spypeekRomL(addr & 0x1FFF);
}

u8
FinalIII::peekIO2(u16 addr)
{
    // I/O space 2 space mirrors $1F00 to $1FFF from ROML
    return peekRomL(addr & 0x1FFF);
}

u8
FinalIII::spypeekIO2(u16 addr) const
{
    return spypeekRomL(addr & 0x1FFF);
}

void
FinalIII::pokeIO2(u16 addr, u8 value) {

    // The control register is mapped to address 0xFF in I/O space 2.
    if (addr == 0xDFFF && writeEnabled()) {
        setControlReg(value);
    }
}

void
FinalIII::nmiDidTrigger()
{
    if (freeezeButtonIsPressed) {

        trace(CRT_DEBUG, "NMI while freeze button is pressed.\n");

        /* After the NMI has been processed by the CPU, the cartridge's counter
         * has reached a value that overflows qD to 0. This has two side
         * effects. First, the Game line switches to 0. Second, because qD is
         * also connectec to the counter's enable pin, the counter freezes. This
         * keeps qD low until the freeze button is released by the user.
         */
        qD = false;
        updateGame();
    }
}

void
FinalIII::setControlReg(u8 value)
{
    control = value;

    // Update external lines
    updateNMI();
    updateGame();
    expansionPort.setExromLine(exrom());

    // Switch memory bank
    bankIn(control & 0x03);

}

bool
FinalIII::writeEnabled() const
{
    return !hidden() || freeezeButtonIsPressed;
}

void
FinalIII::updateNMI()
{
    if (nmi() && !freeezeButtonIsPressed) {
        cpu.releaseNmiLine(INTSRC_EXP);
    } else {
        cpu.pullDownNmiLine(INTSRC_EXP);
    }
}

void
FinalIII::updateGame()
{
    expansionPort.setGameLine(game() && qD);
}

const char *
FinalIII::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Freeze" : nr == 2 ? "Reset" : "";
}

void
FinalIII::pressButton(isize nr)
{
    assert(nr <= numButtons());
    assert(isEmulatorThread());

    trace(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr));

    switch (nr) {

        case 1: // Freeze

            freeezeButtonIsPressed = true;
            updateNMI();
            break;

        case 2: // Reset

            c64.softReset();
            break;
    }
}

void
FinalIII::releaseButton(isize nr)
{
    switch (nr) {
            
        case 1: // Freeze
            
            freeezeButtonIsPressed = false;
            qD = true;
            updateNMI();
            updateGame();
            break;
    }
}

}
