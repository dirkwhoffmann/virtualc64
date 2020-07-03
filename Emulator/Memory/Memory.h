// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MEMORY_INC
#define _MEMORY_INC

#include "basic.h"
#include "HardwareComponent.h"
#include "MemoryTypes.h"


//! @brief    Common interface for C64 memory and VC1541 memory
class Memory : public HardwareComponent {

    friend class CPU;
    
protected:
    
    /*! @brief    Pointer to the first byte of the stack memory
     *  @details  This value is used by peekStack and pokeStack, only.
     */
    u8 *stack = NULL;
    
private:
    
    /*! @brief    Peeks a byte from memory.
     *  @details  Emulates a native read access including side effects.
     *            The value is read is from the currently visible memory.
     *  @seealso  spypeek
     */
	virtual u8 peek(u16 addr) = 0;

    //! @brief    Peeks a byte from the zero page.
    virtual u8 peekZP(u8 addr) = 0;

    //! @brief    Peeks a byte from the stack.
    virtual u8 peekStack(u8 sp) { return stack[sp]; }
    
public:
    
    /*! @brief    Peeks a byte from memory without causing side effects.
     *  seealso   peek
     */
    virtual u8 spypeek(u16 addr) = 0;
    
    /*! @brief    Pokes a byte into memory.
     *  @details  Emulates a native write access including all side effects.
     */
    virtual void poke(u16 addr, u8 value) = 0;

    //! @brief    Pokes a byte into the zero page.
    virtual void pokeZP(u8 addr, u8 value) = 0;

    //! @brief    Pokes a byte onto the stack.
    virtual void pokeStack(u8 sp, u8 value) { stack[sp] = value; }
};

#endif
