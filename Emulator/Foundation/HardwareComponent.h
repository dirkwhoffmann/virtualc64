// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _HARDWARE_COMPONENT_H
#define _HARDWARE_COMPONENT_H

#include "C64Object.h"


// Base class for all hardware components. This class defines the base
// functionality of all hardware components. It comprises functions for
// powering up, powering down, resetting, and serializing.

class HardwareComponent : public C64Object {
    
public:

    // The sub components of this component
    vector<HardwareComponent *> subComponents;
    
protected:
    
    /* State model
     * The virtual hardware components can be in three different states
     * called 'Off', 'Paused', and 'Running'.
     *
     *        Off: The C64 is turned off
     *     Paused: The C64 is turned on, but there is no emulator thread
     *    Running: The C64 is turned on and the emulator thread running
     */
    EmulatorState state = STATE_OFF;
    
    /* Warp mode
     * To speed up emulation (e.g., during disk accesses), the virtual hardware
     * can be put into warp mode. In this mode, the emulation thread is no
     * longer paused to match the target frequency and runs as fast as possible.
     */
    bool warp = false;
    
    
    //
    // Constructing and destroying
    //
    
public:
    
    virtual ~HardwareComponent();
    
    
    //
    // Initializing
    //
    
public:
    
    /* Initializes the component and it's sub-component.
     * This function is called exactly once, in the constructor of the Amiga
     * class. Sub-components can implement the delegation method _initialize()
     * to finalize their initialization, e.g., by setting up referecens that
     * did not exist when they were constructed.
     */
    void initialize();
    virtual void _initialize() { };
    
    
    /*! @brief   Type and behavior of a snapshot item
     *  @details The reset flags indicate whether the snapshot item should be
     *           set to 0 automatically during a reset. The format flags are
     *           important when big chunks of data are specified. They are
     *           needed loadBuffer and saveBuffer to correctly converting little
     *           endian to big endian format.
     */
    enum {
        KEEP_ON_RESET  = 0x00, //! Don't touch item during a reset
        CLEAR_ON_RESET = 0x10, //! Reset to zero during a reset
        
        BYTE_ARRAY     = 0x01, //! Data chunk is an array of bytes
        WORD_ARRAY     = 0x02, //! Data chunk is an array of words
        DWORD_ARRAY    = 0x04, //! Data chunk is an array of double words
        QWORD_ARRAY    = 0x08  //! Data chunk is an array of quad words
    };
    
    /*! @brief Fingerprint of a snapshot item
     */
    typedef struct {
        
        void *data;
        size_t size;
        u8 flags;
        
    } SnapshotItem;
            
protected:
        
    //! @brief    List of snapshot items of this component
    SnapshotItem *snapshotItems = NULL;
    
    //! @brief    Snapshot size on disk (in bytes)
    unsigned snapshotSize = 0;
    
public:


    //
    //! @functiongroup Initializing the component
    //
		
    /*! @brief    Reset component to its initial state.
     *  @details  By default, each component also resets its sub components.
     */
	virtual void reset();
	
    //! @brief    Asks the component to inform the GUI about its current state.
    /*! @details  The GUI invokes this function to update its visual elements,
     *            e.g., after loading a snapshot file. Only some components
     *            overwrite this function.
     */
    virtual void ping();

    //! @brief    Informs the component about a clock frequency change.
    /*! @details  This delegation method is called on startup and whenever the
     *            CPU clock frequency changes (i.e., when switching between
     *            PAL and NTSC). Some components overwrite this function to
     *            update clock dependent lookup tables.
     *  @param    frequency Frequency of the C64 CPU in Hz.
     *            Must be either PAL_CLOCK_FREQUENCY_PAL or NTSC_CLOCK_FREQUENCY.
     */
    virtual void setClockFrequency(u32 frequency);
    
    //
    //! @functiongroup Debugging the component
    //

	//! @brief    Print info about the internal state.
	/*! @details  This functions is intended for debugging purposes only. Any
     *            derived component should override this method and print out
     *            useful debugging information.
	 */ 
    virtual void dump() { };
	
    
    //
    //! @functiongroup Registering snapshot items and sub components
    //
        
    /*! @brief    Registers all snapshot items for this component
     *  @abstract Snaphshot items are usually registered in the constructor of
     *            a virtual component.
     *  @param    items Pointer to the first element of a SnapshotItem* array.
     *            The end of the array is marked by a NULL pointer.
     *  @param    legth Size of the SnapshotItem array in bytes.
     */
    void registerSnapshotItems(SnapshotItem *items, unsigned length);
    

public:
    
    //
    //! @functiongroup Loading and saving snapshots
    //

    //! @brief    Returns the size of the internal state in bytes
    virtual size_t stateSize();

    /*! @brief    Load internal state from memory buffer
     *  @note     Snapshot items of size 2, 4, or 8 are converted to big endian
     *            format automatically. Otherwise, a byte array is assumed.
     *  @param    buffer Pointer to next byte to read
     *  @seealso  WORD_ARRAY, DWORD_ARRAY, QWORD_ARRAY
     */
    void loadFromBuffer(u8 **buffer);
    
    /*! @brief    Delegation methods called inside loadFromBuffer()
     *  @details  Some components overwrite this method to add custom behavior
     *            such as loading items that cannot be handled by the default
     *            implementation.
     */
    virtual void  willLoadFromBuffer(u8 **buffer) { };
    virtual void  didLoadFromBuffer(u8 **buffer) { };

    /*! @brief    Save internal state to memory buffer
     *  @note     Snapshot items of size 2, 4, or 8 are converted to big endian
     *            format automatically. Otherwise, a byte array is assumed.
     *  @param    buffer Pointer to next byte to read
     *  @seealso  WORD_ARRAY, DWORD_ARRAY, QWORD_ARRAY
     */
    void saveToBuffer(u8 **buffer);
    
    /*! @brief    Delegation methods called inside saveToBuffer()
     *  @details  Some components overwrite this method to add custom behavior
     *            such as saving items that cannot be handled by the default
     *            implementation.
     */
    virtual void  willSaveToBuffer(u8 **buffer) { };
    virtual void  didSaveToBuffer(u8 **buffer) { };

    
    //
    // Standard implementations for _reset, _load, and _save
    //

    #define COMPUTE_SNAPSHOT_SIZE \
    SerCounter counter; \
    applyToPersistentItems(counter); \
    applyToResetItems(counter); \
    return counter.count;

    #define RESET_SNAPSHOT_ITEMS \
    SerResetter resetter; \
    applyToResetItems(resetter); \
    debug(SNP_DEBUG, "Resetted\n");

    #define LOAD_SNAPSHOT_ITEMS \
    SerReader reader(buffer); \
    applyToPersistentItems(reader); \
    applyToResetItems(reader); \
    debug(SNP_DEBUG, "Recreated from %d bytes\n", reader.ptr - buffer); \
    return reader.ptr - buffer;

    #define SAVE_SNAPSHOT_ITEMS \
    SerWriter writer(buffer); \
    applyToPersistentItems(writer); \
    applyToResetItems(writer); \
    debug(SNP_DEBUG, "Serialized to %d bytes\n", writer.ptr - buffer); \
    return writer.ptr - buffer;
};

#endif
