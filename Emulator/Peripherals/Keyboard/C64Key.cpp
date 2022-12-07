// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64Key.h"

namespace vc64 {

const C64Key C64Key::del(15);
const C64Key C64Key::ret(47);
const C64Key C64Key::curLeftRight(63);
const C64Key C64Key::F7F8(64);
const C64Key C64Key::F1F2(16);
const C64Key C64Key::F3F4(32);
const C64Key C64Key::F5F6(48);
const C64Key C64Key::curUpDown(62);

const C64Key C64Key::digit3(3);
const C64Key C64Key::W(19);
const C64Key C64Key::A(35);
const C64Key C64Key::digit4(4);
const C64Key C64Key::Z(51);
const C64Key C64Key::S(36);
const C64Key C64Key::E(20);
const C64Key C64Key::leftShift(50);

const C64Key C64Key::digit5(5);
const C64Key C64Key::R(21);
const C64Key C64Key::D(37);
const C64Key C64Key::digit6(6);
const C64Key C64Key::C(53);
const C64Key C64Key::F(38);
const C64Key C64Key::T(22);
const C64Key C64Key::X(52);

const C64Key C64Key::digit7(7);
const C64Key C64Key::Y(23);
const C64Key C64Key::G(39);
const C64Key C64Key::digit8(8);
const C64Key C64Key::B(55);
const C64Key C64Key::H(40);
const C64Key C64Key::U(24);
const C64Key C64Key::V(54);

const C64Key C64Key::digit9(9);
const C64Key C64Key::I(25);
const C64Key C64Key::J(41);
const C64Key C64Key::digit0(10);
const C64Key C64Key::M(57);
const C64Key C64Key::K(42);
const C64Key C64Key::O(26);
const C64Key C64Key::N(56);

const C64Key C64Key::plus(11);
const C64Key C64Key::P(27);
const C64Key C64Key::L(43);
const C64Key C64Key::minus(12);
const C64Key C64Key::period(59);
const C64Key C64Key::colon(44);
const C64Key C64Key::at(28);
const C64Key C64Key::comma(58);

const C64Key C64Key::pound(13);
const C64Key C64Key::asterisk(29);
const C64Key C64Key::semicolon(45);
const C64Key C64Key::home(14);
const C64Key C64Key::rightShift(61);
const C64Key C64Key::equal(46);
const C64Key C64Key::upArrow(30);
const C64Key C64Key::slash(60);

const C64Key C64Key::digit1(1);
const C64Key C64Key::leftArrow(0);
const C64Key C64Key::control(17);
const C64Key C64Key::digit2(2);
const C64Key C64Key::space(65);
const C64Key C64Key::commodore(49);
const C64Key C64Key::Q(18);
const C64Key C64Key::runStop(33);

const C64Key C64Key::restore(31);

C64Key::C64Key(isize nr)
{
    assert(nr >= 0 && nr <= 65);
    
    isize rowcol[66][2] = {

        // First physical row
        {7, 1}, {7, 0}, {7, 3}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0},
        {3, 3}, {4, 0}, {4, 3}, {5, 0}, {5, 3}, {6, 0}, {6, 3}, {0, 0},
        {0, 4},
        
        // Second physical row
        {7, 2}, {7, 6}, {1, 1}, {1, 6}, {2, 1}, {2, 6}, {3, 1}, {3, 6},
        {4, 1}, {4, 6}, {5, 1}, {5, 6}, {6, 1}, {6, 6}, {9, 9}, {0, 5},
        
        // Third physical row
        {7, 7}, {9, 9}, {1, 2}, {1, 5}, {2, 2}, {2, 5}, {3, 2}, {3, 5},
        {4, 2}, {4, 5}, {5, 2}, {5, 5}, {6, 2}, {6, 5}, {0, 1}, {0, 6},
        
        // Fourth physical row
        {7, 5}, {1, 7}, {1, 4}, {2, 7}, {2, 4}, {3, 7}, {3, 4}, {4, 7},
        {4, 4}, {5, 7}, {5, 4}, {6, 7}, {6, 4}, {0, 7}, {0, 2}, {0, 3},
        
        // Fifth physical row
        {7, 4}
    };
    
    this->nr = nr;
    
    if (nr != 31 /* RESTORE */ && nr != 34 /* SHIFT LOCK */) {
        
        this->row = rowcol[nr][0];
        this->col = rowcol[nr][1];
    }
}

C64Key::C64Key(isize row, isize col)
{
    assert(row >= 0 && row < 8);
    assert(col >= 0 && col < 8);
    
    isize nr[64] {
        15, 47, 63, 64, 16, 32, 48, 62,
        3, 19, 35, 4, 51, 36, 20, 50,
        5, 21, 37, 6, 53, 38, 22, 52,
        7, 23, 39, 8, 55, 40, 24, 54,
        9, 25, 41, 10, 57, 42, 26, 56,
        11, 27, 43, 12, 59, 44, 28, 58,
        13, 29, 45, 14, 61, 46, 30, 60,
        1, 0, 17, 2, 65, 49, 18, 33
    };
    
    this->row = row;
    this->col = col;
    this->nr = nr[8 * row + col];
}

std::vector<C64Key>
C64Key::translate(char c)
{
    switch (c) {
            
        case '1': return std::vector { C64Key::digit1 };
        case '!': return std::vector { C64Key::digit1, C64Key::leftShift };
        case '2': return std::vector { C64Key::digit2 };
        case '"': return std::vector { C64Key::digit2, C64Key::leftShift };
        case '3': return std::vector { C64Key::digit3 };
        case '#': return std::vector { C64Key::digit3, C64Key::leftShift };
        case '4': return std::vector { C64Key::digit4 };
        case '$': return std::vector { C64Key::digit4, C64Key::leftShift };
        case '5': return std::vector { C64Key::digit5 };
        case '%': return std::vector { C64Key::digit5, C64Key::leftShift };
        case '6': return std::vector { C64Key::digit6 };
        case '&': return std::vector { C64Key::digit6, C64Key::leftShift };
        case '7': return std::vector { C64Key::digit7 };
        case '\'': return std::vector { C64Key::digit7, C64Key::leftShift };
        case '8': return std::vector { C64Key::digit8 };
        case '(': return std::vector { C64Key::digit8, C64Key::leftShift };
        case '9': return std::vector { C64Key::digit9 };
        case ')': return std::vector { C64Key::digit9, C64Key::leftShift };
        case '0': return std::vector { C64Key::digit0 };
        case '+': return std::vector { C64Key::plus };
        case '-': return std::vector { C64Key::minus };

        case 'q': return std::vector { C64Key::Q };
        case 'Q': return std::vector { C64Key::Q, C64Key::leftShift };
        case 'w': return std::vector { C64Key::W };
        case 'W': return std::vector { C64Key::W, C64Key::leftShift };
        case 'e': return std::vector { C64Key::E };
        case 'E': return std::vector { C64Key::E, C64Key::leftShift };
        case 'r': return std::vector { C64Key::R };
        case 'R': return std::vector { C64Key::R, C64Key::leftShift };
        case 't': return std::vector { C64Key::T };
        case 'T': return std::vector { C64Key::T, C64Key::leftShift };
        case 'y': return std::vector { C64Key::Y };
        case 'Y': return std::vector { C64Key::Y, C64Key::leftShift };
        case 'u': return std::vector { C64Key::U };
        case 'U': return std::vector { C64Key::U, C64Key::leftShift };
        case 'i': return std::vector { C64Key::I };
        case 'I': return std::vector { C64Key::I, C64Key::leftShift };
        case 'o': return std::vector { C64Key::O };
        case 'O': return std::vector { C64Key::O, C64Key::leftShift };
        case 'p': return std::vector { C64Key::P };
        case 'P': return std::vector { C64Key::P, C64Key::leftShift };
        case '@': return std::vector { C64Key::at };
        case '*': return std::vector { C64Key::asterisk };

        case 'a': return std::vector { C64Key::A };
        case 'A': return std::vector { C64Key::A, C64Key::leftShift };
        case 's': return std::vector { C64Key::S };
        case 'S': return std::vector { C64Key::S, C64Key::leftShift };
        case 'd': return std::vector { C64Key::D };
        case 'D': return std::vector { C64Key::D, C64Key::leftShift };
        case 'f': return std::vector { C64Key::F };
        case 'F': return std::vector { C64Key::F, C64Key::leftShift };
        case 'g': return std::vector { C64Key::G };
        case 'G': return std::vector { C64Key::G, C64Key::leftShift };
        case 'h': return std::vector { C64Key::H };
        case 'H': return std::vector { C64Key::H, C64Key::leftShift };
        case 'j': return std::vector { C64Key::J };
        case 'J': return std::vector { C64Key::J, C64Key::leftShift };
        case 'k': return std::vector { C64Key::K };
        case 'K': return std::vector { C64Key::K, C64Key::leftShift };
        case 'l': return std::vector { C64Key::L };
        case 'L': return std::vector { C64Key::L, C64Key::leftShift };
        case ':': return std::vector { C64Key::colon };
        case '[': return std::vector { C64Key::colon, C64Key::leftShift };
        case ';': return std::vector { C64Key::semicolon };
        case ']': return std::vector { C64Key::semicolon, C64Key::leftShift };
        case '=': return std::vector { C64Key::equal };
        case '\n': return std::vector { C64Key::ret };

        case 'z': return std::vector { C64Key::Z };
        case 'Z': return std::vector { C64Key::Z, C64Key::leftShift };
        case 'x': return std::vector { C64Key::X };
        case 'X': return std::vector { C64Key::X, C64Key::leftShift };
        case 'c': return std::vector { C64Key::C };
        case 'C': return std::vector { C64Key::C, C64Key::leftShift };
        case 'v': return std::vector { C64Key::V };
        case 'V': return std::vector { C64Key::V, C64Key::leftShift };
        case 'b': return std::vector { C64Key::B };
        case 'B': return std::vector { C64Key::B, C64Key::leftShift };
        case 'n': return std::vector { C64Key::N };
        case 'N': return std::vector { C64Key::N, C64Key::leftShift };
        case 'm': return std::vector { C64Key::M };
        case 'M': return std::vector { C64Key::M, C64Key::leftShift };
        case ',': return std::vector { C64Key::comma };
        case '<': return std::vector { C64Key::comma, C64Key::leftShift };
        case '.': return std::vector { C64Key::period };
        case '>': return std::vector { C64Key::period, C64Key::leftShift };
        case '/': return std::vector { C64Key::slash };
        case '?': return std::vector { C64Key::slash, C64Key::leftShift };

        case ' ': return std::vector { C64Key::space };

        default: return std::vector<C64Key> { };
    }
}

}
