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
#include "Devices/LinearDevice.h"
#include "utl/abilities/Compressible.h"
#include "utl/io.h"
#include "utl/storage/Buffer.h"
#include <fstream>
#include <sstream>

namespace retro::vault {

using utl::IOError;
using utl::Compressor;

void
BinaryImage::init(isize len)
{
    data.init(len);
}

void
BinaryImage::init(const fs::path &p, Compressor compressor)
{
    if (!validateURL(p))
        throw IOError(IOError::FILE_TYPE_MISMATCH, p);

    if (compressor != Compressor::NONE) {

        if (!fs::exists(p))
            throw IOError(IOError::FILE_NOT_FOUND, p);

        utl::Buffer<u8> bytes;
        bytes.init(p);

        if (bytes.empty())
            throw IOError(IOError::FILE_CANT_READ, p);

        bytes.uncompress(compressor);

        init(bytes.ptr, bytes.size);
        path.clear();
        return;
    }

    if (utl::getSizeOfFile(p) <= 0)
        throw IOError(IOError::FILE_CANT_READ, p);

    this->path = p;

    // Put the image on top of the file. Nothing is loaded yet.
    data.init(utl::getSizeOfFile(p), p);
    didInitialize();
}

void
BinaryImage::init(const LinearDevice &device)
{
    data.init(device.size());

    // Pull in the contents
    auto bytes = data.mutableByteView(0, data.size());
    device.read(bytes.data(), 0, bytes.size());
    didInitialize();
}

void
BinaryImage::init(const u8 *buf, isize len, Compressor compressor)
{
    assert(buf);

    if (compressor != Compressor::NONE) {

        utl::Buffer<u8> bytes(buf, len);
        bytes.uncompress(compressor);
        init(bytes.ptr, bytes.size);
        return;
    }

    data.init(len);

    if (len) std::memcpy(data.mutableByteView(0, len).data(), buf, size_t(len));
    didInitialize();
}

void
BinaryImage::resize(isize len)
{
    data.resize(len);
}

utl::ByteView
BinaryImage::byteView(isize offset, isize len) const
{
    return data.byteView(offset, len);
}

utl::MutableByteView
BinaryImage::mutableByteView(isize offset, isize len)
{
    return data.mutableByteView(offset, len);
}

void
BinaryImage::detach()
{
    data.detach();
    path.clear();
}

void
BinaryImage::copy(u8 *buf, isize offset) const
{
    copy(buf, offset, getSize() - offset);
}

void
BinaryImage::copy(u8 *buf, isize offset, isize len) const
{
    assert(buf);
    std::memcpy(buf, byteView(offset, len).data(), len);
}

void
BinaryImage::save()
{
    data.persist();
}

void
BinaryImage::saveAs(const fs::path &newPath)
{
    // Saving as the file the image already lives in is saving
    std::error_code ec;
    if (data.backed() && fs::equivalent(newPath, path, ec)) { save(); return; }

    // Write the entire image first, so that a failure changes nothing
    copy(newPath);

    // Continue on top of the new file
    path = newPath;
    data.init(getSize(), newPath);
}

isize
BinaryImage::copy(std::ostream &stream, isize offset, isize len, Compressor compressor) const
{
    if (compressor != Compressor::NONE) {

        utl::Buffer<u8> bytes((const u8 *)byteView(offset, len).data(), len);
        bytes.compress(compressor);
        stream.write((const char *)bytes.ptr, bytes.size);

        return bytes.size;
    }

    stream.write((const char *)byteView(offset, len).data(), len);

    return len;
}

isize
BinaryImage::copy(const fs::path &p, isize offset, isize len, Compressor compressor) const
{
    std::error_code ec;

    if (utl::isDirectory(p))
        throw IOError(IOError::FILE_IS_DIRECTORY);

    if (data.backed() && fs::equivalent(p, path, ec))
        throw IOError(IOError::FILE_IN_USE, p);

    std::stringstream compressed;
    if (compressor != Compressor::NONE) copy(compressed, offset, len, compressor);

    std::ofstream stream(p, std::ofstream::binary);

    if (!stream.is_open())
        throw IOError(IOError::FILE_CANT_WRITE, p);

    if (compressor != Compressor::NONE) {

        const auto bytes = compressed.str();
        stream.write(bytes.data(), std::streamsize(bytes.size()));

        return isize(bytes.size());
    }

    isize result = copy(stream, offset, len);
    assert(result == len);

    return result;
}

isize
BinaryImage::copy(std::ostream &stream, Compressor compressor) const
{
    return copy(stream, 0, getSize(), compressor);
}

isize
BinaryImage::copy(const fs::path &p, Compressor compressor) const
{
    return copy(p, 0, getSize(), compressor);
}

}
