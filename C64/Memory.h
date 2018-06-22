/*!
 * @header      Memory.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
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

// Forward declarations
class CPU;

class Memory : public VirtualComponent {

    friend CPU;
    
public:
	
	//! @brief    Constructor
	Memory();
	
	//! @brief    Destructor
	~Memory();

    //
    //! @functiongroup Reading from memory
    //

private:
    
    /*! @brief    Peeks a byte from memory.
     *  @details  This function emulates a native read access including side effects.
     *            The value is read from the specified peek source.
     *  @seealso  snoop()
     */
    virtual uint8_t peek(uint16_t addr, MemoryType source) = 0;
    
    /*! @brief    Peeks a byte from memory.
     *  @details  This function emulates a native read access including side effects.
     *            The value is read is from the currently visible memory.
     *  @seealso  snoop()
     */
	virtual uint8_t peek(uint16_t addr) = 0;

public:
    
    /*! @brief    Peeks a byte from memory without causing side effects.
     */
    virtual uint8_t snoop(uint16_t addr, MemoryType source) = 0;
    
    /*! @brief    Peeks a byte from memory without causing side effects.
     */
    virtual uint8_t snoop(uint16_t addr) = 0;
    

    //
    //! @functiongroup Writing into memory
    //
    
    /*! @brief    Pokes a byte into memory.
     *  @details  The value is written into the specified memory.
     */
    virtual void poke(uint16_t addr, uint8_t value, MemoryType target) = 0;
    
    /*! @brief    Pokes a byte into memory.
     *  @details  This function emulates a native write access including side effects.
     */
    virtual void poke(uint16_t addr, uint8_t value) = 0;
};

#endif
