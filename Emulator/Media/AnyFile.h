// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "CoreObject.h"
#include "MediaFile.h"
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

class AnyFile : public CoreObject, public MediaFile {

public:

    // Physical location of this file
    fs::path path = "";

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
    void init(const fs::path &path) throws;
    void init(const fs::path &path, std::istream &stream) throws;
    void init(std::istream &stream) throws;
    void init(const u8 *buf, isize len) throws;
    void init(FILE *file) throws;
    
    
    //
    // Methods from MediaFile
    //

    // Returns the size of this file
    virtual isize getSize() const override { return size; }

    // Returns a pointer to the file data
    virtual u8 *getData() const override { return data; }


    //
    // Accessing
    //
    
public:
    
    // Returns the logical name of this file
    virtual string name() const override;
    virtual PETName<16> getName() const;

    // Returns a data byte
    u8 getData(isize nr) { return (data && nr < size) ? data[nr] : 0; }

    // Returns a fingerprint (hash value) for this file
    u64 fnv() const override;
    
    // Removes a certain number of bytes from the beginning of the file
    void strip(isize count);

    
    //
    // Flashing
    //

    // Copies the file contents into a buffer starting at the provided offset
    // DEPRECATED
    void flash(u8 *buf, isize offset = 0) const override;

    
    //
    // Serializing
    //
    
protected:

    virtual bool isCompatiblePath(const fs::path &path) = 0;
    virtual bool isCompatibleStream(std::istream &stream) = 0;
    
    void readFromStream(std::istream &stream) override;
    void readFromFile(const fs::path &path) override;
    void readFromBuffer(const u8 *buf, isize len) override;

public:
    
    void writeToStream(std::ostream &stream) override;
    void writeToFile(const fs::path &path) override;
    void writeToBuffer(u8 *buf) override;


private:
    
    // Delegation methods
    virtual void finalizeRead() throws { };
    virtual void finalizeWrite() throws { };
};

}
