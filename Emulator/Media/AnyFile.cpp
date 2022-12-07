// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
    data = new u8[capacity]();
    size = capacity;
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
AnyFile::init(const string &path)
{
    std::ifstream stream(path);
    if (!stream.is_open()) throw VC64Error(ERROR_FILE_NOT_FOUND, path);
    init(path, stream);
}

void
AnyFile::init(const string &path, std::istream &stream)
{
    if (!isCompatiblePath(path)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
    init(stream);
    this->path = path;
}

void
AnyFile::init(std::istream &stream)
{
    if (!isCompatibleStream(stream)) throw VC64Error(ERROR_FILE_TYPE_MISMATCH);
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
    std::stringstream stream;
    int c; while ((c = fgetc(file)) != EOF) { stream.put((char)c); }
    init(stream);
}

PETName<16>
AnyFile::getName() const
{
    auto idx = path.rfind('/');
    auto start = idx != string::npos ? idx + 1 : 0;

    idx = path.rfind('.');
    auto len = idx != string::npos ? idx - start : string::npos;

    return PETName<16>(path.substr(start, len));
}

FileType
AnyFile::type(const string &path)
{
    std::ifstream stream(path);
    if (!stream.is_open()) return FILETYPE_UNKNOWN;
    
    if (Snapshot::isCompatible(path) &&
        Snapshot::isCompatible(stream))return FILETYPE_SNAPSHOT;

    if (Script::isCompatible(path) &&
        Script::isCompatible(stream))return FILETYPE_SCRIPT;

    if (CRTFile::isCompatible(path) &&
        CRTFile::isCompatible(stream))return FILETYPE_CRT;

    if (T64File::isCompatible(path) &&
        T64File::isCompatible(stream)) return FILETYPE_T64;

    if (P00File::isCompatible(path) &&
        P00File::isCompatible(stream)) return FILETYPE_P00;

    if (PRGFile::isCompatible(path) &&
        PRGFile::isCompatible(stream)) return FILETYPE_PRG;

    if (D64File::isCompatible(path) &&
        D64File::isCompatible(stream)) return FILETYPE_D64;

    if (G64File::isCompatible(path) &&
        G64File::isCompatible(stream)) return FILETYPE_G64;

    if (TAPFile::isCompatible(path) &&
        TAPFile::isCompatible(stream)) return FILETYPE_TAP;

    if (RomFile::isCompatible(path)) {
        if (RomFile::isRomStream(ROM_TYPE_BASIC, stream)) return FILETYPE_BASIC_ROM;
        if (RomFile::isRomStream(ROM_TYPE_CHAR, stream)) return FILETYPE_CHAR_ROM;
        if (RomFile::isRomStream(ROM_TYPE_KERNAL, stream)) return FILETYPE_KERNAL_ROM;
        if (RomFile::isRomStream(ROM_TYPE_VC1541, stream)) return FILETYPE_VC1541_ROM;
    }
    
    if (Folder::isCompatible(path)) return FILETYPE_FOLDER;

    return FILETYPE_UNKNOWN;
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
AnyFile::readFromFile(const string  &path)
{
    std::ifstream stream(path);

    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_READ);
    }
    
    this->path = string(path);

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
AnyFile::writeToFile(const string &path)
{
    std::ofstream stream(path);

    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_WRITE);
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
