// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PROCESSOR_PORT_H
#define _PROCESSOR_PORT_H

#include "C64Component.h"

/*! @brief    Processor port
 *  @details  The C64 CPU contains a processor port register and a data
 *            direction register that indicates if a processor bit is configured
 *            as input or output. The register serves multiple pursposes.
 *            Firstly, it is used for bank switching, i.e. it decided for
 *            certain memory regions if ROM or RAM is avaible. Secondly, it is
 *            used to communicate with the datasette.
 */
class ProcessorPort : public C64Component {
   
    //! @brief    Processor port bits
    u8 port;
    
    //! @brief    Processor port direction bits
    u8 direction;

    //! @brief    Clock cycle when floating bit values reach zero
    /*! @details  Bit 3, 6, and 7 of the processor need our special attention.
     *            When the direction of these bits is changed from output to
     *            input, there will be no external signal driving them. As a
     *            result, the bits will be in a floating state and act as an
     *            capacitor. They will discharge slowly and eventually reach
     *            zero. These variables are used to indicate when the zero level
     *            is reached. All three variables are queried in readPort() and
     *            have the following semantics:
     *            dischargeCycleBit > current cycle => bit reads as 1
     *                                                 (if configured as input)
     *            otherwise                         => bit reads as 0
     *                                                 (if configured as input)
     */
    u64 dischargeCycleBit3;
    u64 dischargeCycleBit6;
    u64 dischargeCycleBit7;

    
    //
    // Constructing and serializing
    //
    
public:
    
    ProcessorPort(C64 &ref);
    
    
    //
    // Methods from HardwareComponent
    //

private:

    void _reset() override; 
    void _dump() override;

    
    //
    //! @functiongroup Accessing the port registers
    //
    
public:
    
    //! @brief    Reads from the processor port register.
    u8 read();

    //! @brief    Reads from the processor port direction register.
    u8 readDirection();

    //! @brief    Writes to the processor port register.
    void write(u8 value);
    
    //! @brief    Writes to the processor port direction register.
    void writeDirection(u8 value);
};

#endif 
