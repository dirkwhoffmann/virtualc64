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
#include "C64Types.h"
#include "MediaFileTypes.h"
#include "MediaFile.h"
#include "PETName.h"
#include "utl/storage/Buffer.h"
#include "FileSystems/CBM/FSObjects.h"

#include <sstream>
#include <fstream>

namespace vc64 {

using retro::vault::cbm::PETName;
using utl::Buffer;

/* All media files are organized in the class hierarchy displayed below. AnyFile
 * provides basic functionalities for reading and writing files, streams, and
 * buffers. AnyCollection provides an abstract interface for accessing
 * single files. This interface is implemented for those formats that organize
 * their contents as a collection of files (in contrast to those formats that
 * organzie their contents as a collection of tracks or sectors, or do not store
 * files at all).
 *
 *     ---------
 *    | AnyFile |     Internal classes
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

/// Base class of all media files
///
/// Provides basic functionality for reading and writing files, streams, and buffers.
class AnyFile : public CoreObject {

public:

    /// Physical location of this file
    std::filesystem::path path = "";

    /// The raw data of this file
    Buffer<u8> data;

    
    //
    // Static methods
    //

    /// Determines the type of an arbitrary file on disk
    static FileType type(const fs::path &path);

    // Factory methods
    /// Creates a media file from a file on disk. The file type is determined automatically
    static AnyFile *make(const fs::path &path);
    /// Creates a media file of a given type from a file on disk
    static AnyFile *make(const fs::path &path, FileType type);
    /// Creates a media file of a given type from a memory buffer
    static AnyFile *make(const u8 *buf, isize len, FileType type);
    /// Creates a media file of a given type from the contents of a drive
    static AnyFile *make(class DriveAPI &drive, FileType type);

    
    //
    // Initializing
    //
    
public:
    
    /// Destructor
    virtual ~AnyFile() { };

    /// Initializes the file with an empty buffer of the given capacity
    void init(isize capacity);
    /// Initializes the file with the contents of a buffer
    void init(const Buffer<u8> &buffer);
    /// Initializes the file with the contents of a string
    void init(const string &str);
    /// Initializes the file with the contents of a file
    void init(const std::filesystem::path &path);
    /// Initializes the file with the contents of a memory region
    void init(const u8 *buf, isize len);

    
    //
    // Methods
    //

public:
    
    // Returns the media type of this file
    /// Returns the media type of this file
    virtual FileType type() const { return FileType::UNKNOWN; }

    // Returns the size of this file
    /// Returns the size of this file
    virtual isize getSize() const { return data.size; }

    // Returns a textual representation of the file size
    // virtual string getSizeAsString() const;

    // Returns a pointer to the file data
    /// Returns a pointer to the file data
    virtual u8 *getData() const { return data.ptr; }

    // Returns the logical name of this file
    // virtual string name() const = 0;

    // Returns a fingerprint (hash value) for this file
    /// Returns a FNV-64 fingerprint of this file
    virtual u64 fnv64() const { return data.fnv64(); }
    /// Returns a CRC-32 fingerprint of this file
    virtual u32 crc32() const { return data.crc32(); }
    

    //
    // Accessing
    //
    
public:
    
    // Returns the logical name of this file
    /// Returns the logical name of this file
    virtual string name() const;
    /// Returns the logical name of this file in PETSCII encoding
    virtual PETName<16> getName() const;

    // Returns a data byte
    /// Returns a data byte
    u8 getData(isize nr) { return (data.ptr && nr < data.size) ? data.ptr[nr] : 0; }

    /// Removes a certain number of bytes from the beginning of the file
    void strip(isize count);

    
    //
    // Flashing
    //

    // Copies the file contents into a buffer
    /// Copies a portion of the file contents into a buffer
    virtual void flash(u8 *buf, isize offset, isize len) const;
    /// Copies the file contents (starting at an offset) into a buffer
    virtual void flash(u8 *buf, isize offset = 0) const;

    
    //
    // Serializing
    //
    
protected:

    virtual bool isCompatiblePath(const fs::path &path) const = 0;
    virtual bool isCompatibleBuffer(const u8 *buf, isize len) const = 0;
    bool isCompatibleBuffer(const Buffer<u8> &buffer) const;

    isize readFromBuffer(const u8 *buf, isize len);
    isize readFromBuffer(const Buffer<u8> &buffer);

public:
    
    /// Writes a portion of the file to a stream
    isize writeToStream(std::ostream &stream, isize offset, isize len);
    /// Writes a portion of the file to a file on disk
    isize writeToFile(const std::filesystem::path &path, isize offset, isize len);
    /// Writes a portion of the file to a memory buffer
    isize writeToBuffer(u8 *buf, isize offset, isize len);
    /// Writes a portion of the file to a buffer
    isize writeToBuffer(Buffer<u8> &buffer, isize offset, isize len);

    /// Writes the file to a stream
    isize writeToStream(std::ostream &stream);
    /// Writes the file to a file on disk
    isize writeToFile(const std::filesystem::path &path);
    /// Writes the file to a memory buffer
    isize writeToBuffer(u8 *buf);
    /// Writes the file to a buffer
    isize writeToBuffer(Buffer<u8> &buffer);

private:
    
    // Delegation methods
    virtual void finalizeRead() { };
    virtual void finalizeWrite() { };
};

}
