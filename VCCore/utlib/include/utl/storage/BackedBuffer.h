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
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace utl {

/* A byte buffer that loads its contents from a file as they are needed.
 *
 * The buffer has a size of its own. The file may be shorter -- the bytes it
 * does not cover read as zero -- or longer, in which case the excess is
 * neither read nor touched. A buffer may also have no file at all: it is then
 * a plain buffer in memory, zero throughout until something is written.
 *
 * Contents are handed out as views. Asking for a view loads whatever part of
 * the requested region has not been loaded before; the rest of the buffer
 * stays where it is, in the file. What a view shows is therefore exactly what
 * the file held when that part was first asked for, or what was written into
 * the buffer since.
 *
 *   byteView()         a read-only view
 *   mutableByteView()  a writable view. The region counts as modified from
 *                      this moment on, whether or not anything is written
 *                      through it.
 *
 * Nothing reaches the file until persist() is called. It writes every modified
 * region back, and nothing else. If it writes anything at all, it first grows
 * a file that is shorter than the buffer to the buffer's size, so an image
 * saved by us always has the size we gave it. Unmodified buffers leave their
 * file alone, however short.
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
 * - Handle a file that is not the bytes themselves. A compressed file is
 *   unpacked before it gets here (see retro::vault::BinaryImage).
 * - Notice changes made to the file by someone else. The file is ours while
 *   the buffer uses it.
 * - Synchronize. Like any buffer, it is used by one thread at a time.
 */
class BackedBuffer {

public:

    // The unit of loading and of dirty tracking
    static constexpr isize pageSize = 4096;

    // If set, persist() leaves the file alone
    bool readOnly = false;

private:

    enum class Page : u8 { Absent, Clean, Dirty };

    // The file the contents come from, empty if there is none
    fs::path file;

    /* The file is read as needed and opened for writing only when something
     * has to be written, so a file that cannot be written is fine for as long
     * as nobody tries. Reading happens while handing out a view, which is a
     * const operation, hence the mutable stream.
     */
    mutable std::ifstream in;
    std::fstream out;

    // Number of bytes the file holds
    isize fileBytes = 0;

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

    // Creates a buffer of the given size, in memory or on top of a file
    explicit BackedBuffer(isize size);
    BackedBuffer(isize size, const fs::path &path, bool readOnly = false);

    ~BackedBuffer();

    // Copying is refused, because two buffers would share one file
    BackedBuffer(const BackedBuffer &) = delete;
    BackedBuffer &operator=(const BackedBuffer &) = delete;

    BackedBuffer(BackedBuffer &&other) noexcept;
    BackedBuffer &operator=(BackedBuffer &&other) noexcept;


    //
    // Allocating
    //

public:

    /* Gives the buffer a size, and a file to load from.
     *
     * Whatever the buffer held before is dropped, unpersisted. Without a file
     * every byte reads as zero, and persist() has nowhere to write to; with
     * one, the file must exist and be readable.
     */
    void init(isize size);
    void init(isize size, const fs::path &path, bool readOnly = false);

    // Detaches the connected drive (if any)
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

    // Returns true if the buffer has a file to load from and persist to
    bool backed() const { return !file.empty(); }


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

    /* Writes all modified pages to the file.
     *
     * Does nothing if readOnly is set, if there is no file, or if nothing has
     * been modified. The pages become clean when all of them have been written
     * and the file has been closed again. If anything fails on the way, every
     * modified page stays dirty, and the next persist() writes them all again.
     */
    void persist();

private:

    // Loads the pages covering a region, where not loaded yet
    void load(isize offset, isize len) const;

    // Loads pages [first, last), all of which are absent
    void fetch(isize first, isize last) const;

    // Reads from, writes to, and grows the file
    void readFile(u8 *dst, isize offset, isize len) const;
    void writeFile(const u8 *src, isize offset, isize len);
    void growFile(isize newSize);
    void closeFile();
};

}
