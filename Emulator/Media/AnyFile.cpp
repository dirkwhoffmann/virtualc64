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

AnyFile::AnyFile(isize capacity)
{
    data = new u8[capacity]();
    size = capacity;
}

AnyFile::~AnyFile()
{
    if (data) delete[] data;
}

PETName<16>
AnyFile::getName() const
{
    auto idx = path.rfind('/');
    isize start = idx != string::npos ? idx + 1 : 0;
    
    idx = path.rfind('.');
    isize len = idx != string::npos ? idx - start : string::npos;
    
    return PETName<16>(path.substr(start, len));
}

FileType
AnyFile::type(const string &path)
{
    std::ifstream stream(path);
    if (!stream.is_open()) return FILETYPE_UNKNOWN;
    
    if (Snapshot::isCompatiblePath(path) &&
        Snapshot::isCompatibleStream(stream))return FILETYPE_SNAPSHOT;

    if (Script::isCompatiblePath(path) &&
        Script::isCompatibleStream(stream))return FILETYPE_SCRIPT;

    if (CRTFile::isCompatiblePath(path) &&
        CRTFile::isCompatibleStream(stream))return FILETYPE_CRT;

    if (T64File::isCompatiblePath(path) &&
        T64File::isCompatibleStream(stream)) return FILETYPE_T64;

    if (P00File::isCompatiblePath(path) &&
        P00File::isCompatibleStream(stream)) return FILETYPE_P00;

    if (PRGFile::isCompatiblePath(path) &&
        PRGFile::isCompatibleStream(stream)) return FILETYPE_PRG;

    if (D64File::isCompatiblePath(path) &&
        D64File::isCompatibleStream(stream)) return FILETYPE_D64;

    if (G64File::isCompatiblePath(path) &&
        G64File::isCompatibleStream(stream)) return FILETYPE_G64;

    if (TAPFile::isCompatiblePath(path) &&
        TAPFile::isCompatibleStream(stream)) return FILETYPE_TAP;

    if (RomFile::isCompatiblePath(path)) {
        if (RomFile::isRomStream(ROM_TYPE_BASIC, stream)) return FILETYPE_BASIC_ROM;
        if (RomFile::isRomStream(ROM_TYPE_CHAR, stream)) return FILETYPE_CHAR_ROM;
        if (RomFile::isRomStream(ROM_TYPE_KERNAL, stream)) return FILETYPE_KERNAL_ROM;
        if (RomFile::isRomStream(ROM_TYPE_VC1541, stream)) return FILETYPE_VC1541_ROM;
    }
    
    if (Folder::isFolder(path.c_str())) return FILETYPE_FOLDER;

    return FILETYPE_UNKNOWN;
}

u64
AnyFile::fnv() const
{
    return data ? util::fnv_1a_64(data, size) : 0;
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
    memcpy(buffer + offset, data, size);
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
    assert(data == nullptr);
    data = new u8[fsize]();
    size = fsize;

    // Read data
    stream.read((char *)data, size);
    
    // Fix known inconsistencies
    repair();
    
    return size;
}

isize
AnyFile::readFromFile(const string  &path)
{
    std::ifstream stream(path);

    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_READ);
    }
    
    isize result = readFromStream(stream);
    assert(result == size);
    
    this->path = string(path);
    return result;
}

isize
AnyFile::readFromBuffer(const u8 *buf, isize len)
{
    assert(buf);

    std::istringstream stream(string((const char *)buf, len));
    
    isize result = readFromStream(stream);
    assert(result == size);
    
    return result;
}

isize
AnyFile::writeToStream(std::ostream &stream)
{
    stream.write((char *)data, size);
    return size;
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
    memcpy(buf, data, size);
}
