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

#include "BasicTypes.h"

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
    static const C64Key del;             ///< INST/DEL key
    static const C64Key ret;             ///< RETURN key
    static const C64Key curLeftRight;    ///< Cursor left/right key
    static const C64Key F7F8;            ///< F7/F8 key
    static const C64Key F1F2;            ///< F1/F2 key
    static const C64Key F3F4;            ///< F3/F4 key
    static const C64Key F5F6;            ///< F5/F6 key
    static const C64Key curUpDown;       ///< Cursor up/down key

    // Second row in key matrix
    static const C64Key digit3;          ///< 3 key
    static const C64Key W;               ///< Key W
    static const C64Key A;               ///< Key A
    static const C64Key digit4;          ///< 4 key
    static const C64Key Z;               ///< Key Z
    static const C64Key S;               ///< Key S
    static const C64Key E;               ///< Key E
    static const C64Key leftShift;       ///< Left SHIFT key
    
    // Third row in key matrix
    static const C64Key digit5;          ///< 5 key
    static const C64Key R;               ///< Key R
    static const C64Key D;               ///< Key D
    static const C64Key digit6;          ///< 6 key
    static const C64Key C;               ///< Key C
    static const C64Key F;               ///< Key F
    static const C64Key T;               ///< Key T
    static const C64Key X;               ///< Key X
    
    // Fourth row in key matrix
    static const C64Key digit7;          ///< 7 key
    static const C64Key Y;               ///< Key Y
    static const C64Key G;               ///< Key G
    static const C64Key digit8;          ///< 8 key
    static const C64Key B;               ///< Key B
    static const C64Key H;               ///< Key H
    static const C64Key U;               ///< Key U
    static const C64Key V;               ///< Key V
    
    // Fifth row in key matrix
    static const C64Key digit9;          ///< 9 key
    static const C64Key I;               ///< Key I
    static const C64Key J;               ///< Key J
    static const C64Key digit0;          ///< 0 key
    static const C64Key M;               ///< Key M
    static const C64Key K;               ///< Key K
    static const C64Key O;               ///< Key O
    static const C64Key N;               ///< Key N
    
    // Sixth row in key matrix
    static const C64Key plus;            ///< + key
    static const C64Key P;               ///< Key P
    static const C64Key L;               ///< Key L
    static const C64Key minus;           ///< - key
    static const C64Key period;          ///< . key
    static const C64Key colon;           ///< : key
    static const C64Key at;              ///< @ key
    static const C64Key comma;           ///< , key
    
    // Seventh row in key matrix
    static const C64Key pound;           ///< Pound sign key
    static const C64Key asterisk;        ///< * key
    static const C64Key semicolon;       ///< ; key
    static const C64Key home;            ///< CLR/HOME key
    static const C64Key rightShift;      ///< Right SHIFT key
    static const C64Key equal;           ///< = key
    static const C64Key upArrow;         ///< Up arrow key
    static const C64Key slash;           ///< / key
    
    // Eights row in key matrix
    static const C64Key digit1;          ///< 1 key
    static const C64Key leftArrow;       ///< Left arrow key
    static const C64Key control;         ///< CTRL key
    static const C64Key digit2;          ///< 2 key
    static const C64Key space;           ///< SPACE key
    static const C64Key commodore;       ///< Commodore key key
    static const C64Key Q;               ///< Key Q
    static const C64Key runStop;         ///< RUN/STOP key
    
    // Keys not represented in the keyboard matrix
    static const C64Key restore;         ///< RESTORE (not part of the keyboard matrix) key
    static const C64Key shiftLock;       ///< SHIFT LOCK (not part of the keyboard matrix) key

    
    //
    // Initializing
    //
    
    /// Creates a key from its identifier (0 ... 65)
    C64Key(isize nr = 0);
    /// Creates a key from its position in the keyboard matrix
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
