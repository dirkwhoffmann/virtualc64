/*!
 * @header      Datasette.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 Dirk W. Hoffmann
 * @brief       Declares Datasette class
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
class C64;

/*!
 * @brief   Virtual tape recorder (datasette)
 */
class Datasette : public VirtualComponent {
    
public:
    
    //! Constructor
    Datasette();
    
    //! Destructor
    ~Datasette();
    
    //! Reset VC1541 drive
    void reset();
    
    //! Dump current configuration into message queue
    void ping();

    //! Size of internal state
    uint32_t stateSize();
    
    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Save state
    void saveToBuffer(uint8_t **buffer);

    //! Dump current state into logfile
    void dumpState();

    // ---------------------------------------------------------------------------------------------
    //                                     Atrributes
    // ---------------------------------------------------------------------------------------------
    
private:
    
    //! @brief      Size of the attached data buffer
    /*! @discussion Equals 0 iff no tape is inserted. */
    uint32_t size;
    
    //! @brief      Data buffer (contains the raw data of the TAP archive)
    /*! @todo       Idea: Convert tape archive data into type (0?) format. */
    uint8_t *data;
    
    //! @brief      Data format (TAP type)
    /*! @discussion In TAP format 0, data byte 0 signals a long puls without stating its length precisely.
     *              In TAP format 1, each 0 is followed by three bytes stating the precise length in
     *              LO_LO_HI_00 format. */
    uint8_t type;
    
    /*! @brief      Read/Write head
     *  @discussion Value must be between 0 and size. head == size indicates EOT (end of tape) */
    uint32_t head;

    /*! @brief      Read/Write head
     *  @discussion Head position, measured in cycles */
    uint64_t headInCycles;

    /*! @brief      Read/Write head
     *  @discussion Head position, measured in seconds */
    uint32_t headInSeconds;

    //! @brief      Tape length in cycles
    /*! @discussion The value is computed in insertTape by examining all pulses in the data buffer */
    uint64_t durationInCycles;

    /*! @brief      Next scheduled rising edge on data line */
    int64_t nextRisingEdge;

    /*! @brief      Next scheduled falling edge on data line */
    int64_t nextFallingEdge;
    
    /*! @brief      Indicates whether the play key is pressed */
    bool playKey;
    
    /*! @brief      Indicates whether the motor is on */
    bool motor;
    
    // ---------------------------------------------------------------------------------------------
    //                                    Methods
    // ---------------------------------------------------------------------------------------------

public:
    
    //
    //! @functiongroup Handling virtual tapes
    //
    
    /*! @brief Returns true if a tape is inserted */
    inline bool hasTape() { return size != 0; }
    
    /*! @brief      Inserts a TAP archive as a virtual tape */
    void insertTape(TAPArchive *a);

    /*! @brief      Ejects the virtual tape
     *  @discussion Does nothing, if no tape is present.  */
    void ejectTape();

    /*! @brief      Put head at the beginning of the tape */
    void rewind() { head = headInSeconds = headInCycles = 0; }


    //
    //! @functiongroup Handling the read/write head
    //

    /*! @brief      Advances the read/write head for one pulse 
     *  @discussion This methods updates head, headInCycles, and headInSeconds */
    void advanceHead(bool silent = false);
    
    /*! @brief      Get current head position in different units */
    uint32_t getHead() { return head; }
    uint32_t getHeadInCycles() { return headInCycles; }
    uint32_t getHeadInSeconds() { return headInSeconds; }
    
    /*! @brief      Set current head position in cycles */
    void setHeadInCycles(uint64_t value);
    
    /*! @brief      Returns the head position as a percentage value
     *  @deprecated */
    // int progress() { return size ? (int)(100.0 * head / size) : 0; }

    /*! @brief      Returns the tape length in cycles */
    uint64_t getDurationInCycles() { return durationInCycles; }

    /*! @brief      Returns the tape length in seconds */
    uint32_t getDurationInSeconds() { return durationInCycles / PAL_CYCLES_PER_SECOND; }

    /*! @brief      Pulse length at current head position */
    int pulseLength(int *skip);
    int pulseLength() { return pulseLength(NULL); }
    //
    //! @functiongroup Running the device
    //
    
    /*! @brief      Press play on tape */
    void pressPlay(); 

    /*! @brief      Press stop key */
    void pressStop();

    /*! @brief      Returns true if the play key is pressed */
    bool getPlayKey() { return playKey; }

    /*! @brief      Switches motor on or off */
    void setMotor(bool value);

    /*! @brief    Executes the virtual datasette
     */
    void _execute();
    void _executeFirst();
    void _executeFalling();
    void _executeRising();
    void _executeLast();
    
    inline void execute() { if (playKey && motor) _execute(); }
};

#endif


