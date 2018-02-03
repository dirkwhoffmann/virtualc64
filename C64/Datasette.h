/*!
 * @header      Datasette.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 - 2016 Dirk W. Hoffmann
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

// Forward declarations
class TAPContainer;

/*! 
 *  @brief    Virtual tape recorder (datasette)
 */
class Datasette : public VirtualComponent {
    
public:
    
    //! @brief    Constructor
    Datasette();
    
    //! @brief    Destructor
    ~Datasette();
    
    //! @brief    Resets the VC1541 drive
    void reset();
    
    //! @brief    Dumps current configuration into message queue
    void ping();

    //! @brief    Return the size of the internal state
    size_t stateSize();
    
    //! @brief    Restores the current state from a buffer
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Saves the current state into a buffer
    void saveToBuffer(uint8_t **buffer);

    //! @brief    Dumps the current state
    void dumpState();

    
    // ---------------------------------------------------------------------------------------------
    //                                     Atrributes
    // ---------------------------------------------------------------------------------------------
    
private:
    
    //
    //! @functiongroup Tape
    //

    /*! @brief    Data buffer (contains the raw data of the TAP archive)
     *  @details  Pointer is NULL if no data is present
     */
    uint8_t *data;

    /*! @brief    Size of the attached data buffer
     *  @details  Equals 0 iff no tape is inserted.
     */
    uint64_t size;
    
    /*! @brief    Data format (TAP type)
     *  @details  In TAP format 0, data byte 0 signals a long puls without stating its length precisely.
     *            In TAP format 1, each 0 is followed by three bytes stating the precise length in
     *            LO_LO_HI_00 format. 
     */
    uint8_t type;
    
    /*! @brief    Tape length in cycles
     *  @details  The value is computed in insertTape by examining all pulses in the data buffer
     */
    uint64_t durationInCycles;

    //
    //! @functiongroup Datasette
    //

    /*! @brief    Read/Write head
     *  @details  Value must be between 0 and size. head == size indicates EOT (end of tape) 
     */
    uint64_t head;

    /*! @brief    Read/Write head
     *  @details  Head position, measured in cycles 
     */
    uint64_t headInCycles;

    /*! @brief    Read/Write head
     *  @details  Head position, measured in seconds
     */
    uint32_t headInSeconds;

    /*! @brief    Next scheduled rising edge on data line 
     */
    int64_t nextRisingEdge;

    /*! @brief    Next scheduled falling edge on data line 
     */
    int64_t nextFallingEdge;
    
    /*! @brief    Indicates whether the play key is pressed 
     */
    bool playKey;
    
    /*! @brief    Indicates whether the motor is on 
     */
    bool motor;
    
    // ---------------------------------------------------------------------------------------------
    //                                    Methods
    // ---------------------------------------------------------------------------------------------

public:
    
    //
    //! @functiongroup Handling virtual tapes
    //
    
    /*! @brief    Returns true if a tape is inserted 
     */
    bool hasTape() { return size != 0; }
    
    /*! @brief    Inserts a TAP archive as a virtual tape 
     */
    void insertTape(TAPContainer *a);

    /*! @brief    Ejects the virtual tape
     *  @details  Does nothing, if no tape is present.  
     */
    void ejectTape();

    /*! @brief    Returns type of tape (TAP format, 0 or 1). 
     */
    uint8_t getType() { return type; }

    /*! @brief    Returns the tape length in cycles 
     */
    uint64_t getDurationInCycles() { return durationInCycles; }
    
    /*! @brief    Returns the tape length in seconds 
     */
    uint32_t getDurationInSeconds() { return (uint32_t)(durationInCycles / (uint64_t)PAL_CYCLES_PER_SECOND); }

    //
    //! @functiongroup Handling the read/write head
    //

    /*! @brief    Puts the read/write head at the beginning of the tape
     */
    void rewind() { head = headInSeconds = headInCycles = 0; }

    /*! @brief    Advances the read/write head for one pulse
     *  @details  This methods updates head, headInCycles, and headInSeconds 
     */
    void advanceHead(bool silent = false);
    
    /*! @brief    Gets the current head position in different units
     */
    uint64_t getHead() { return head; }
    uint64_t getHeadInCycles() { return headInCycles; }
    uint32_t getHeadInSeconds() { return headInSeconds; }
    
    /*! @brief    Sets the current head position in cycles
     */
    void setHeadInCycles(uint64_t value);
    
    /*! @brief    Returns the pulse length at the current head position
     */
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


