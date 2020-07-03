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
#include "VirtualComponent.h"
#include "Memory_types.h"


//! @brief    Common interface for C64 memory and VC1541 memory
class Memory : public VirtualComponent {

    friend class CPU;
    
protected:
    
    /*! @brief    Pointer to the first byte of the stack memory
     *  @details  This value is used by peekStack and pokeStack, only.
     */
    uint8_t *stack = NULL;
    
private:
    
    /*! @brief    Peeks a byte from memory.
     *  @details  Emulates a native read access including side effects.
     *            The value is read is from the currently visible memory.
     *  @seealso  spypeek
     */
	virtual uint8_t peek(uint16_t addr) = 0;

    //! @brief    Peeks a byte from the zero page.
    virtual uint8_t peekZP(uint8_t addr) = 0;

    //! @brief    Peeks a byte from the stack.
    virtual uint8_t peekStack(uint8_t sp) { return stack[sp]; }
    
public:
    
    /*! @brief    Peeks a byte from memory without causing side effects.
     *  seealso   peek
     */
    virtual uint8_t spypeek(uint16_t addr) = 0;
    
    /*! @brief    Pokes a byte into memory.
     *  @details  Emulates a native write access including all side effects.
     */
    virtual void poke(uint16_t addr, uint8_t value) = 0;

    //! @brief    Pokes a byte into the zero page.
    virtual void pokeZP(uint8_t addr, uint8_t value) = 0;

    //! @brief    Pokes a byte onto the stack.
    virtual void pokeStack(uint8_t sp, uint8_t value) { stack[sp] = value; }
};

#endif
