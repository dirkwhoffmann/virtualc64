// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Object.h"
#include "FileTypes.h"
#include "PETName.h"

// Base class for all supported file types
class AnyFile : public C64Object {
    
protected:
	     
    // Physical location of this file on disk (if known)
    char *path = nullptr;
    
    /* The logical name of this file. Some archives store a logical name in the
     * header section. If no name is stored, the logical name is constructed
     * out of the physical name by stripping off path and extension.
     */
    char name[256];
    
    // The raw data of this file
    u8 *data = nullptr;
    
    // The size of this file in bytes
    usize size = 0;
    

    //
    // Creating
    //
    
public:
    
    template <class T> static T *make(const u8 *buf, size_t len);
    template <class T> static T *make(const char *path);
    template <class T> static T *make(FILE *file);

    
    //
    // Initializing
    //
    
public:
    
    AnyFile();
    AnyFile(usize capacity);
    virtual ~AnyFile();

    // Allocates memory for storing the object data
    virtual bool alloc(usize capacity);

    // Frees the memory allocated by this object
    virtual void dealloc();

    
    //
    // Accessing file attributes
    //
    
public:
    
    // Returns the type of this file
    virtual FileType type() { return FILETYPE_UNKNOWN; }
    
	// Returns the physical name of this file
    const char *getPath() { return path ? path : ""; }

    // Sets the physical name of this file
    void setPath(const char *path);

    // Returns the logical name of this file
    virtual const char *getName() { return name; }
 
    // Returns a unique fingerprint for this file
    u64 fnv();
    
    
    //
    // Reading file data
    //

    // Returns a pointer to the raw data of this file
    u8 *getData() { return data; }

    // Returns the file size in bytes
    virtual size_t getSize() { return size; }

    /* Copies the file contents into C64 memory starting at 'offset'. 'buffer'
     * must be a pointer to RAM or ROM.
     */
    virtual void flash(u8 *buffer, size_t offset = 0);

    
    //
    // Serializing
    //
    
    // Returns the required buffer size for this file
    size_t sizeOnDisk() { return writeToBuffer(nullptr); }

    /* Returns true iff this specified buffer is compatible with this object.
     * This function is used in readFromBuffer().
     */
    virtual bool matchingBuffer(const u8 *buffer, size_t length) { return false; }
    
    /* Checks whether this file has the same type as the file stored in the
     * specified file.
     */
    virtual bool matchingFile(const char *path) { return false; }

protected:

    /* Deserializes this object from a memory buffer. This function uses
     * matchingBuffer() to verify that the buffer contains a compatible
     * binary representation.
     */
    virtual void readFromBuffer(const u8 *buf, size_t len) throws;
    
    /* Deserializes this object from a file. This function uses
     * matchingFile() to verify that the file contains a compatible binary
     * representation. This function requires no custom implementation. It
     * first reads in the file contents in memory and invokes readFromBuffer
     * afterwards.
     */
    void readFromFile(const char *path) throws;

    /* Deserializes this object from a file that is already open.
     */
    void readFromFile(FILE *file) throws;
    
public:
    
    /* Writes the file contents into a memory buffer. By passing a null pointer,
     * a test run is performed. Test runs are used to determine how many bytes
     * will be written.
     */
	virtual size_t writeToBuffer(u8 *buf) throws;

    /* Writes the file contents to a file. This function requires no custom
     * implementation. It invokes writeToBuffer first and writes the data to
     * disk afterwards.
     */
	virtual size_t writeToFile(const char *path) throws;

    // virtual size_t writeToFile(FILE *file) throws;
};
