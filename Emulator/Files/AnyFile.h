// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
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
    isize size = 0;
    

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

    /*
    template <class T> static T *make(std::istream &stream, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (stream); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    */
    
    template <class T> static T *make(const u8 *buf, isize len) throws
    {
        std::stringstream stream;
        stream.write((const char *)buf, len);
        return make <T> (stream);
    }
    
    /*
    template <class T> static T *make(const u8 *buf, isize len, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (buf, len); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    */
    
    template <class T> static T *make(const string &path) throws
    {
        if (!T::isCompatiblePath(path)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
        
        std::ifstream stream(path);
        if (!stream.is_open()) throw VC64Error(ERROR_FILE_NOT_FOUND, path);
        auto file = make <T> (stream);
        file->path = path;
        return file;
    }

    /*
    template <class T> static T *make(const string &path, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (path); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    */
    
    template <class T> static T *make(class Disk &disk) throws
    {
        return T::makeWithDisk(disk);
    }

    /*
    template <class T> static T *make(class Disk &disk, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (disk); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    */
    
    template <class T> static T *make(class FSDevice &fs) throws
    {
        return T::makeWithFileSystem(fs);
    }

    /*
    template <class T> static T *make(class FSDevice &fs, ErrorCode *err)
    {
        *err = ERROR_OK;
        try { return make <T> (fs); }
        catch (VC64Error &exception) { *err = exception.data; }
        return nullptr;
    }
    */
    
    
    //
    // Initializing
    //
    
public:
    
    AnyFile() { };
    AnyFile(isize capacity);
    virtual ~AnyFile();
    
    //
    // Accessing
    //
    
public:
    
    // Returns the logical name of this file
    virtual PETName<16> getName() const;

    // Determines the type of an arbitrary file on file
    static FileType type(const string &path);

    // Returns the media type of this file
    virtual FileType type() const { return FILETYPE_UNKNOWN; }
     
    // Returns a data byte
    u8 getData(isize nr) { return (data && nr < size) ? data[nr] : 0; }
        
    // Returns a fingerprint (hash value) for this file
    u64 fnv() const;
    
    // Removes a certain number of bytes from the beginning of the file
    void strip(isize count);

    
    //
    // Flashing
    //

    // Copies the file contents into a buffer starting at the provided offset
    void flash(u8 *buf, isize offset = 0);

    
    //
    // Serializing
    //
    
protected:

    virtual isize readFromStream(std::istream &stream) throws;
    isize readFromFile(const string &path) throws;
    isize readFromBuffer(const u8 *buf, isize len) throws;

public:
    
    virtual isize writeToStream(std::ostream &stream) throws;
    isize writeToStream(std::ostream &stream, ErrorCode *ec);

    isize writeToFile(const string &path) throws;
    isize writeToFile(const string &path, ErrorCode *ec);
    
    isize writeToBuffer(u8 *buf) throws;
    isize writeToBuffer(u8 *buf, ErrorCode *ec);


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
