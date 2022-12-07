// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ParCableTypes.h"
#include "DriveTypes.h"
#include "SubComponent.h"

namespace vc64 {

class ParCable : public SubComponent {
    
    //
    // Initializing
    //
    
public:
    
    ParCable(C64 &ref);
    
    
    //
    // Methods from C64Object
    //

private:
    
    const char *getDescription() const override { return "ParCable"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from C64Component
    //

private:
    
    void _reset(bool hard) override;

    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Using
    //
    
public:
    
    // Returns the current value on the cable
    u8 getValue() const;

    // Sends a handshake signal
    void driveHandshake();
    void c64Handshake();
    
private:
    
    void c64Handshake(Drive &drive);
    
private:
    
    // Collects port values
    u8 getCIA() const;
    u8 getVIA(const Drive &drive) const;
    u8 getPIA(const Drive &drive) const;
};

}
