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
    
    /* Indicates if the emulator should be executed in warp mode.
     * To speed up emulation (e.g., during disk accesses), the virtual hardware
     * can be put into warp mode. In this mode, the emulation thread is no
     * longer paused to match the target frequency and runs as fast as possible.
     */
    bool warpMode = false;
    
    /* Indicates if the emulator should be executed in debug mode.
     * Debug mode is enabled when the GUI debugger is opend and disabled when
     * the GUI debugger is closed. In debug mode, several time-consuming tasks
     * are performed that are usually left out. E.g., the CPU checks for
     * breakpoints and records the executed instruction in it's trace buffer.
     */
    bool debugMode = false;
    
    
    //
    // Constructing and destroying
    //
    
public:
    
    virtual ~HardwareComponent();
    
    
    //
    // Acccessing properties
    //
    
    bool inWarpMode() { return warpMode; }
    bool inDebugMode() { return debugMode; }
    
    
    //
    // Managing the component
    //
    
public:
    
    /* Initializes the component and its subcomponent.
     * This function is called exactly once, in the constructor of the Amiga
     * class. Subcomponents can implement the delegation method _initialize()
     * to finalize their initialization, e.g., by setting up referecens that
     * did not exist when they were constructed.
     */
    void initialize();
    virtual void _initialize() { };
    
    /* There are several functions for querying and changing state:
     *
     *          -----------------------------------------------
     *         |                     run()                     |
     *         |                                               V
     *     ---------   powerOn()   ---------     run()     ---------
     *    |   Off   |------------>| Paused  |------------>| Running |
     *    |         |<------------|         |<------------|         |
     *     ---------   powerOff()  ---------    pause()    ---------
     *         ^                                               |
     *         |                   powerOff()                  |
     *          -----------------------------------------------
     *
     *     isPoweredOff()         isPaused()          isRunning()
     * |-------------------||-------------------||-------------------|
     *                      |----------------------------------------|
     *                                     isPoweredOn()
     */
    
    bool isPoweredOff() { return state == STATE_OFF; }
    bool isPoweredOn() { return state != STATE_OFF; }
    bool isPaused() { return state == STATE_PAUSED; }
    bool isRunning() { return state == STATE_RUNNING; }
    
protected:
    
    /* powerOn() powers the component on.
     *
     * current   | next      | action
     * -------------------------------------------------------------------------
     * off       | paused    | _powerOn() on each subcomponent
     * paused    | paused    | none
     * running   | running   | none
     */
    void powerOn();
    virtual void _powerOn() { }
    
    /* powerOff() powers the component off.
     *
     * current   | next      | action
     * -------------------------------------------------------------------------
     * off       | off       | none
     * paused    | off       | _powerOff() on each subcomponent
     * running   | off       | pause(), _powerOff() on each subcomponent
     */
    void powerOff();
    virtual void _powerOff() { }
    
    /* run() puts the component in 'running' state.
     *
     * current   | next      | action
     * -------------------------------------------------------------------------
     * off       | running   | powerOn(), _run() on each subcomponent
     * paused    | running   | _run() on each subcomponent
     * running   | running   | none
     */
    virtual void run();
    virtual void _run() { }
    
    /* pause() puts the component in 'paused' state.
     *
     * current   | next      | action
     * -------------------------------------------------------------------------
     * off       | off       | none
     * paused    | paused    | none
     * running   | paused    | _pause() on each subcomponent
     */
    virtual void pause();
    virtual void _pause() { };
    
public:
    
    /* Resets the component and its subcomponent.
     * Each component must implement this function.
     */
    virtual void reset();
    virtual void _reset() = 0;

    /* Asks the component to inform the GUI about its current state.
     * The GUI invokes this function when it needs to update all of its visual
     * elements. This happens, e.g., when a snapshot file was loaded.
     */
    virtual void ping();
    virtual void _ping() { }
    
    /* Collects information about the component and it's subcomponents.
     * Many components contains an info variable of a class specific type
     * (e.g., CPUInfo, MemoryInfo, ...). These variables contain the
     * information shown in the GUI's inspector window and are updated by
     * calling this function. The function is called automatically when the
     * emulator switches to pause state to keep the GUI inspector data up
     * to date.
     * Note: Because this function accesses the internal emulator state with
     * many non-atomic operations, it must not be called on a running emulator.
     * To query information while the emulator is running, set up an inspection
     * target via setInspectionTarget()
     */
    void inspect();
    virtual void _inspect() { }
    
    /* Base method for building the class specific getInfo() methods
     * If the emulator is running, the result of the most recent inspection is
     * returned. If the emulator is not running, the function first updates the
     * cached values in order to return up-to-date results.
     */
    template<class T> T getInfo(T &cachedValues) {
        
        if (!isRunning()) _inspect();
        
        T result;
        synchronized { result = cachedValues; }
        return result;
    }
    
    /* Dumps debug information about the current configuration to the console
     */
    virtual void dumpConfig();
    virtual void _dumpConfig() { }

    /* Dumps debug information about the internal state to the console
     */
    virtual void dump();
    virtual void _dump() { }


     // Switches warp mode on or off
    virtual void setWarp(bool enable);
    virtual void _setWarp(bool enable) { };
    void enableWarpMode() { setWarp(true); }
    void disableWarpMode() { setWarp(false); }
    
    // Switches debug mode on or off
    virtual void setDebug(bool enable);
    virtual void _setDebug(bool enable) { };
    void enableDebugMode() { setDebug(true); }
    void disableDebugMode() { setDebug(false); }

    /* Informs the component about a clock frequency change.
     * This delegation method is called on startup and whenever the CPU clock
     * frequency changes (i.e., when switching between PAL and NTSC). Some
     * components overwrite this function to update clock dependent lookup
     * tables.
     */
    virtual void setClockFrequency(u32 value);
    virtual void _setClockFrequency(u32 value) { }

    

    
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
