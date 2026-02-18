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
#include "VirtualC64.h"
#include "Media/AnyFile.h"
#include "Media/CRTFile.h"
#include "Media/D64File.h"
#include "Media/G64File.h"
#include "Media/P00File.h"
#include "Media/PRGFile.h"
#include "Media/RomFile.h"
#include "Media/Script.h"
#include "Media/Snapshot.h"
#include "Media/T64File.h"
#include "Media/TAPFile.h"
#include "Workspace.h"

namespace vc64 {

FileType
AnyFile::type(const fs::path &path)
{
    if (Workspace::isCompatible(path))  return FileType::WORKSPACE;
    if (Snapshot::isCompatible(path))   return FileType::SNAPSHOT;
    if (Script::isCompatible(path))     return FileType::SCRIPT;
    if (CRTFile::isCompatible(path))    return FileType::CRT;
    if (T64File::isCompatible(path))    return FileType::T64;
    if (P00File::isCompatible(path))    return FileType::P00;
    if (PRGFile::isCompatible(path))    return FileType::PRG;
    if (D64File::isCompatible(path))    return FileType::D64;
    if (G64File::isCompatible(path))    return FileType::G64;
    if (TAPFile::isCompatible(path))    return FileType::TAP;

    if (RomFile::isCompatible(path)) {

        Buffer<u8> buffer(path);
        if (RomFile::isRomBuffer(RomType::BASIC, buffer)) return FileType::BASIC_ROM;
        if (RomFile::isRomBuffer(RomType::CHAR, buffer)) return FileType::CHAR_ROM;
        if (RomFile::isRomBuffer(RomType::KERNAL, buffer)) return FileType::KERNAL_ROM;
        if (RomFile::isRomBuffer(RomType::VC1541, buffer)) return FileType::VC1541_ROM;
    }

    return FileType::UNKNOWN;
}

AnyFile *
AnyFile::make(const fs::path &path)
{
    return make(path, type(path));
}

AnyFile *
AnyFile::make(const fs::path &path, FileType type)
{
    switch (type) {

        case FileType::WORKSPACE:  return new Workspace(path);
        case FileType::SNAPSHOT:   return new Snapshot(path);
        case FileType::SCRIPT:     return new Script(path);
        case FileType::CRT:        return new CRTFile(path);
        case FileType::T64:        return new T64File(path);
        case FileType::PRG:        return new PRGFile(path);
        case FileType::P00:        return new P00File(path);
        case FileType::D64:        return new D64File(path);
        case FileType::G64:        return new G64File(path);
        case FileType::TAP:        return new TAPFile(path);
        case FileType::BASIC_ROM:  return new RomFile(path);
        case FileType::CHAR_ROM:   return new RomFile(path);
        case FileType::KERNAL_ROM: return new RomFile(path);
        case FileType::VC1541_ROM: return new RomFile(path);

        default:
            return nullptr;
    }
}

AnyFile *
AnyFile::make(const u8 *buf, isize len, FileType type)
{
    switch (type) {

        case FileType::SNAPSHOT:   return new Snapshot(buf, len);
        case FileType::SCRIPT:     return new Script(buf, len);
        case FileType::CRT:        return new CRTFile(buf, len);
        case FileType::T64:        return new T64File(buf, len);
        case FileType::PRG:        return new PRGFile(buf, len);
        case FileType::P00:        return new P00File(buf, len);
        case FileType::D64:        return new D64File(buf, len);
        case FileType::G64:        return new G64File(buf, len);
        case FileType::TAP:        return new TAPFile(buf, len);
        case FileType::BASIC_ROM:  return new RomFile(buf, len);
        case FileType::CHAR_ROM:   return new RomFile(buf, len);
        case FileType::KERNAL_ROM: return new RomFile(buf, len);
        case FileType::VC1541_ROM: return new RomFile(buf, len);
            
        default:
            return nullptr;
    }
}

AnyFile *
AnyFile::make(DriveAPI &drive, FileType type)
{
    auto disk = drive.disk.get();
    if (!disk) return nullptr;

    switch (type) {

        case FileType::G64:        return new G64File(*disk);

        default:
            return nullptr;
    }
}

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
