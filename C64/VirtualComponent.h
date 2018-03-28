/*!
 * @header      VirtualComponent.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2015 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _VIRTUAL_COMPONENT_INC
#define _VIRTUAL_COMPONENT_INC

#include "VC64Object.h"

// Forward declarations
class C64;

//! @brief    Time delayed uint8_t value
/*! @details  Use READ8_DELAYED and WRITE8_DELAYED to read and write a value.
 *            All written values show up one cycle after they were written.
 *  @note     Time delayed variables are speed optimized for read accesses.
 */
typedef struct {
    uint64_t timeStamp; // Cycle when the new value shows up
    uint8_t value;
    uint8_t prevValue;
} uint8_delayed;

/*! @brief    Common functionality of all virtual computer components.
 *  @details  This class defines the base functionality of all virtual components.
 *            The class comprises functions for resetting, suspending and resuming the component,
 *            as well as functions for loading and saving state (snapshots).
 */
class VirtualComponent : public VC64Object {

public: 

    /*! @brief    Reference to the virtual C64 top-level object.
     *  @details  This reference is setup in the reset method and provides easy access to all
     *            other components of the same virtual C64. */
    C64 *c64;
    
private:
    
	/*! @brief    Indicates whether the component is currently active.
     *  @details  All virtual components can be in two states. They can either be 'running' or 'halted'.
     *            During normal operation, all components are running. If an error occurrs, or if the
     *            user requests the virtual machine to halt in the debugger, the components will enter
     *            the 'halted' state.
     */
	bool running;
		
	/*! @brief    The original state before the first call of suspend(). 
     *  @see      suspend
     */
	// bool suspendedState;

	/*! @brief    Number of times the component is suspended.
     *  @details  The value is equal to the number of suspend calls minus the number of resume calls
     */
	int suspendCounter;
				
public:
    
	//! @brief    Constructor
	VirtualComponent();

	//! @brief    Destructor
	virtual ~VirtualComponent();

    
    //
    //! @functiongroup Initializing the component
    //
		
    /*! @brief    Assign top-level C64 object.
     *  @details  The provided reference is propagated automatically to all sub components.
     */
    void setC64(C64 *c64);

    
    /*! @brief    Reset component to its initial state.
     *  @details  By default, each component also resets all of its sub components.
     */
	virtual void reset();
	
    //! @brief    Trigger the component to send messages about its current state.
    /*! @details  The GUI invokes this function to update its visual elements, e.g., after loading
     *            a snapshot file. Only a few components overwrite this function. All others stay 
     *            silent on default.
     */
    virtual void ping();

    //
    //! @functiongroup Debugging the component
    //

	//! @brief    Print info about the internal state.
	/*! @details  This functions is intended for debugging purposes only. Any derived component should
	 *            override this method and print out some useful debugging information.
	 */ 
	virtual void dumpState();
	
    
    //
    //! @functiongroup Running the component
    //

	//! @brief    Start component.
	/*! @details  The function is called when the virtual computer is requested to run.
     *            Some components such as the CPU require asynchronously running threads and will start 
     *            them here. Most of the other components are of a static nature and won't implement 
     *            additional functionality.
     */
	virtual void run();

	/*! @brief    Returns true iff the component is running.
     */
	virtual bool isRunning();
	
	/*! @brief    Stops component.
     *  @details  The function is called when the virtual computer is requested to freeze. For example, the 
     *            CPU will ask its asynchronously running thread to halt. Most of the other components are 
     *            of a static nature and won't implement additional functionality.
     */
	virtual void halt();

	/*! @brief    Returns true iff the component is halted
     */
	virtual bool isHalted();

	/*! @brief    Suspends component.
     *  @details  The suspend mechanism is a nested run/halt mechanism. First of all, it works like halt,
     *            i.e., the component freezes. In contrast to halt, the suspend function remembers whether
     *            the component was already halted or running. When the resume function is invoked, the original
     *            running state is reestablished. In other words: If your component is currently running and you
     *            suspend it 10 times, you'll have to resume it 10 times to make it run again.
     *  @see      resume
     */
	void suspend();
	
	/*! @brief    Resumes component.
     *  @details  This functions concludes a suspend operation.
     *  @see      suspend
     */
	void resume();

    
    //
    //! @functiongroup Registering snapshot items and sub components
    //
    
protected:
    
    /*! @brief   Type and behavior of a snapshot item
     *  @details The reset flags indicate whether the snapshot item should be set to 0 automatically during 
     *           a reset. The format flags are important when big chunks of data are specified. They are needed
     *           loadBuffer and saveBuffer to correctly converting little endian format to big endian format.
     */
    enum {
        KEEP_ON_RESET      = 0x00, //! Don't touch item in VirtualComponent::reset()
        CLEAR_ON_RESET     = 0x10, //! Set item to 0 in VirtualComponent::reset()
        BYTE_FORMAT        = 0x01, //! Data chunk consists of 8 bit values.
        WORD_FORMAT        = 0x02, //! Data chunk consists of 16 bit values
        DOUBLE_WORD_FORMAT = 0x04, //! Data chunk consists of 32 bit values
        QUAD_WORD_FORMAT   = 0x08  //! Data chunk consists of 64 bit values
    };

    /*! @brief Fingerprint of a snapshot item
     */
    typedef struct {
        
        void *data;
        size_t size;
        uint8_t flags;
        
    } SnapshotItem;
    
    /*! @brief    List of snapshot items of this component
     *  @details  Initial value is NULL, indicating that nothing is saved to a snapshot
     */
    SnapshotItem *snapshotItems;
    
    /*! @brief    Snapshot size on disk (in bytes)
     */
    unsigned snapshotSize;
    
    /*! @brief    Registers all snapshot items for this component
     *  @abstract Snaphshot items are usually registered in the constructor of a virtual component.
     *  @param    items Pointer to the first element of a SnapshotItem* array. The end of the array
     *            is marked by a NULL pointer in the data field.
     *  @param    legth Size of the SnapshotItem array in bytes.
     */
    void registerSnapshotItems(SnapshotItem *items, unsigned length);
    
    /*! @brief    Sub components of this component
     *  @details  Initial value is NULL, indicating that no sub components are present
     */
    VirtualComponent **subComponents;

    /*! @brief    Registers all sub components for this component
     *  @abstract Sub components are usually registered in the constructor of a virtual component.
     *  @param    items Pointer to the first element of a VirtualComponet* array. The end of the array
     *            is marked by a NULL pointer in the data field.
     *  @param    legth Size of the subComponent array in bytes.
     */
    void registerSubComponents(VirtualComponent **subComponents, unsigned length);


public:
    
    //
    //! @functiongroup Loading and saving snapshots
    //

    /*! @brief    Returns size of internal state in bytes
     */
    virtual size_t stateSize();
    
    /*! @brief    Load internal state from memory buffer
     *  @note     Snapshot items of size 2, 4, or 8 are converted automatically to big endian format.
     *            Take this into account when loading byte arrays of these sizes.
     *  @param    buffer Pointer to next byte to read
     */
    virtual void loadFromBuffer(uint8_t **buffer);
    
    /*! @brief    Save internal state to memory buffer
     *  @note     Snapshot items of size 2, 4, or 8 are converted automatically to big endian format.
     *            Take this into account when saving byte arrays of these sizes.
     *  @param    buffer Pointer to next byte to read
     */
    virtual void saveToBuffer(uint8_t **buffer);
    
    
    //
    //! @functiongroup Saving single snapshot items
    //
    
    /*! @brief    Saves an 8 bit state item in big endian format
     */
    void write8(uint8_t **ptr, uint8_t value) { *((*ptr)++) = value; }
    
    /*! @brief    Saves a 16 bit state item in big endian format
     */
    void write16(uint8_t **ptr, uint16_t value) {
        write8(ptr, (uint8_t)(value >> 8)); write8(ptr, (uint8_t)value); }
    
    /*! @brief    Saves a 32 bit state item in big endian format
     */
    void write32(uint8_t **ptr, uint32_t value) {
        write16(ptr, (uint16_t)(value >> 16)); write16(ptr, (uint16_t)value); }
    
    /*! @brief    Saves a 64 bit state item in big endian format
     */
    void write64(uint8_t **ptr, uint64_t value) {
        write32(ptr, (uint32_t)(value >> 32)); write32(ptr, (uint32_t)value); }
    
    /*! @brief    Saves a byte block of arbitarary size
     *  @param    ptr    Target buffer
     *  @param    values Pointer to the beginning of the data blockbt
     *  @param    length Length of data block in bytes
     */
    void writeBlock(uint8_t **ptr, uint8_t *values, size_t length) {
        memcpy(*ptr, values, length); *ptr += length; }
    
    /*! @brief    Saves a word block of arbitarary size in big endian format
     *  @param    ptr    Target buffer
     *  @param    values Pointer to the beginning of the data block
     *  @param    length Length of data block in bytes
     */
    void writeBlock16(uint8_t **ptr, uint16_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint16_t); i++) write16(ptr, values[i]); }

    /*! @brief    Saves a double word block of arbitarary size in big endian format
     *  @param    ptr    Target buffer
     *  @param    values Pointer to the beginning of the data block
     *  @param    length Length of data block in bytes
     */
    void writeBlock32(uint8_t **ptr, uint32_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint32_t); i++) write32(ptr, values[i]); }

    /*! @brief    Saves a quad word block of arbitarary size in big endian format
     *  @param    ptr    Target buffer
     *  @param    values Pointer to the beginning of the data block
     *  @param    length Length of data block in bytes
     */
    void writeBlock64(uint8_t **ptr, uint64_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint64_t); i++) write64(ptr, values[i]); }

    
    //
    //! @functiongroup Loading single snapshot items
    //

    /*! @brief    Reads an 8 bit state item in big endian format
     */
    uint8_t read8(uint8_t **ptr) { return (uint8_t)(*((*ptr)++)); }

    /*! @brief    Reads a 16 bit state item in big endian format
     */
    uint16_t read16(uint8_t **ptr) { return ((uint16_t)read8(ptr) << 8) | (uint16_t)read8(ptr); }
    
    /*! @brief    Reads a 32 bit state item in big endian format
     */
    uint32_t read32(uint8_t **ptr) { return ((uint32_t)read16(ptr) << 16) | (uint32_t)read16(ptr); }
    
    /*! @brief    Reads a 64 bit state item in big endian format
     */
    uint64_t read64(uint8_t **ptr) { return ((uint64_t)read32(ptr) << 32) | (uint64_t)read32(ptr); }
    
    /*! @brief    Loads a byte block of arbitarary size
     *  @param    ptr    Source buffer
     *  @param    values Pointer to the beginning of the data block to write into
     *  @param    length Number of bytes to load
     */
    void readBlock(uint8_t **ptr, uint8_t *values, size_t length) {
        memcpy(values, *ptr, length); *ptr += length; }
    
    /*! @brief    Loads a word block of arbitarary size in big endian format
     *  @param    ptr    Source buffer
     *  @param    values Pointer to the beginning of the data block to write into
     *  @param    length Number of bytes to load
     */
    void readBlock16(uint8_t **ptr, uint16_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint16_t); i++) values[i] = read16(ptr); }

    /*! @brief    Loads a double word block of arbitarary size in big endian format
     *  @param    ptr    Source buffer
     *  @param    values Pointer to the beginning of the data block to write into
     *  @param    length Number of bytes to load
     */
    void readBlock32(uint8_t **ptr, uint32_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint32_t); i++) values[i] = read32(ptr); }

    /*! @brief    Loads a quad word block of arbitarary size in big endian format
     *  @param    ptr    Source buffer
     *  @param    values Pointer to the beginning of the data block to write into
     *  @param    length Number of bytes to load
     */
    void readBlock64(uint8_t **ptr, uint64_t *values, size_t length) {
        for (unsigned i = 0; i < length / sizeof(uint64_t); i++) values[i] = read64(ptr); }

    //! @brief    Reads a time delayed variable
    #define read8_delayed(var) ((c64->cycle >= var.timeStamp) ? var.value : var.prevValue)
    
    //! @brief    Writes to a time delayed variable
    void write8_delayed(uint8_delayed &var, uint8_t value);

    //! @brief    Initializes a time delayed variable
    void init8_delayed(uint8_delayed &var, uint8_t value);

    //! @brief    Initializes a time delayed variable
    void clear8_delayed(uint8_delayed &var) { init8_delayed(var, 0); }

};

#endif

