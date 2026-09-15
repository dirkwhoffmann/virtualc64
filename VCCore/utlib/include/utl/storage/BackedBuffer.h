// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "utl/primitives/ByteView.h"
#include "utl/storage/Buffer.h"
#include <fstream>
#include <memory>
#include <vector>

namespace utl {

/* Where a BackedBuffer takes its contents from, and puts its modifications.
 *
 * A backing is plain storage with a length. It is asked for bytes it holds and
 * told to overwrite bytes it holds; the only way it ever grows is extend().
 * Anything smarter -- a compressed file, say, that has to be unpacked when it
 * is opened and packed again when it is written -- is a Backing of its own,
 * doing that work in its constructor and in flush(). GzipBacking is one.
 */
class Backing {

public:

    virtual ~Backing() = default;

    // Number of bytes the backing holds
    virtual isize size() const = 0;

    // Copies bytes [offset, offset + len), which lie within size(), into dst
    virtual void read(u8 *dst, isize offset, isize len) = 0;

    // Overwrites bytes [offset, offset + len), which lie within size()
    virtual void write(const u8 *src, isize offset, isize len) = 0;

    // Grows the backing to newSize bytes, the new ones being zero
    virtual void extend(isize newSize) = 0;

    /* Called once at the end of every persist() that wrote something.
     *
     * What write() was given may be held back until here, and counts as
     * persisted only once flush() has returned. A backing that cannot store
     * it throws, and persist() then treats every page as unwritten.
     */
    virtual void flush() { }
};

/* A backing that is a file.
 *
 * The file must exist. It is read as needed and opened for writing only when
 * something has to be written, so a file that cannot be written is fine for
 * as long as nobody tries.
 */
class FileBacking : public Backing {

    fs::path path;
    std::ifstream in;
    std::fstream out;
    isize bytes = 0;

public:

    explicit FileBacking(const fs::path &path);

    isize size() const override { return bytes; }
    void read(u8 *dst, isize offset, isize len) override;
    void write(const u8 *src, isize offset, isize len) override;
    void extend(isize newSize) override;
    void flush() override;
};

/* A backing that is a buffer in memory.
 *
 * The buffer belongs to the caller and must outlive the backing. extend()
 * resizes it, which moves its memory.
 */
class BufferBacking : public Backing {

    Buffer<u8> &buffer;

public:

    explicit BufferBacking(Buffer<u8> &buffer) : buffer(buffer) { }

    isize size() const override { return buffer.size; }
    void read(u8 *dst, isize offset, isize len) override;
    void write(const u8 *src, isize offset, isize len) override;
    void extend(isize newSize) override;
};

/* A backing that is a gzip compressed file, such as an .hdz or .adz image.
 *
 * A compressed file cannot be read or written in pieces, so the whole file is
 * unpacked into memory when the backing is created, and everything else
 * works on that. A buffer on top still loads lazily -- it copies pages out of
 * the unpacked data as they are asked for -- but the unpacking is eager.
 *
 * flush() packs the data and rewrites the file in full. It compresses before
 * it opens the file, so a failing compression leaves the file as it was; a
 * failing write does not.
 *
 * An empty file holds nothing, and nothing is written as an empty file.
 */
class GzipBacking : public Backing {

    fs::path path;
    Buffer<u8> data;

public:

    // Unpacks the file. Throws if it cannot be read or is not gzip data.
    explicit GzipBacking(const fs::path &path);

    isize size() const override { return data.size; }
    void read(u8 *dst, isize offset, isize len) override;
    void write(const u8 *src, isize offset, isize len) override;
    void extend(isize newSize) override;
    void flush() override;
};

/* A byte buffer that loads its contents from a backing as they are needed.
 *
 * The buffer has a size of its own. The backing may be shorter -- the bytes
 * it does not cover read as zero -- or longer, in which case the excess is
 * neither read nor touched.
 *
 * Contents are handed out as views. Asking for a view loads whatever part of
 * the requested region has not been loaded before; the rest of the buffer
 * stays where it is, in the backing. What a view shows is therefore exactly
 * what the backing held when that part was first asked for, or what was
 * written into the buffer since.
 *
 *   byteView()         a read-only view
 *   mutableByteView()  a writable view. The region counts as modified from
 *                      this moment on, whether or not anything is written
 *                      through it.
 *
 * Nothing reaches the backing until persist() is called. It writes every
 * modified region back, and nothing else. If it writes anything at all, it
 * first extends a backing that is shorter than the buffer to the buffer's
 * size, so an image saved by us always has the size we gave it. Unmodified
 * buffers leave their backing alone, however short.
 *
 *   readOnly  persist() does nothing. Modifications are still tracked, so a
 *             buffer whose flag is cleared again can persist them later.
 *
 * The buffer keeps track of what it has loaded and modified in pages of
 * pageSize bytes. That is the granularity of both: asking for a single byte
 * loads its page, and modifying a single byte writes its page back. Adjacent
 * pages are loaded, and written, in one go.
 *
 * Memory:
 *
 * The buffer reserves its full size up front, but the operating system only
 * provides memory for the pages that are touched, so a buffer far larger than
 * RAM costs what has been loaded. Views stay valid for as long as the buffer
 * holds its storage; nothing but dealloc() moves or frees it.
 *
 * Things this class does not do:
 *
 * - Write anything by itself. A buffer that goes away unpersisted loses its
 *   modifications, by design.
 * - Notice changes made to the backing by someone else. The backing is ours
 *   while the buffer uses it.
 * - Synchronize. Like any buffer, it is used by one thread at a time.
 */
class BackedBuffer {

public:

    // The unit of loading and of dirty tracking
    static constexpr isize pageSize = 4096;

    // If set, persist() leaves the backing alone
    bool readOnly = false;

private:

    enum class Page : u8 { Absent, Clean, Dirty };

    // Where the contents come from (nullptr if there is no backing)
    std::unique_ptr<Backing> backing;

    // The buffer
    u8 *mem = nullptr;
    isize bytes = 0;

    // The state of each page, and the number of dirty ones
    mutable std::vector<Page> pages;
    isize dirtyPages = 0;


    //
    // Initializing
    //

public:

    // Creates an empty buffer
    BackedBuffer() = default;

    // Creates a buffer of the given size on top of a backing
    BackedBuffer(isize size, std::unique_ptr<Backing> backing, bool readOnly = false);
    BackedBuffer(isize size, const fs::path &path, bool readOnly = false);
    BackedBuffer(isize size, Buffer<u8> &buffer, bool readOnly = false);

    ~BackedBuffer();

    /* Copying is refused, because two buffers would share one backing without
     * knowing about each other.
     */
    BackedBuffer(const BackedBuffer &) = delete;
    BackedBuffer &operator=(const BackedBuffer &) = delete;

    BackedBuffer(BackedBuffer &&other) noexcept;
    BackedBuffer &operator=(BackedBuffer &&other) noexcept;


    //
    // Allocating
    //

public:

    /* Gives the buffer a size and a backing.
     *
     * Whatever the buffer held before is dropped, unpersisted. A nullptr
     * backing is allowed: every byte then reads as zero, and persist() has
     * nowhere to write to.
     */
    void init(isize size, std::unique_ptr<Backing> backing, bool readOnly = false);

    // Shortcuts for the backings above
    void init(isize size, const fs::path &path, bool readOnly = false);
    void init(isize size, Buffer<u8> &buffer, bool readOnly = false);

    /* Loads everything and lets go of the backing.
     *
     * The buffer keeps its size and its contents, but has nowhere to persist
     * to from then on: it is a plain buffer in memory. Modifications that
     * were never persisted are kept, and so is their mark, but persist() has
     * nothing left to write them to.
     */
    void detach();

    // Returns the buffer to the state of a default constructed one
    void dealloc();


    //
    // Querying
    //

public:

    // Length of the buffer in bytes
    isize size() const { return bytes; }

    // Returns true if the buffer has size zero
    bool empty() const { return bytes == 0; }

    // Returns true if something has been modified since the last persist()
    bool dirty() const { return dirtyPages > 0; }

    // Returns true if the buffer has a backing to load from and persist to
    bool backed() const { return backing != nullptr; }


    //
    // Accessing
    //

public:

    /* Returns a view of bytes [offset, offset + len), loading them first.
     *
     * The region must lie within the buffer. The mutable variant marks the
     * region as modified.
     */
    ByteView byteView(isize offset, isize len) const;
    MutableByteView mutableByteView(isize offset, isize len);


    //
    // Persisting
    //

public:

    /* Writes all modified pages to the backing.
     *
     * Does nothing if readOnly is set, if there is no backing, or if nothing
     * has been modified. The pages become clean when all of them have been
     * written and the backing has been flushed. If anything fails on the way,
     * every modified page stays dirty, and the next persist() writes them all
     * again.
     */
    void persist();

private:

    // Loads the pages covering a region, where not loaded yet
    void load(isize offset, isize len) const;

    // Loads pages [first, last), all of which are absent
    void fetch(isize first, isize last) const;
};

}
