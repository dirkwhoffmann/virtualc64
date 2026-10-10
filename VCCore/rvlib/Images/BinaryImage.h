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
#include "utl/abilities/Compressible.h"
#include "utl/storage/BackedBuffer.h"

namespace retro::vault {

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
 * - The image keeps the file open for as long as it lives. Parts not loaded
 *   yet are read when first asked for, so nobody else may change the file in
 *   the meantime.
 *
 * - Writing the changes back to the file (save()) is safe. Copying the image
 *   onto that file is refused, because the image would lose the bytes it has
 *   not loaded yet.
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
 * Compression:
 *
 * Images are independent of compression. Compression is handled by the import
 * and export functions, which accept a `utl::Compressor` and default to `NONE`.
 * When importing a compressed byte stream, the data is uncompressed into memory
 * before the image is created. The resulting image is therefore memory-backed
 * and has no knowledge of the original file or its compression. When exporting,
 * the image data is compressed before it is written to the output stream.
 */

class BinaryImage : public AnyImage, public utl::Dumpable {

    using Compressor = utl::Compressor;

    // The raw data of this file
    utl::BackedBuffer data;


    //
    // Initializing
    //

public:

    /* Creates an image of the given size.
     *
     * The image is initialized with all zeroes.
     */
    void init(isize len);

    /* Creates an image holding a copy of the given bytes.
     *
     * If a compressor is given, the bytes are the compressed form of the
     * image and are uncompressed first.
     */
    void init(const u8 *buf, isize len, Compressor compressor = Compressor::NONE);

    /* Creates an image on top of a file, loading its contents lazily.
     *
     * A compressed file cannot be read in pieces. If a compressor is given,
     * the file is therefore uncompressed in full, and the image holds the
     * result in memory: it is not file backed, has no path, and save() has
     * nowhere to write to (use saveAs()).
     */
    void init(const fs::path& p, Compressor compressor = Compressor::NONE);

    /* Initializes the image with the contents of a device.
     *
     * The device is asked for its bytes rather than handing over a pointer to
     * them, so this works for any device, including ones that do not keep the
     * whole image in memory. All of them are copied.
     */
    void init(const class LinearDevice& device);

protected:

    // Changes the size of the image, keeping its contents
    void resize(isize len);


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

    // Returns the image size in bytes
    isize getSize() const { return data.size(); }

    // Returns true if the image contains no data
    bool empty() const { return data.empty(); }

    // Returns true if the image is backed by a file
    bool fileBacked() const { return data.backed(); }

    // Returns true if the backing file has outdated data
    bool modified() const { return data.dirty(); }


    //
    // Accessing data
    //

public:

    // Returns a view of bytes [offset, offset + len) within the image
    utl::ByteView byteView(isize offset, isize len) const;
    utl::MutableByteView mutableByteView(isize offset, isize len);

    // Turns a file backed image into a memory backed image
    void detach();


    //
    // Exporting
    //

public:

    /* Writes the modifications back to the backing file.
     *
     * Only what has been modified is written. No action is performed if no
     * backing file is present.
     */
    void save() override;

    /* Writes the entire image to a new, plain file and continues on top of it.
     *
     * Later calls to save() go to the new file. The file is written before
     * the image switches over, so if writing fails, nothing changes. Views
     * taken before the call must not be used after it. Saving as the file the
     * image already lives in is the same as save(). To write a copy instead,
     * which leaves the image where it is, use copy().
     */
    void saveAs(const fs::path &path);

    /* Writes bytes [offset, offset + len), or the entire image, into a buffer,
     * to a stream or a file, compressed if a compressor is given.
     *
     * The image is not changed in any way, in particular it keeps its backing
     * file, which is also why it cannot be the target of a copy: copying onto
     * the file the image lives in throws. The result is the number of bytes
     * written, which is the compressed size if the data was compressed.
     */
    void copy(u8 *dst, isize offset = 0) const;
    void copy(u8 *dst, isize offset, isize len) const;

    isize copy(std::ostream &stream,
               Compressor compressor = Compressor::NONE) const;
    isize copy(std::ostream &stream, isize offset, isize len,
               Compressor compressor = Compressor::NONE) const;

    isize copy(const fs::path &path,
               Compressor compressor = Compressor::NONE) const;
    isize copy(const fs::path &path, isize offset, isize len,
               Compressor compressor = Compressor::NONE) const;

    //
    // Delegates
    //

private:

    // Called at the end of init()
    virtual void didInitialize() {};
};

}
