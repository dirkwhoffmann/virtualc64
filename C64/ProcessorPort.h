/*!
 * @header      ProcessorPort.h
 * @author      Written by Dirk Hoffmann
 * @copyright   All rights reserved.
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

#ifndef _PROCESSORPORT_H
#define _PROCESSORPORT_H

#include "VirtualComponent.h"

/*! @brief    Processor port
 *  @details  The C64 contains a processor port register and a data direction register that
 *            indicates if a processor bit is configured as input or output. The register
 *            serves multiple pursposes. Firstly, it is used for bank switching, i.e. it
 *            decided for certain memory regions if ROM or RAM is avaible. Secondly, it
 *            it used to communicate with the datasette.
 */
class ProcessorPort : public VirtualComponent {
   
    //! @brief    Processor port bits
    uint8_t port;
    
    //! @brief    Processor port direction bits
    uint8_t direction;

    //! @brief    Clock cycle when floating bit values reach zero
    /*! @details  Bit 3, 6, and 7 of the processor need our special attention.
     *            When these bits are changed from being outputs to being inputs, there is no
     *            external signal driving them. They get into a floating state and act as an
     *            capacitor. As a result, they will discharge slowly and eventually reach zero.
     *            These variables are used to indicate when the zero level is reached.
     *            All three variables are queried in readPort() and have the following semantics:
     *            dischargeCycleBit3 > current cycle => bit reads as 1 (if configured as input)
     *            otherwise                          => bit reads as 0 (if configured as input)
     */
    uint64_t dischargeCycleBit3;
    uint64_t dischargeCycleBit6;
    uint64_t dischargeCycleBit7;

public:
    //! @brief    Constructor
    ProcessorPort();
    
    //! @brief    Destructor
    ~ProcessorPort();
    
    //! @brief    Restores the initial state.
    void reset();
    
    //! @brief    Prints debug information.
    void dumpState();

    //! @brief    Reads from the processor port register
    uint8_t read();

    //! @brief    Reads from the processor port direction register
    uint8_t readDirection();

    //! @brief    Writes to the processor port register
    void write(uint8_t value);
    
    //! @brief    Writes to the processor port direction register
    void writeDirection(uint8_t value);
};

#endif 
