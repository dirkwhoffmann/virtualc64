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
#include "Buffer.h"
#include "PETName.h"

#include <sstream>
#include <fstream>

namespace vc64 {

using util::Buffer;

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
    std::filesystem::path path = "";

    // The raw data of this file
    Buffer<u8> data;

    
    //
    // Initializing
    //
    
public:
    
    virtual ~AnyFile() { };

    void init(isize capacity);
    void init(const Buffer<u8> &buffer) throws;
    void init(const string &str) throws;
    void init(const std::filesystem::path &path) throws;
    void init(const u8 *buf, isize len) throws;


    //
    // Methods from MediaFile
    //

public:

    virtual isize getSize() const override { return data.size; }
    virtual u8 *getData() const override { return data.ptr; }
    virtual u64 fnv64() const override { return data.fnv64(); }
    virtual u32 crc32() const override { return data.crc32(); }


    //
    // Accessing
    //
    
public:
    
    // Returns the logical name of this file
    virtual string name() const override;
    virtual PETName<16> getName() const;

    // Returns a data byte
    u8 getData(isize nr) { return (data.ptr && nr < data.size) ? data.ptr[nr] : 0; }

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
    virtual bool isCompatibleBuffer(const u8 *buf, isize len) = 0;
    bool isCompatibleBuffer(const Buffer<u8> &buffer);

    isize readFromBuffer(const u8 *buf, isize len) throws override;
    isize readFromBuffer(const Buffer<u8> &buffer) throws;

public:
    
    isize writeToStream(std::ostream &stream, isize offset, isize len) throws;
    isize writeToFile(const std::filesystem::path &path, isize offset, isize len) throws;
    isize writeToBuffer(u8 *buf, isize offset, isize len) throws;
    isize writeToBuffer(Buffer<u8> &buffer, isize offset, isize len) throws;

    isize writeToStream(std::ostream &stream) throws override;
    isize writeToFile(const std::filesystem::path &path) throws override;
    isize writeToBuffer(u8 *buf) throws override;
    isize writeToBuffer(Buffer<u8> &buffer) throws;

private:
    
    // Delegation methods
    virtual void finalizeRead() throws { };
    virtual void finalizeWrite() throws { };
};

}
