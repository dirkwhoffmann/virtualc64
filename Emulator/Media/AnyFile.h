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

namespace vc64 {

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
    // Initializing
    //
    
public:
    
    AnyFile() { };
    AnyFile(isize capacity);
    virtual ~AnyFile();
    
    void init(isize capacity);
    void init(const string &path) throws;
    void init(const string &path, std::istream &stream) throws;
    void init(std::istream &stream) throws;
    void init(const u8 *buf, isize len) throws;
    void init(FILE *file) throws;
    
    
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
    // DEPRECATED
    void flash(u8 *buf, isize offset = 0) const;

    
    //
    // Serializing
    //
    
protected:

    virtual bool isCompatiblePath(const string &path) = 0;
    virtual bool isCompatibleStream(std::istream &stream) = 0;
    
    void readFromStream(std::istream &stream) throws;
    void readFromFile(const string &path) throws;
    void readFromBuffer(const u8 *buf, isize len) throws;

public:
    
    void writeToStream(std::ostream &stream) throws;
    void writeToFile(const string &path) throws;
    void writeToBuffer(u8 *buf) throws;


private:
    
    // Delegation methods
    virtual void finalizeRead() throws { };
    virtual void finalizeWrite() throws { };
};

}
