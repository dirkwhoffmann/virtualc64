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
#include "AnyFile.h"
#include "Checksum.h"
#include "IOUtils.h"
#include "CRTFile.h"
#include "D64File.h"
#include "Folder.h"
#include "G64File.h"
#include "P00File.h"
#include "PRGFile.h"
#include "RomFile.h"
#include "Script.h"
#include "Snapshot.h"
#include "T64File.h"
#include "TAPFile.h"

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
AnyFile::init(const u8 *buf, isize len)
{
    assert(buf);
    std::stringstream stream(std::ios::binary);
    stream.write((const char *)buf, len);
    init(stream);
}

void
AnyFile::init(const fs::path &path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open()) throw Error(VC64ERROR_FILE_NOT_FOUND, path);
    init(path, stream);
}

void
AnyFile::init(const fs::path &path, std::istream &stream)
{
    if (!isCompatiblePath(path)) throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    init(stream);
    this->path = path;
}

void
AnyFile::init(std::istream &stream)
{
    if (!isCompatibleStream(stream)) throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    readFromStream(stream);
}

void
AnyFile::init(FILE *file)
{
    assert(file);
    std::stringstream stream(std::ios::binary);
    int c; while ((c = fgetc(file)) != EOF) { stream.put((char)c); }
    init(stream);
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
    data.resize(count);
}

void
AnyFile::flash(u8 *buffer, isize offset) const
{
    if (data.ptr) {

        assert(buffer);
        std::memcpy(buffer + offset, data.ptr, data.size);
    }
}

isize
AnyFile::readFromStream(std::istream &stream)
{
    // Get stream size
    auto fsize = stream.tellg();
    stream.seekg(0, std::ios::end);
    fsize = stream.tellg() - fsize;
    stream.seekg(0, std::ios::beg);

    // Allocate memory
    data.init(isize(fsize));

    // Read from stream
    stream.read((char *)data.ptr, data.size);
    finalizeRead();

    return data.size;
}

isize
AnyFile::readFromFile(const fs::path &path)
{
    std::ifstream stream(path, std::ifstream::binary);

    if (!stream.is_open()) {
        throw Error(VC64ERROR_FILE_CANT_READ, path);
    }
    
    this->path = path;

    isize result = readFromStream(stream);
    assert(result == data.size);

    return result;
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
    if (util::isDirectory(path)) {
        throw Error(VC64ERROR_FILE_IS_DIRECTORY);
    }

    std::ofstream stream(path, std::ofstream::binary);

    if (!stream.is_open()) {
        throw Error(VC64ERROR_FILE_CANT_WRITE, path);
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
