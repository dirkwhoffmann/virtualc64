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
#include <sstream>
#include <fstream>

/* All media files are organized in the class hierarchy displayed below. Two
 * abstract classes are involed: AnyFile and AnyCollection. AnyFiles provides
 * basic functionality for reading and writing files, streams, and buffers.
 * AnyCollection provides an abstract interface for accessing single files.
 * This interface is implemented for those formats that organize their contents
 * as a collection of files (in contrast to those formats that organzie their
 * contents as a collection of tracks or sectors, or do not store files at all).
 *
 *     ---------
 *    | AnyFile |
 *     ---------
 *         |
 *         |--------------------------------------------------
 *         |         |       |       |       |       |        |
 *         |     ---------   |   ---------   |   ---------    |
 *         |    | ROMFile |  |  |TAPFile  |  |  | D64File |   |
 *         |     ---------   |   ---------   |   ---------    |
 *         |             ----------      ----------       ---------
 *         |            | Snapshot |    | CRTFile  |     | G64File |
 *         |             ----------      ----------       ---------
 *         |
 *  ---------------
 * | AnyCollection |
 *  ---------------
 *         |
 *         |-----------------------------------------------
 *                   |           |            |            |
 *               ---------   ---------    ---------    ---------
 *              | T64File | | PRGFile |  | P00File |  | Folder  |
 *               ---------   ---------    ---------    ---------
 */
  
class AnyFile : public C64Object {
    
public:
	     
    // Physical location of this file
    string path = "";
    
    // The raw data of this file
    u8 *data = nullptr;
    
    // The size of this file in bytes
    usize size = 0;
    

    //
    // Creating
    //
    
public:
    
    template <class T> static T *make(std::istream &stream) throws
    {
        if (!T::isCompatibleStream(stream)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
        
        T *obj = new T();
        
        try { obj->readFromStream(stream); } catch (VC64Error &err) {
            delete obj;
            throw err;
        }
        return obj;
    }

    template <class T> static T *make(std::istream &stream, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (stream); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
        
    template <class T> static T *make(const u8 *buf, usize len) throws
    {
        std::stringstream stream;
        stream.write((const char *)buf, len);
        return make <T> (stream);
    }
    
    template <class T> static T *make(const u8 *buf, usize len, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (buf, len); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    
    template <class T> static T *make(const string &path) throws
    {
        if (!T::isCompatibleName(path)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
        
        std::ifstream stream(path);
        if (!stream.is_open()) throw VC64Error(ERROR_FILE_NOT_FOUND);
        return make <T> (stream);
    }

    template <class T> static T *make(const string &path, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (path); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    
    template <class T> static T *make(class Disk &disk) throws
    {
        return T::makeWithDisk(disk);
    }

    template <class T> static T *make(class Disk &disk, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (disk); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }

    template <class T> static T *make(class FSDevice &fs) throws
    {
        return T::makeWithFileSystem(fs);
    }

    template <class T> static T *make(class FSDevice &fs, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (fs); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    
    
    //
    // Initializing
    //
    
public:
    
    AnyFile() { };
    AnyFile(usize capacity);
    virtual ~AnyFile();

    
    //
    // Accessing
    //
    
public:
    
    // Returns the logical name of this file
    virtual PETName<16> getName() const;

    // Returns the media type of this file
    virtual FileType type() const { return FILETYPE_UNKNOWN; }
     
    // Returns a fingerprint (hash value) for this file
    u64 fnv() const;
    
    
    //
    // Flashing data
    //

    // Copies the file contents into a buffer starting at the provided offset
    void flash(u8 *buf, usize offset = 0);

    
    //
    // Serializing
    //
    
protected:

    virtual usize readFromStream(std::istream &stream) throws;
    usize readFromFile(const char *path) throws;
    usize readFromBuffer(const u8 *buf, usize len) throws;

public:
    
    virtual usize writeToStream(std::ostream &stream) throws;
    usize writeToStream(std::ostream &stream, ErrorCode *err);

    usize writeToFile(const char *path) throws;
    usize writeToFile(const char *path, ErrorCode *err);
    
    usize writeToBuffer(u8 *buf) throws;
    usize writeToBuffer(u8 *buf, ErrorCode *err);


    //
    // Repairing
    //
    
public:
    
    /* This function is called in the default implementation of readFromStream.
     * It can be overwritten to fix known inconsistencies in certain media
     * files.
     */
    virtual void repair() { };    
};
