// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/AnyImage.h"

namespace retro::vault {

/* An image that is a contiguous block of bytes, read into memory in one go.
 *
 * This is what almost every format in this library is: a floppy image, a hard
 * drive image, an executable. The file is slurped into 'data' by init(), and
 * everything below -- sizing, views, copying, hashing, dumping, exporting --
 * is a statement about that buffer.
 *
 * Formats that are not a buffer derive from AnyImage directly (see SVMFile)
 * and simply do not have these members.
 */
class BinaryImage : public AnyImage, public utl::Dumpable {

public:

    // The raw data of this file
    utl::Buffer<u8> data;


    //
    // Initializing
    //

public:

    void init(isize len);
    void init(const u8 *buf, isize len);
    void init(const utl::Buffer<u8>& buffer);
    // void init(const string& str);
    void init(const fs::path& p);


    //
    // Methods from Hashable
    //

public:

    u64 hash(HashAlgorithm algorithm) const override {
        return data.hash(algorithm);
    }


    //
    // Methods from Dumpable
    //

public:

    Dumpable::DataProvider dataProvider() const override {
        return data.dataProvider();
    }


    //
    // Querying meta information
    //

public:

    isize getSize() const { return data.size; }
    u8* getData() const { return data.ptr; }
    bool empty() const { return data.empty(); }


    //
    // Accessing data
    //

public:

    utl::ByteView byteView(isize offset = 0) const;
    utl::ByteView byteView(isize offset, isize len) const;
    utl::MutableByteView byteView(isize offset = 0);
    utl::MutableByteView byteView(isize offset, isize len);

    // Copies the file contents into a buffer
    virtual void copy(u8 *dst, isize offset, isize len) const;
    virtual void copy(u8 *dst, isize offset = 0) const;


    //
    // Exporting
    //

public:

    // Update the image or a portion of the image on disk
    void save() override;
    void save(const utl::Range<isize> range);
    void save(const std::vector<utl::Range<isize>> ranges);

    // Create a new image file on disk and update it with the current contents
    void saveAs(const fs::path &path);

    virtual isize writeToStream(std::ostream &stream) const;
    virtual isize writeToFile(const fs::path &path) const;

    virtual isize writeToStream(std::ostream &stream, isize offset, isize len) const;
    virtual isize writeToFile(const fs::path &path, isize offset, isize len) const;

private:

    // Called at the end of init()
    virtual void didInitialize() {};
};

}
