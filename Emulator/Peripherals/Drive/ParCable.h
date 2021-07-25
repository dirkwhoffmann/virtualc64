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
#include "C64Component.h"

class ParCable : public C64Component {
    
    //
    // Initializing
    //
    
public:
    
    ParCable(C64 &ref);
    
    const char *getDescription() const override;
    
private:
    
    void _reset(bool hard) override;
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }    

    
    //
    // Using
    //
    
public:
    
    // Returns the cable value as seen from the C64 side
    u8 getValue() const;
    
    // Returns the cable value as seen from the drive side
    u8 getValue(DriveID id) const;
    
    // Sends a handshake signal
    void driveHandshake();
    void c64Handshake();
    
private:
    
    // Collects port values
    u8 getCIA() const;
    u8 getVIA(DriveID id) const;
};
