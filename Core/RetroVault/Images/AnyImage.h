// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ImageTypes.h"
#include "utl/abilities.h"
#include "utl/storage.h"
#include "utl/primitives/Range.h"
#include <iostream>

namespace retro::vault {

using namespace utl;

extern LogChannel HDF_DBG;

class AnyImage : public Hashable, public Dumpable, public Loggable {

public:

    // The location of this file (may be empty)
    fs::path path;
    
    // File handle (if 'path' is provided)
    std::fstream file;

    // The raw data of this file
    Buffer<u8> data;


    //
    // Static functions
    //

public:
    
    // Analyzes the type and format of the specified file
    static optional<ImageInfo> about(const fs::path& url);

    // Image factory
    static unique_ptr<AnyImage> tryMake(const fs::path& path);
    static unique_ptr<AnyImage> make(const fs::path& path);


    //
    // Initializing
    //

public:

    virtual ~AnyImage() = default;

    void init(isize len);
    void init(const u8 *buf, isize len);
    void init(const Buffer<u8>& buffer);
    // void init(const string& str);
    void init(const fs::path& path);

    // Checks if the URL points to an image of the same type
    virtual bool validateURL(const fs::path& url) const noexcept = 0;


    //
    // Methods from Hashable
    //

public:

    u64 hash(HashAlgorithm algorithm) const noexcept override {
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

    virtual ImageType type() const noexcept = 0;
    virtual ImageFormat format() const noexcept = 0;
    ImageInfo info() const noexcept { return { type(), format() }; }

    isize getSize() const { return data.size; }
    u8* getData() const { return data.ptr; }
    bool empty() const { return data.empty(); }

    virtual std::vector<string> describeImage() const noexcept { return {}; }


    //
    // Checking consistency
    //

public:
    
    // Scans the image and throws an exception if an inconsistency is found
    virtual void checkIntegrity() { };

    //
    // Accessing data
    //

public:

    ByteView byteView(isize offset = 0) const;
    ByteView byteView(isize offset, isize len) const;
    MutableByteView byteView(isize offset = 0);
    MutableByteView byteView(isize offset, isize len);

    // Copies the file contents into a buffer
    virtual void copy(u8 *dst, isize offset, isize len) const;
    virtual void copy(u8 *dst, isize offset = 0) const;


    //
    // Exporting
    //

public:

    // Update the image or a portion of the image on disk
    void save();
    void save(const Range<isize> range);
    void save(const std::vector<Range<isize>> ranges);
    
    // Create a new image file on disk and update it with the current contents
    void saveAs(const fs::path &path);
    
    isize writeToStream(std::ostream &stream) const;
    isize writeToFile(const fs::path &path) const;

    isize writeToStream(std::ostream &stream, isize offset, isize len) const;
    isize writeToFile(const fs::path &path, isize offset, isize len) const;

private:

    // Called at the end of init()
    virtual void didInitialize() {};
};

}
