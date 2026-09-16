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
#include "utl/storage/BackedBuffer.h"

namespace retro::vault {

class LinearDevice;

/* An image that is a contiguous block of bytes.
 *
 * This is what almost every format in this library is: a floppy image, a hard
 * drive image, an executable. Everything below -- sizing, copying, hashing,
 * dumping, exporting -- is a statement about those bytes.
 *
 * The bytes live in a utl::BackedBuffer. An image read from a file sits on
 * top of that file and loads its bytes as they are asked for, so opening a
 * hard drive image of several gigabytes costs next to nothing until its
 * blocks are used. Images built in memory -- from a size, from bytes, or from
 * a device -- have no backing and hold all of their bytes.
 *
 * What lazy loading means for an image read from a file:
 *
 * - The image keeps the file open for as long as it lives.
 * - It is not a snapshot. Parts not loaded yet are read when first asked for,
 *   so nobody else may change the file in the meantime. Writing the image to
 *   its own file (save(), writeToFile()) is safe; that case is handled.
 *
 * The bytes are private. Everything, subclasses included, reaches them through
 * two views:
 *
 *   byteView()         read-only
 *   mutableByteView()  writable. Ask for it only when writing: the region
 *                      counts as modified from then on.
 *
 * Images cannot be copied, because two copies would share one backing.
 *
 * Formats that are not a buffer derive from AnyImage directly (see SVMFile)
 * and simply do not have these members.
 */
class BinaryImage : public AnyImage, public utl::Dumpable {

    // The raw data of this file
    utl::BackedBuffer data;


    //
    // Initializing
    //

public:

    // Creates an image of the given size, all zero
    void init(isize len);

    // Creates an image holding a copy of the given bytes
    void init(const u8 *buf, isize len);

    // Creates an image on top of a file, loading its contents lazily
    void init(const fs::path& p);

    /* Initializes the image with the contents of a device.
     *
     * The device is asked for its bytes rather than handing over a pointer to
     * them, so this works for any device, including ones that do not keep the
     * whole image in memory. All of them are copied.
     */
    void init(const LinearDevice& device);

protected:

    /* Choosing the storage of an image read from a file.
     *
     * init(path) asks makeBacking() where the bytes come from, and imageSize()
     * how large the image is. Formats whose files are not the image itself
     * override them: a compressed file needs a backing that unpacks it, and a
     * short file an image larger than itself.
     *
     * imageSize() is handed the backing, not just its size, so a format can
     * look inside first -- an HDF reads its rigid disk block to learn how
     * large the drive is meant to be. Nothing it reads stays loaded.
     */
    virtual std::unique_ptr<utl::Backing> makeBacking(const fs::path &path) const;
    virtual isize imageSize(utl::Backing &backing) const { return backing.size(); }


    //
    // Methods from Hashable
    //

public:

    u64 hash(HashAlgorithm algorithm) const override {
        return byteView(0, getSize()).hash(algorithm);
    }


    //
    // Methods from Dumpable
    //

public:

    Dumpable::DataProvider dataProvider() const override {
        return byteView(0, getSize()).dataProvider();
    }


    //
    // Querying meta information
    //

public:

    isize getSize() const { return data.size(); }
    bool empty() const { return data.empty(); }

    /* Returns where the bytes of the image live
     *
     * FILE_BACKED if the image sits on top of the file it was read from or
     * saved to, MEMORY_BACKED if it was built in memory or let go of its file
     * (see detach()).
     */
    StorageMode getStorageMode() const {
        return data.backed() ? StorageMode::FILE_BACKED : StorageMode::MEMORY_BACKED;
    }

    /* Returns true if the image holds changes the file does not have yet
     *
     * A memory-backed image has no file to compare itself to; for such an
     * image, the answer says nothing (see save()).
     */
    bool modified() const { return data.dirty(); }


    //
    // Accessing data
    //

public:

    // Returns a view of bytes [offset, offset + len), which must lie within the image
    utl::ByteView byteView(isize offset, isize len) const;
    utl::MutableByteView mutableByteView(isize offset, isize len);

    /* Loads the image into memory and lets go of its file.
     *
     * The image keeps its contents and forgets where they came from: it is
     * then an image built in memory, like any other, and the file is no
     * longer touched. Modifications that were never saved are kept.
     */
    void detach();

    // Copies the file contents into a buffer
    virtual void copy(u8 *dst, isize offset, isize len) const;
    virtual void copy(u8 *dst, isize offset = 0) const;


    //
    // Exporting
    //

public:

    /* Writes the modifications back to the file the image came from.
     *
     * Only what has been modified is written (see utl::BackedBuffer::persist),
     * in the file's own format -- a compressed file is packed again. An image
     * that was built in memory has no file to go back to; for such an image,
     * save() is saveAs(path).
     */
    void save() override;

    /* Writes the entire image to a new file and continues on top of it.
     *
     * Later calls to save() go to the new file. The file is written before
     * the image switches over, so if writing fails, nothing changes. Views
     * taken before the call must not be used after it.
     */
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
