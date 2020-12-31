// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ANYC64FILE_H
#define _ANYC64FILE_H

#include "C64Object.h"
#include "PETName.h"

// Base class for all supported file types
class AnyFile : public C64Object {
    
protected:
	     
    // The physical name (full path) of this file
    char *path = NULL;
    
    /* The logical name of this file. Some archives store a logical name in the
     * header section. If no name is stored, the logical name is constructed
     * out of the physical name by stripping off path and extension.
     */
    char name[256];
    
    /* Unicode representation of the logical name. The provides unicode format
     * is compatible with font C64ProMono which is used, e.g., in the mount
     * dialogs preview panel.
     */

    unsigned short unicode[256];
    
    // The size of this file in bytes
    size_t size = 0;

    // The raw data of this file
    u8 *data = nullptr;
    
    // File pointer (an offset into the data array)
    long fp = -1;
    
    // End of file position (equals the last valid offset plus 1)
    long eof = -1;
    
 
    //
    // Initializing
    //
    
public:
    
    AnyFile();
    AnyFile(size_t capacity);
    virtual ~AnyFile();

    
    // Frees the memory allocated by this object
    virtual void dealloc();

    
    //
    // Accessing file attributes
    //
    
public:
    
    // Returns the type of this file
    virtual FileType type() { return FILETYPE_UNKNOWN; }

    // Returns a string representation for the type of this file
    const char *typeString() { return sFileType(type()); }
    
	// Returns the physical name of this file
    const char *getPath() { return path ? path : ""; }

    // Sets the physical name of this file
    void setPath(const char *path);

    // Returns the logical name of this file
    virtual const char *getName() { return name; }
 
    // Returns the logical name as a unicode character array
    const unsigned short *getUnicodeName();
	
    // Returns a unique fingerprint for this file
    u64 fnv();
    
    
    //
    // Reading file data
    //

    // Returns a pointer to the raw data of this file
    u8 *getData() { return data; }

    // Returns the file size in bytes
    virtual size_t getSize() { return size; }

    /* Moves the file pointer to the specified offset. seek(0) returns to the
     * beginning of the file.
     */
    virtual void seek(long offset);
    
    // Reads a byte (-1 = EOF)
    virtual int read();

    /* Copies the file contents into C64 memory starting at 'offset'. 'buffer'
     * must be a pointer to RAM or ROM.
     */
    virtual void flash(u8 *buffer, size_t offset = 0);

    
    //
    // Serializing
    //
    
    // Required buffer size for this file
    size_t sizeOnDisk() { return writeToBuffer(NULL); }

    /* Checks whether this file has the same type as the file stored in the
     * specified file.
     */
    virtual bool hasSameType(const char *path) { return false; }

    // Reads the file contents from a memory buffer
    virtual bool readFromBuffer(const u8 *buffer, size_t length);
	
    // Reads the file contents from a file
	bool readFromFile(const char *path);

    /* Writes the file contents into a memory buffer. By passing a null pointer,
     * a test run is performed. Test runs are used to determine how many bytes
     * will be written.
     */
	virtual size_t writeToBuffer(u8 *buffer);

    /* Writes the file contents to a file. This function requires no custom
     * implementation. It invokes writeToBuffer first and writes the data to
     * disk afterwards.
     */
	bool writeToFile(const char *path);
};

#endif
