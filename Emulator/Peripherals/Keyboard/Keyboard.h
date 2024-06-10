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

#pragma once

#include "SubComponent.h"
#include "CmdQueue.h"
#include "C64Types.h"
#include "C64Key.h"
#include "Buffer.h"

namespace vc64 {

class Keyboard final : public SubComponent {

    Descriptions descriptions = {{

        .name           = "Keyboard",
        .description    = "Keyboard"
    }};

    // The keyboard matrix (indexed by row or by column)
    u8 kbMatrixRow[8] = { };
    u8 kbMatrixCol[8] = { };

    // The number of pressed keys in a certain row or column
    u8 kbMatrixRowCnt[8] = { };
    u8 kbMatrixColCnt[8] = { };
    
    // Indicates if the shift lock is currently pressed
    bool shiftLock = false;

    // Delayed keyboard commands (auto-typing)
    util::SortedRingBuffer<Cmd, 1024> pending;


    //
    // Methods
    //
    
public:
    
    Keyboard(C64 &ref) : SubComponent(ref) { }

    Keyboard& operator= (const Keyboard& other) {

        CLONE_ARRAY(kbMatrixRow)
        CLONE_ARRAY(kbMatrixCol)
        CLONE_ARRAY(kbMatrixRowCnt)
        CLONE_ARRAY(kbMatrixColCnt)
        CLONE(shiftLock)
        CLONE(pending)
        
        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker

        << kbMatrixRow
        << kbMatrixCol
        << kbMatrixRowCnt
        << kbMatrixColCnt
        << shiftLock;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _reset(bool hard) override;


    //
    // Accessing the keyboard matrix
    //
    
public:

    // Reads a column or row from the keyboard matrix
    u8 getColumnValues(u8 rowMask) const;
    u8 getRowValues(u8 columnMask) const;
    u8 getRowValues(u8 columnMask, u8 thresholdMask) const;

    // Checks whether a certain key is pressed
    bool isPressed(C64Key key) const;

    // Presses or releases a key
    void press(C64Key key);
    void release(C64Key key);
    void toggle(C64Key key) { isPressed(key) ? release(key) : press(key); }

    // Clears the keyboard matrix
    void releaseAll();


    //
    // Auto typing
    //
    
public:
    
    // Auto-types a string
    void autoType(const string &text);

    // Discards all pending key events and clears the keyboard matrix
    void abortAutoTyping();


    //
    // Processing commands and events
    //

public:

    // Processes a command from the command queue
    void processCommand(const Cmd &cmd);

    // Processes the next auto-type event
    void processKeyEvent(EventID id);
};

}
