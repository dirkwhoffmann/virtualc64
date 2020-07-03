/*!
 * @header      Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
