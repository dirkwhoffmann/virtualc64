// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include <vector>

struct C64Key {
  
    // Unique key identifier (0 .. 65)
    isize nr = -1;
    
    // Row and column indices
    isize row = -1;
    isize col = -1;
    
    
    //
    // Constants
    //
    
    // First row in key matrix
    static const C64Key del;
    static const C64Key ret;
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
    
    // Restore key
    static const C64Key restore;
    
    
    //
    // Initializing
    //
    
    C64Key(isize nr);
    C64Key(isize row, isize col);

    
    //
    // Translating
    //
    
    static std::vector<C64Key> translate(char c);
};
