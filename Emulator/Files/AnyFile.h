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

// Base class for all supported media file formats (D64, T64, PRG, etc.)
class AnyFile : public C64Object {
    
protected:
	     
    // Physical location of this file
    string path = "";
    
    // The raw data of this file
    u8 *data = nullptr;
    
    // The size of this file in bytes
    usize size = 0;
    

    //
    // Generating
    //
    
public:
    
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

    template <class T> static T *make(const u8 *buf, size_t len)
    {
        std::stringstream stream;
        stream.write((const char *)buf, len);
        return make <T> (stream);
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
    
    AnyFile() { };
    AnyFile(usize capacity);
    virtual ~AnyFile();

    // Allocates memory for storing the object data
    virtual bool alloc(usize capacity);

    
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
    string getPath() { return path; }

    // Sets the physical name of this file
    void setPath(string path);

    // Returns the logical name of this file
    virtual PETName<16> getName();
 
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
