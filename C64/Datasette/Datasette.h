/*!
 * @header      Datasette.h
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

#ifndef _DATASETTE_INC
#define _DATASETTE_INC

class TAPFile;

//! @brief    A Commodore 1530 (C2N) tape recorder (Datasette)
class Datasette : public VirtualComponent {
    
    //
    // Tape
    //
    
    //! @brief    Data buffer (contains the raw data of the TAP archive)
    uint8_t *data = NULL;
    
    //! @brief    Size of the attached data buffer
    uint64_t size = 0;
    
    /*! @brief    Data format (TAP type)
     *  @details  In TAP format 0, data byte 0 signals a long pulse without
     *            stating its length precisely.
     *            In TAP format 1, each 0 is followed by three bytes stating
     *            the precise length in LO_LO_HI_00 format.
     */
    uint8_t type = 0;
    
    /*! @brief    Tape length in cycles
     *  @details  The value is set when insertTape() is called. It is computed
     *            by iterating over all pulses in the data buffer.
     */
    uint64_t durationInCycles = 0;
    
    
    //
    // Datasette
    //
    
    /*! @brief    Read/Write head
     *  @details  Value must be between 0 and size.
     *  @note     head == size indicates EOT (End Of Tape)
     */
    uint64_t head = 0;
    
    /*! @brief    Read/Write head
     *  @details  Head position, measured in cycles
     */
    uint64_t headInCycles = 0;
    
    /*! @brief    Read/Write head
     *  @details  Head position, measured in seconds
     */
    uint32_t headInSeconds = 0;
    
    /*! @brief    Next scheduled rising edge on data line
     */
    int64_t nextRisingEdge = 0;
    
    /*! @brief    Next scheduled falling edge on data line
     */
    int64_t nextFallingEdge = 0;
    
    /*! @brief    Indicates whether the play key is pressed
     */
    bool playKey = false;
    
    /*! @brief    Indicates whether the motor is on
     */
    bool motor = false;
    
    
public:
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Constructor
    Datasette();
    
    //! @brief    Destructor
    ~Datasette();
    
    //
    //! @functiongroup Methods from VirtualComponent
    //
    
    void reset();
    void ping();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);


    //
    //! @functiongroup Handling virtual tapes
    //
    
    //! @brief    Returns true if a tape is inserted.
    bool hasTape() { return size != 0; }
    
    //! @brief    Inserts a TAP archive as a virtual tape.
    bool insertTape(TAPFile *a);

    /*! @brief    Ejects the virtual tape.
     *  @details  Does nothing, if no tape is present.  
     */
    void ejectTape();

    //! @brief    Returns the tape type (TAP format, 0 or 1).
    uint8_t getType() { return type; }

    //! @brief    Returns the tape length in cycles.
    uint64_t getDurationInCycles() { return durationInCycles; }
    
    //! @brief    Returns the tape length in seconds.
    uint32_t getDurationInSeconds() { return (uint32_t)(durationInCycles / (uint64_t)PAL_CLOCK_FREQUENCY); }

    
    //
    //! @functiongroup Operating the read/write head
    //

    //! @brief    Puts the read/write head at the beginning of the tape.
    void rewind() { head = headInSeconds = headInCycles = 0; }

    /*! @brief    Advances the read/write head one pulse.
     *  @details  This methods updates head, headInCycles, and headInSeconds.
     *  @param    silent indicates if a MSG_VC1530_PROGRESS should be sent.
     */
    void advanceHead(bool silent = false);
    
    //! @brief    Returns the head position
    uint64_t getHead() { return head; }

    //! @brief    Returns the head position in CPU cycles
    uint64_t getHeadInCycles() { return headInCycles; }

    //! @brief    Returns the head position in seconds
    uint32_t getHeadInSeconds() { return headInSeconds; }
    
    //! @brief    Sets the current head position in cycles.
    void setHeadInCycles(uint64_t value);
    
    //! @brief    Returns the pulse length at the current head position
    int pulseLength(int *skip);
    int pulseLength() { return pulseLength(NULL); }

    
    //
    //! @functiongroup Running the device
    //
    
    /*! @brief    Returns true if the play key is pressed 
     */
    bool getPlayKey() { return playKey; }

    /*! @brief    Press play on tape 
     */
    void pressPlay(); 

    /*! @brief    Press stop key 
     */
    void pressStop();

    /*! @brief    Returns true if the datasette motor is switched on 
     */
    bool getMotor() { return motor; }

    /*! @brief    Switches motor on or off 
     */
    void setMotor(bool value);

    /*! @brief  Executes the virtual datasette
     */
    void execute() { if (playKey && motor) _execute(); }

private:

    //! @brief    Internal execution function
    void _execute();

    //! @brief    Simulates the falling edge of a pulse
    void _executeFalling();

    //! @brief    Simulates the rising edge of a pulse
    void _executeRising();

};

#endif


