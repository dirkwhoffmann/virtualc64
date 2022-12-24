// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class Reu : public Cartridge {

private:

    // TODO
    u8 dummy = 0;


    //
    // Initializing
    //

public:

    Reu(C64 &ref) : Cartridge(ref) { };
    Reu(C64 &ref, isize kb) : Reu(ref) { setRamCapacity(kb * 1024); }
    const char *getDescription() const override { return "REU"; }
    CartridgeType getCartridgeType() const override { return CRT_REU; }

private:

    void _reset(bool hard) override;


    //
    // Serializing
    //

private:

    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker

        << dummy;
    }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }

    isize __size() { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }

    isize _size() override { return Cartridge::_size() + __size(); }
    isize _load(const u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    isize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }


    //
    // Accessing cartridge memory
    //

public:

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;

private:

};
