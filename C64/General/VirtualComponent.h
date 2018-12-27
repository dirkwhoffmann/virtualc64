/*!
 * @header      VirtualComponent.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
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

#ifndef _VIRTUAL_COMPONENT_INC
#define _VIRTUAL_COMPONENT_INC

#include "VC64Object.h"

// Forward declarations
class C64;


/*! @brief    Base class for all virtual hardware components
 *  @details  This class defines the base functionality of all virtual
 *            components. The class comprises functions for resetting,
 *            suspending and resuming the component, as well as functions for
 *            loading and saving snapshots.
 */
class VirtualComponent : public VC64Object {

protected:
    
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
        uint8_t flags;
        
    } SnapshotItem;
    
public: 

    /*! @brief    Reference to the virtual C64 top-level object.
     *  @details  This reference is setup for all hardware components in the
     *            constructor of the C64 class.
     */
    C64 *c64 = NULL;
    
protected:
    
    //! @brief    Sub components of this component
    VirtualComponent **subComponents = NULL;
    
    //! @brief    List of snapshot items of this component
    SnapshotItem *snapshotItems = NULL;
    
    //! @brief    Snapshot size on disk (in bytes)
    unsigned snapshotSize = 0;
    
public:

	//! @brief    Destructor
	virtual ~VirtualComponent();


    //
    //! @functiongroup Initializing the component
    //
		
    /*! @brief    Assign top-level C64 object.
     *  @details  The provided reference is propagated automatically to all
     *            sub components.
     */
    virtual void setC64(C64 *c64);

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
    virtual void setClockFrequency(uint32_t frequency);
    
    
    //
    //! @functiongroup Controlling the emulation thread
    //
    
    /*! @brief    Freezes the emulation thread.
     *  @details  If the internal state of the emulator is changed from outside
     *            the emulation thread, the change must be embedded in a
     *            suspend / resume block as follows:
     *
     *            suspend();
     *            do something with the internal state;
     *            resume();
     *
     *  @note     Multiple suspend / resume blocks can be nested.
     *  @see      resume
     */
    virtual void suspend();
    
    /*! @brief    Continues the emulation thread.
     *  @details  This functions concludes a suspend operation.
     *  @see      suspend
     */
    virtual void resume();
    
    
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
    
    /*! @brief    Registers all sub components for this component
     *  @abstract Sub components are usually registered in the constructor of
     *            a virtual component.
     *  @param    items Pointer to the first element of a VirtualComponet* array.
     *            The end of the array is marked by a NULL pointer.
     *  @param    legth Size of the subComponent array in bytes.
     */
    void registerSubComponents(VirtualComponent **subComponents, unsigned length);
    
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
    void loadFromBuffer(uint8_t **buffer);
    
    /*! @brief    Delegation methods called inside loadFromBuffer()
     *  @details  Some components overwrite this method to add custom behavior
     *            such as loading items that cannot be handled by the default
     *            implementation.
     */
    virtual void  willLoadFromBuffer(uint8_t **buffer) { };
    virtual void  didLoadFromBuffer(uint8_t **buffer) { };

    /*! @brief    Save internal state to memory buffer
     *  @note     Snapshot items of size 2, 4, or 8 are converted to big endian
     *            format automatically. Otherwise, a byte array is assumed.
     *  @param    buffer Pointer to next byte to read
     *  @seealso  WORD_ARRAY, DWORD_ARRAY, QWORD_ARRAY
     */
    void saveToBuffer(uint8_t **buffer);
    
    /*! @brief    Delegation methods called inside saveToBuffer()
     *  @details  Some components overwrite this method to add custom behavior
     *            such as saving items that cannot be handled by the default
     *            implementation.
     */
    virtual void  willSaveToBuffer(uint8_t **buffer) { };
    virtual void  didSaveToBuffer(uint8_t **buffer) { };
};

#endif
