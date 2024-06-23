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

AnyFile::AnyFile(isize capacity)
{
    init(capacity);
}

AnyFile::~AnyFile()
{
    if (data) delete[] data;
}

void
AnyFile::init(isize capacity)
{
    data = new u8[capacity]();
    size = capacity;
}

void
AnyFile::init(const fs::path &path)
{
    std::ifstream stream(path);
    if (!stream.is_open()) throw Error(ERROR_FILE_NOT_FOUND, path);
    init(path, stream);
}

void
AnyFile::init(const fs::path &path, std::istream &stream)
{
    if (!isCompatiblePath(path)) throw Error(ERROR_FILE_TYPE_MISMATCH);
    init(stream);
    this->path = path;
}

void
AnyFile::init(std::istream &stream)
{
    if (!isCompatibleStream(stream)) throw Error(ERROR_FILE_TYPE_MISMATCH);
    readFromStream(stream);
}

void
AnyFile::init(const u8 *buf, isize len)
{
    assert(buf);
    std::stringstream stream;
    stream.write((const char *)buf, len);
    init(stream);
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

u64
AnyFile::fnv() const
{
    return data ? util::fnv64(data, size) : 0;
}

void
AnyFile::strip(isize count)
{
    assert(data != nullptr);
    assert(count < size);

    isize newSize = size - count;
    u8 *newData = new u8[newSize];
    
    memcpy(newData, data + count, newSize);
    delete [] data;
    
    size = newSize;
    data = newData;
}

void
AnyFile::flash(u8 *buffer, isize offset) const
{
    assert(buffer);
    std::memcpy(buffer + offset, data, size);
}

void
AnyFile::readFromStream(std::istream &stream)
{
    // Get stream size
    auto fsize = stream.tellg();
    stream.seekg(0, std::ios::end);
    fsize = stream.tellg() - fsize;
    stream.seekg(0, std::ios::beg);

    // Allocate memory
    data = new u8[isize(fsize)]();
    size = isize(fsize);

    // Read data
    stream.read((char *)data, size);
    finalizeRead();
}

void
AnyFile::readFromFile(const fs::path &path)
{
    std::ifstream stream(path);

    if (!stream.is_open()) {
        throw Error(ERROR_FILE_CANT_READ);
    }
    
    this->path = path;

    readFromStream(stream);
}

void
AnyFile::readFromBuffer(const u8 *buf, isize len)
{
    assert(buf);

    // Allocate memory
    size = len;
    assert(data == nullptr);
    data = new u8[size];

    // Copy data
    std::memcpy(data, buf, size);
    finalizeRead();
}

void
AnyFile::writeToStream(std::ostream &stream)
{
    stream.write((char *)data, size);
}

void
AnyFile::writeToFile(const fs::path &path)
{
    std::ofstream stream(path);

    if (!stream.is_open()) {
        throw Error(ERROR_FILE_CANT_WRITE);
    }
    
    writeToStream(stream);
}

void
AnyFile::writeToBuffer(u8 *buf)
{
    assert(buf);
    std::memcpy(buf, data, size);
}

}
