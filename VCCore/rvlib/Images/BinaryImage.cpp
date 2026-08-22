// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/BinaryImage.h"
#include "utl/io.h"
#include "utl/support.h"
#include <fstream>

namespace retro::vault {

using utl::ByteView;
using utl::MutableByteView;
using utl::IOError;

void
BinaryImage::init(isize len)
{
    data.init(len);
}

void
BinaryImage::init(const utl::Buffer<u8> &buffer)
{
    init(buffer.ptr, buffer.size);
}

/*
void
BinaryImage::init(const string &str)
{
    init((const u8 *)str.c_str(), (isize)str.length());
}
*/

void
BinaryImage::init(const fs::path &p)
{
    if (!validateURL(p))
        throw utl::IOError(utl::IOError::FILE_TYPE_MISMATCH, p);

    std::fstream stream(p, std::ios::binary | std::ios::in);

    if (!stream)
        throw utl::IOError(utl::IOError::FILE_NOT_FOUND, p);

    // Read file into a vector
    std::vector<u8> buffer((std::istreambuf_iterator<char>(stream)),
                           std::istreambuf_iterator<char>());

    if (buffer.empty())
        throw utl::IOError(utl::IOError::FILE_CANT_READ, p);

    this->path = p;

    // Initialize image with the vector contents
    init(buffer.data(), isize(buffer.size()));
}

void
BinaryImage::init(const u8 *buf, isize len)
{
    assert(buf);

    // Allocate memory
    data.alloc(len);

    // Copy data
    std::memcpy(data.ptr, buf, data.size);
    didInitialize();
}

void
BinaryImage::copy(u8 *buf, isize offset, isize len) const
{
    assert(buf);
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    std::memcpy(buf + offset, data.ptr, len);
}

utl::ByteView
BinaryImage::byteView(isize offset) const
{
    return byteView(offset, data.size - offset);
}

utl::ByteView
BinaryImage::byteView(isize offset, isize len) const
{
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    return utl::ByteView(data.ptr + offset, len);
}

utl::MutableByteView
BinaryImage::byteView(isize offset)
{
    return byteView(offset, data.size - offset);
}

utl::MutableByteView
BinaryImage::byteView(isize offset, isize len)
{
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    return utl::MutableByteView(data.ptr + offset, len);
}

void
BinaryImage::copy(u8 *buf, isize offset) const
{
    copy (buf, offset, data.size);
}

void
BinaryImage::save()
{
    /* getSize(), not size(). While this lived on AnyImage there was no size()
     * in scope at all, so the call resolved to the inherited Loggable::size()
     * -- the number of registered log channels -- and every save() wrote that
     * many bytes. The buffer is right here now, so ask it.
     */
    save(utl::Range<isize>{0, getSize()});
}

void
BinaryImage::save(const utl::Range<isize> range)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) throw utl::IOError(utl::IOError::FILE_CANT_WRITE, path);

    printf("Saving range %ld - %ld...\n", range.lower, range.upper - 1);

    // Move to the correct position
    file.seekp(range.lower, std::ios::beg);

    // Write the data to the stream
    file.write((char *)(data.ptr + range.lower), range.size());

    // Update the file on disk
    file.flush();
}

void
BinaryImage::save(const std::vector<utl::Range<isize>> ranges)
{
    for (auto &range: ranges) save(range);
}

void
BinaryImage::saveAs(const fs::path &newPath)
{
    path = newPath;
    save();
}

isize
BinaryImage::writeToStream(std::ostream &stream, isize offset, isize len) const
{
    assert(offset >= 0 && len >= 0 && offset + len <= data.size);

    stream.write((char *)data.ptr + offset, len);

    return data.size;
}

isize
BinaryImage::writeToFile(const fs::path &p, isize offset, isize len) const
{
    if (utl::isDirectory(p)) {
        throw utl::IOError(utl::IOError::FILE_IS_DIRECTORY);
    }

    std::ofstream stream(p, std::ofstream::binary);

    if (!stream.is_open()) {
        throw utl::IOError(utl::IOError::FILE_CANT_WRITE, p);
    }

    isize result = writeToStream(stream, offset, len);
    assert(result == data.size);

    return result;
}

isize
BinaryImage::writeToStream(std::ostream &stream) const
{
    return writeToStream(stream, 0, data.size);
}

isize
BinaryImage::writeToFile(const fs::path &p) const
{
    return writeToFile(p, 0, data.size);
}

}
