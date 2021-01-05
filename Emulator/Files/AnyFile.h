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
    
    template <class T> static T *make(FILE *file);

    template <class T> static T *make(std::istream &stream)
    {
        if (!T::isCompatibleStream(stream)) throw Error(ERROR_INVALID_TYPE);
        
        T *obj = new T();
        
        try { obj->readFromStream(stream); } catch (Error &err) {
            delete obj;
            throw err;
        }
        return obj;
    }

    template <class T> static T *make(const char *path)
    {
        std::ifstream stream(path);
        if (!stream.is_open()) throw Error(ERROR_FILE_NOT_FOUND);
        return make <T> (stream);
    }

    
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
    
    // Returns a pointer to the raw data of this file
    u8 *getData() { return data; }

    // Returns the file size in bytes
    size_t getSize() { return size; }
    
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
    // Flashing file data
    //

    // Copies the file contents into a buffer starting at the provided offset
    void flash(u8 *buffer, size_t offset = 0);

    
    //
    // Serializing
    //
    

    /* Returns true iff this specified buffer is compatible with this object.
     * This function is used in readFromBuffer().
     */
    [[deprecated]] virtual bool matchingBuffer(const u8 *buffer, size_t length) { return false; }
    
    /* Checks whether this file has the same type as the file stored in the
     * specified file.
     */
    virtual bool matchingFile(const char *path) { return false; }

protected:

    usize readFromFile(const char *path) throws;
    usize readFromBuffer(const u8 *buf, size_t len) throws;
    virtual usize readFromStream(std::istream &stream) throws;

public:
    
    usize writeToFile(const char *path) throws;
    usize writeToBuffer(u8 *buf) throws;
    virtual usize writeToStream(std::ostream &stream) throws;
    

    //
    // Repairing
    //
    
public:
    
    /* Some media files (CRTs, TAPs, T64s) contain known inconsistencies in
     * their header signature. Some subclasses overwrite this function to
     * provide a standardized place where those error can be fixed.
     */
    virtual void repair() { };
    
};
