// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Media/AnyFile.h"
#include "Media/CRTFile.h"
#include "Media/D64File.h"
#include "Media/Folder.h"
#include "Media/G64File.h"
#include "Media/P00File.h"
#include "Media/PRGFile.h"
#include "Media/RomFile.h"
#include "Media/Script.h"
#include "Media/Snapshot.h"
#include "Media/T64File.h"
#include "Media/TAPFile.h"

namespace vc64 {

void
AnyFile::init(isize capacity)
{
    data.init(capacity);
}

void
AnyFile::init(const Buffer<u8> &buffer)
{
    init(buffer.ptr, buffer.size);
}

void 
AnyFile::init(const string &str)
{
    init((const u8 *)str.c_str(), (isize)str.length());
}

void
AnyFile::init(const fs::path &path)
{
    if (!isCompatiblePath(path)) throw IOError(IOError::FILE_TYPE_MISMATCH, path);
    
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open()) throw IOError(IOError::FILE_NOT_FOUND, path);

    std::ostringstream sstr(std::ios::binary);
    sstr << stream.rdbuf();
    init(sstr.str());
    this->path = path;
}

void
AnyFile::init(const u8 *buf, isize len)
{
    assert(buf);
    if (!isCompatibleBuffer(buf, len)) throw IOError(IOError::FILE_TYPE_MISMATCH);
    readFromBuffer(buf, len);
}

string
AnyFile::name() const
{
    return getName().str();
}

PETName<16>
AnyFile::getName() const
{
    auto s = path.string();

    auto idx = s.rfind('/');
    auto start = idx != string::npos ? idx + 1 : 0;

    idx = s.rfind('.');
    auto len = idx != string::npos ? idx - start : string::npos;

    return PETName<16>(s.substr(start, len));
}

void
AnyFile::strip(isize count)
{
    data.strip(count);
}

void
AnyFile::flash(u8 *buf, isize offset, isize len) const
{
    assert(buf);
    std::memcpy(buf + offset, data.ptr, len);
}

void
AnyFile::flash(u8 *buf, isize offset) const
{
    flash (buf, offset, data.size);
}

bool 
AnyFile::isCompatibleBuffer(const Buffer<u8> &buffer) const
{
    return isCompatibleBuffer(buffer.ptr, buffer.size);
}

isize
AnyFile::readFromBuffer(const u8 *buf, isize len)
{
    assert(buf);

    // Allocate memory
    data.alloc(len);

    // Copy data
    std::memcpy(data.ptr, buf, data.size);
    finalizeRead();

    return data.size;
}

isize
AnyFile::readFromBuffer(const Buffer<u8> &buffer)
{
    return readFromBuffer(buffer.ptr, buffer.size);
}

isize
AnyFile::writeToStream(std::ostream &stream, isize offset, isize len)
{
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    stream.write((char *)data.ptr + offset, len);
    finalizeWrite();

    return data.size;
}

isize
AnyFile::writeToFile(const std::filesystem::path &path, isize offset, isize len)
{
    if (utl::isDirectory(path)) {
        throw IOError(IOError::FILE_IS_DIRECTORY);
    }

    std::ofstream stream(path, std::ofstream::binary);

    if (!stream.is_open()) {
        throw IOError(IOError::FILE_CANT_WRITE, path);
    }

    isize result = writeToStream(stream, offset, len);
    assert(result == data.size);

    return result;
}

isize
AnyFile::writeToBuffer(u8 *buf, isize offset, isize len)
{
    assert(buf);
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    std::memcpy(buf, (char *)data.ptr + offset, len);
    finalizeWrite();

    return data.size;
}

isize
AnyFile::writeToBuffer(Buffer<u8> &buffer, isize offset, isize len)
{
    buffer.alloc(len);
    return writeToBuffer(buffer.ptr, offset, len);
}

isize
AnyFile::writeToStream(std::ostream &stream)
{
    return writeToStream(stream, 0, data.size);
}

isize
AnyFile::writeToFile(const std::filesystem::path &path)
{
    return writeToFile(path, 0, data.size);
}

isize
AnyFile::writeToBuffer(u8 *buf)
{
    return writeToBuffer(buf, 0, data.size);
}

isize
AnyFile::writeToBuffer(Buffer<u8> &buffer)
{
    return writeToBuffer(buffer, 0, data.size);
}

}
