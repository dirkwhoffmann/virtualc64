// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/storage/BackedBuffer.h"
#include "utl/io/IOError.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

namespace utl {

BackedBuffer::BackedBuffer(isize size)
{
    init(size);
}

BackedBuffer::BackedBuffer(isize size, const fs::path &path, bool readOnly)
{
    init(size, path, readOnly);
}

BackedBuffer::~BackedBuffer()
{
    dealloc();
}

BackedBuffer::BackedBuffer(BackedBuffer &&other) noexcept
{
    *this = std::move(other);
}

BackedBuffer &
BackedBuffer::operator=(BackedBuffer &&other) noexcept
{
    if (this != &other) {

        dealloc();

        readOnly = std::exchange(other.readOnly, false);
        file = std::move(other.file);
        in = std::move(other.in);
        out = std::move(other.out);
        fileBytes = std::exchange(other.fileBytes, 0);
        mem = std::exchange(other.mem, nullptr);
        bytes = std::exchange(other.bytes, 0);
        pages = std::move(other.pages);
        dirtyPages = std::exchange(other.dirtyPages, 0);
        other.file.clear();
        other.pages.clear();
    }
    return *this;
}

void
BackedBuffer::init(isize size)
{
    assert(size >= 0);

    dealloc();

    /* calloc rather than new[]: for a region of this size the allocator asks
     * the operating system for fresh pages, which are zero already and are not
     * backed by memory until they are touched.
     */
    if (size) {

        mem = (u8 *)std::calloc(size_t(size), 1);
        if (!mem) throw std::bad_alloc();
    }

    bytes = size;
    pages.assign(size_t((size + pageSize - 1) / pageSize), Page::Absent);
    dirtyPages = 0;
}

void
BackedBuffer::init(isize size, const fs::path &path, bool readOnly)
{
    if (!fs::exists(path))
        throw IOError(IOError::FILE_NOT_FOUND, path);

    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open())
        throw IOError(IOError::FILE_CANT_READ, path);

    // Nothing is allowed to fail after the buffer has been given its memory
    init(size);

    file = path;
    in = std::move(stream);
    fileBytes = isize(fs::file_size(path));
    this->readOnly = readOnly;
}

void
BackedBuffer::detach()
{
    // Everything has to be in memory before the file goes away
    load(0, bytes);

    in.close();
    out.close();
    file.clear();
    fileBytes = 0;
}

void
BackedBuffer::dealloc()
{
    std::free(mem);

    in.close();
    out.close();
    file.clear();
    fileBytes = 0;
    readOnly = false;
    mem = nullptr;
    bytes = 0;
    pages.clear();
    dirtyPages = 0;
}

ByteView
BackedBuffer::byteView(isize offset, isize len) const
{
    assert(offset >= 0 && len >= 0 && offset + len <= bytes);

    load(offset, len);
    return ByteView(mem + offset, len);
}

MutableByteView
BackedBuffer::mutableByteView(isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= bytes);

    load(offset, len);

    if (len) {

        for (auto p = offset / pageSize; p <= (offset + len - 1) / pageSize; p++) {

            if (pages[p] != Page::Dirty) { pages[p] = Page::Dirty; dirtyPages++; }
        }
    }
    return MutableByteView(mem + offset, len);
}

void
BackedBuffer::persist()
{
    if (readOnly || !backed() || !dirtyPages) return;

    // Give a short file the buffer's size before writing into it
    growFile(bytes);

    auto count = isize(pages.size());

    for (isize p = 0; p < count;) {

        if (pages[p] != Page::Dirty) { p++; continue; }

        // Collect a run of dirty pages and write it in one go
        auto q = p;
        while (q < count && pages[q] == Page::Dirty) q++;

        auto begin = p * pageSize;
        auto end = std::min(q * pageSize, bytes);
        writeFile(mem + begin, begin, end - begin);
        p = q;
    }

    /* Only a completed close makes the pages clean: a buffered stream holds on
     * to what it was given, so nothing is safe before the stream is gone.
     */
    closeFile();

    for (auto &page : pages) if (page == Page::Dirty) page = Page::Clean;
    dirtyPages = 0;
}

void
BackedBuffer::load(isize offset, isize len) const
{
    if (!len) return;

    auto last = (offset + len - 1) / pageSize;

    for (auto p = offset / pageSize; p <= last;) {

        if (pages[p] != Page::Absent) { p++; continue; }

        // Collect a run of absent pages and load it in one go
        auto q = p;
        while (q <= last && pages[q] == Page::Absent) q++;

        fetch(p, q);
        p = q;
    }
}

void
BackedBuffer::fetch(isize first, isize last) const
{
    auto begin = first * pageSize;
    auto end = std::min(last * pageSize, bytes);

    // Bytes up to 'covered' come from the file, the rest is zero
    auto covered = std::clamp(fileBytes, begin, end);

    if (covered > begin) readFile(mem + begin, begin, covered - begin);
    if (end > covered) std::memset(mem + covered, 0, size_t(end - covered));

    for (auto p = first; p < last; p++) pages[p] = Page::Clean;
}

void
BackedBuffer::readFile(u8 *dst, isize offset, isize len) const
{
    assert(offset >= 0 && len >= 0 && offset + len <= fileBytes);

    in.clear();
    in.seekg(offset);
    in.read((char *)dst, len);

    if (!in || in.gcount() != len)
        throw IOError(IOError::FILE_CANT_READ, file);
}

void
BackedBuffer::writeFile(const u8 *src, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= fileBytes);

    // Open the file for writing, without truncating it, when first needed
    if (!out.is_open()) {

        out.open(file, std::ios::binary | std::ios::in | std::ios::out);

        if (!out.is_open())
            throw IOError(IOError::FILE_CANT_WRITE, file);
    }

    out.seekp(offset);
    out.write((const char *)src, len);

    if (!out)
        throw IOError(IOError::FILE_CANT_WRITE, file);
}

void
BackedBuffer::growFile(isize newSize)
{
    if (newSize <= fileBytes) return;

    // Anything written so far must be in the file before it changes size
    if (out.is_open()) out.flush();

    std::error_code ec;
    fs::resize_file(file, uintmax_t(newSize), ec);

    if (ec)
        throw IOError(IOError::FILE_CANT_WRITE, file);

    fileBytes = newSize;
}

void
BackedBuffer::closeFile()
{
    if (!out.is_open()) return;

    out.flush();
    auto good = bool(out);
    out.close();

    if (!good)
        throw IOError(IOError::FILE_CANT_WRITE, file);
}

}
