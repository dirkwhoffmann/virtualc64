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

#include "Types.h"
#include <vector>

namespace vc64 {

/** Representation of a key on the C64 keyboard
 */
struct C64Key {

    /** @brief  Unique key identifier (0 .. 65)
     */
    isize nr = -1;
    
    /** @brief  Row index of this key in the keyboard matrix
     *  @note   The RESTORE and the SHIFT LOCK key have no keyboard matrix
     *          representation.
     */
    isize row = -1;

    /** @brief  Column index of this key in the keyboard matrix
     *  @note   The RESTORE and the SHIFT LOCK key have no keyboard matrix
     *          representation.
     */
    isize col = -1;

    
    //
    // Constants
    //
    
    // First row in key matrix
    static const C64Key del;            //! Delete key
    static const C64Key ret;            //! Return key
    static const C64Key curLeftRight;
    static const C64Key F7F8;
    static const C64Key F1F2;
    static const C64Key F3F4;
    static const C64Key F5F6;
    static const C64Key curUpDown;

    // Second row in key matrix
    static const C64Key digit3;
    static const C64Key W;
    static const C64Key A;
    static const C64Key digit4;
    static const C64Key Z;
    static const C64Key S;
    static const C64Key E;
    static const C64Key leftShift;
    
    // Third row in key matrix
    static const C64Key digit5;
    static const C64Key R;
    static const C64Key D;
    static const C64Key digit6;
    static const C64Key C;
    static const C64Key F;
    static const C64Key T;
    static const C64Key X;
    
    // Fourth row in key matrix
    static const C64Key digit7;
    static const C64Key Y;
    static const C64Key G;
    static const C64Key digit8;
    static const C64Key B;
    static const C64Key H;
    static const C64Key U;
    static const C64Key V;
    
    // Fifth row in key matrix
    static const C64Key digit9;
    static const C64Key I;
    static const C64Key J;
    static const C64Key digit0;
    static const C64Key M;
    static const C64Key K;
    static const C64Key O;
    static const C64Key N;
    
    // Sixth row in key matrix
    static const C64Key plus;
    static const C64Key P;
    static const C64Key L;
    static const C64Key minus;
    static const C64Key period;
    static const C64Key colon;
    static const C64Key at;
    static const C64Key comma;
    
    // Seventh row in key matrix
    static const C64Key pound;
    static const C64Key asterisk;
    static const C64Key semicolon;
    static const C64Key home;
    static const C64Key rightShift;
    static const C64Key equal;
    static const C64Key upArrow;
    static const C64Key slash;
    
    // Eights row in key matrix
    static const C64Key digit1;
    static const C64Key leftArrow;
    static const C64Key control;
    static const C64Key digit2;
    static const C64Key space;
    static const C64Key commodore;
    static const C64Key Q;
    static const C64Key runStop;
    
    // Keys not represented in the keyboard matrix
    static const C64Key restore;
    static const C64Key shiftLock;

    
    //
    // Initializing
    //
    
    C64Key(isize nr = 0);
    C64Key(isize row, isize col);

    
    //
    // Translating
    //
    
    /** @brief      Translates a character into a series of C64 keys.
     *  @return     A vector of C64 keys.
     *  @note       The return type is a vector as some characters additionally
     *              require the shift key to be pressed.
     */
    static std::vector<C64Key> translate(char c);
};

}
