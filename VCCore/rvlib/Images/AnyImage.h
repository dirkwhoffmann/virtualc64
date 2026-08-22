// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/ImageTypes.h"
#include "utl/abilities.h"
#include "utl/storage.h"
#include "utl/primitives/Range.h"
#include <iostream>

namespace retro::vault {

// extern utl::LogChannel HDF_DBG;

/* Common base of everything the library treats as an image.
 *
 * It holds only what every image has: a location on disk, a type and format it
 * can name itself by, and the ability to write itself back. Deliberately *not*
 * a byte buffer -- most images are one, and those derive from BinaryImage,
 * which adds the buffer and everything that reads it. An SVM is the case that
 * forced the distinction: it is a directory tree, and a folder-backed one has
 * no byte stream at all, so inheriting getSize(), byteView() or writeToFile()
 * would only have given it members that answer for data it does not have.
 *
 * Hashable stays here because identity is universal -- an SVM hashes its
 * manifest. Dumpable does not: a hex dump is a statement about bytes, so it
 * belongs to BinaryImage.
 */
class AnyImage : public utl::Hashable, public utl::Loggable {

public:

    // The location of this file (may be empty)
    fs::path path;


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

    // Checks if the URL points to an image of the same type
    virtual bool validateURL(const fs::path& url) const noexcept = 0;


    //
    // Querying meta information
    //

public:

    virtual ImageType type() const noexcept = 0;
    virtual ImageFormat format() const noexcept = 0;
    ImageInfo info() const noexcept { return { type(), format() }; }

    virtual std::vector<string> describeImage() const noexcept { return {}; }


    //
    // Checking consistency
    //

public:

    // Scans the image and throws an exception if an inconsistency is found
    virtual void checkIntegrity() { };


    //
    // Exporting
    //

public:

    /* Writes the image back to 'path'.
     *
     * Pure, because what "writing back" means is exactly what differs between
     * an image that is a buffer and one that is a tree: BinaryImage dumps its
     * bytes, SVMFile packs its root folder (see SVMFile::persist).
     */
    virtual void save() = 0;
};

}
