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

//
// FileBacking
//

FileBacking::FileBacking(const fs::path &path) : path(path)
{
    if (!fs::exists(path))
        throw IOError(IOError::FILE_NOT_FOUND, path);

    in.open(path, std::ios::binary);

    if (!in.is_open())
        throw IOError(IOError::FILE_CANT_READ, path);

    bytes = isize(fs::file_size(path));
}

void
FileBacking::read(u8 *dst, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= bytes);

    in.clear();
    in.seekg(offset);
    in.read((char *)dst, len);

    if (!in || in.gcount() != len)
        throw IOError(IOError::FILE_CANT_READ, path);
}

void
FileBacking::write(const u8 *src, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= bytes);

    // Open the file for writing, without truncating it, when first needed
    if (!out.is_open()) {

        out.open(path, std::ios::binary | std::ios::in | std::ios::out);

        if (!out.is_open())
            throw IOError(IOError::FILE_CANT_WRITE, path);
    }

    out.seekp(offset);
    out.write((const char *)src, len);

    if (!out)
        throw IOError(IOError::FILE_CANT_WRITE, path);
}

void
FileBacking::extend(isize newSize)
{
    if (newSize <= bytes) return;

    // Anything written so far must be in the file before it changes size
    if (out.is_open()) out.flush();

    std::error_code ec;
    fs::resize_file(path, uintmax_t(newSize), ec);

    if (ec)
        throw IOError(IOError::FILE_CANT_WRITE, path);

    bytes = newSize;
}

void
FileBacking::flush()
{
    if (!out.is_open()) return;

    out.flush();
    auto good = bool(out);
    out.close();

    if (!good)
        throw IOError(IOError::FILE_CANT_WRITE, path);
}


//
// BufferBacking
//

void
BufferBacking::read(u8 *dst, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= buffer.size);
    if (len) std::memcpy(dst, buffer.ptr + offset, size_t(len));
}

void
BufferBacking::write(const u8 *src, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= buffer.size);
    if (len) std::memcpy(buffer.ptr + offset, src, size_t(len));
}

void
BufferBacking::extend(isize newSize)
{
    if (newSize > buffer.size) buffer.resize(newSize, 0);
}


//
// GzipBacking
//

GzipBacking::GzipBacking(const fs::path &path) : path(path)
{
    if (!fs::exists(path))
        throw IOError(IOError::FILE_NOT_FOUND, path);

    std::ifstream in(path, std::ios::binary);

    if (!in.is_open())
        throw IOError(IOError::FILE_CANT_READ, path);

    std::vector<u8> packed((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());

    if (in.bad())
        throw IOError(IOError::FILE_CANT_READ, path);

    if (packed.empty()) return;

    data.init(packed.data(), isize(packed.size()));

    try {
        data.gunzip();
    } catch (std::exception &err) {
        throw IOError(IOError::ZLIB_ERROR, err.what());
    }
}

void
GzipBacking::read(u8 *dst, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= data.size);
    if (len) std::memcpy(dst, data.ptr + offset, size_t(len));
}

void
GzipBacking::write(const u8 *src, isize offset, isize len)
{
    assert(offset >= 0 && len >= 0 && offset + len <= data.size);
    if (len) std::memcpy(data.ptr + offset, src, size_t(len));
}

void
GzipBacking::extend(isize newSize)
{
    if (newSize > data.size) data.resize(newSize, 0);
}

void
GzipBacking::flush()
{
    // Compress first, so that a failure here leaves the file alone
    std::vector<u8> packed;

    if (data.size) try {
        Compressible::gzip(data.ptr, data.size, packed);
    } catch (std::exception &err) {
        throw IOError(IOError::ZLIB_ERROR, err.what());
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);

    if (!out.is_open())
        throw IOError(IOError::FILE_CANT_WRITE, path);

    out.write((const char *)packed.data(), std::streamsize(packed.size()));
    out.flush();

    if (!out)
        throw IOError(IOError::FILE_CANT_WRITE, path);
}


//
// BackedBuffer
//

BackedBuffer::BackedBuffer(isize size, std::unique_ptr<Backing> backing, bool readOnly)
{
    init(size, std::move(backing), readOnly);
}

BackedBuffer::BackedBuffer(isize size, const fs::path &path, bool readOnly)
{
    init(size, path, readOnly);
}

BackedBuffer::BackedBuffer(isize size, Buffer<u8> &buffer, bool readOnly)
{
    init(size, buffer, readOnly);
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
        backing = std::move(other.backing);
        mem = std::exchange(other.mem, nullptr);
        bytes = std::exchange(other.bytes, 0);
        pages = std::move(other.pages);
        dirtyPages = std::exchange(other.dirtyPages, 0);
        other.pages.clear();
    }
    return *this;
}

void
BackedBuffer::init(isize size, std::unique_ptr<Backing> backing, bool readOnly)
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

    this->backing = std::move(backing);
    this->readOnly = readOnly;
    bytes = size;
    pages.assign(size_t((size + pageSize - 1) / pageSize), Page::Absent);
    dirtyPages = 0;
}

void
BackedBuffer::init(isize size, const fs::path &path, bool readOnly)
{
    init(size, std::make_unique<FileBacking>(path), readOnly);
}

void
BackedBuffer::init(isize size, Buffer<u8> &buffer, bool readOnly)
{
    init(size, std::make_unique<BufferBacking>(buffer), readOnly);
}

void
BackedBuffer::detach()
{
    // Everything has to be in memory before the backing goes away
    load(0, bytes);
    backing = nullptr;
}

void
BackedBuffer::dealloc()
{
    std::free(mem);

    readOnly = false;
    backing = nullptr;
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
    if (readOnly || !backing || !dirtyPages) return;

    // Give a short backing the buffer's size before writing into it
    if (backing->size() < bytes) backing->extend(bytes);

    auto count = isize(pages.size());

    for (isize p = 0; p < count;) {

        if (pages[p] != Page::Dirty) { p++; continue; }

        // Collect a run of dirty pages and write it in one go
        auto q = p;
        while (q < count && pages[q] == Page::Dirty) q++;

        auto begin = p * pageSize;
        auto end = std::min(q * pageSize, bytes);
        backing->write(mem + begin, begin, end - begin);
        p = q;
    }

    /* Only a completed flush makes the pages clean. A backing may hold on to
     * what it was given until then -- a compressed file does, and so does a
     * buffered stream -- so nothing is safe before flush() has returned.
     */
    backing->flush();

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

    // Bytes up to 'covered' come from the backing, the rest is zero
    auto available = backing ? backing->size() : 0;
    auto covered = std::clamp(available, begin, end);

    if (covered > begin) backing->read(mem + begin, begin, covered - begin);
    if (end > covered) std::memset(mem + covered, 0, size_t(end - covered));

    for (auto p = first; p < last; p++) pages[p] = Page::Clean;
}

}
