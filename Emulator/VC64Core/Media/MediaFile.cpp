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

#include "VirtualC64Config.h"
#include "MediaFile.h"
#include "VirtualC64.h"
#include "StringUtils.h"
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

FileType
MediaFile::type(const fs::path &path)
{
    Buffer<u8> buffer(path);
    if (buffer.empty()) return FileType::UNKNOWN;

    if (Snapshot::isCompatible(path) &&
        Snapshot::isCompatible(buffer)) return FileType::SNAPSHOT;

    if (Script::isCompatible(path) &&
        Script::isCompatible(buffer)) return FileType::SCRIPT;

    if (CRTFile::isCompatible(path) &&
        CRTFile::isCompatible(buffer)) return FileType::CRT;

    if (T64File::isCompatible(path) &&
        T64File::isCompatible(buffer)) return FileType::T64;

    if (P00File::isCompatible(path) &&
        P00File::isCompatible(buffer)) return FileType::P00;

    if (PRGFile::isCompatible(path) &&
        PRGFile::isCompatible(buffer)) return FileType::PRG;

    if (D64File::isCompatible(path) &&
        D64File::isCompatible(buffer)) return FileType::D64;

    if (G64File::isCompatible(path) &&
        G64File::isCompatible(buffer)) return FileType::G64;

    if (TAPFile::isCompatible(path) &&
        TAPFile::isCompatible(buffer)) return FileType::TAP;

    if (RomFile::isCompatible(path)) {
        if (RomFile::isRomBuffer(RomType::BASIC, buffer)) return FileType::BASIC_ROM;
        if (RomFile::isRomBuffer(RomType::CHAR, buffer)) return FileType::CHAR_ROM;
        if (RomFile::isRomBuffer(RomType::KERNAL, buffer)) return FileType::KERNAL_ROM;
        if (RomFile::isRomBuffer(RomType::VC1541, buffer)) return FileType::VC1541_ROM;
    }

    if (Folder::isCompatible(path)) return FileType::FOLDER;

    return FileType::UNKNOWN;
}

MediaFile *
MediaFile::make(const fs::path &path)
{
    return make(path, type(path));
}

MediaFile *
MediaFile::make(const fs::path &path, FileType type)
{
    switch (type) {

        case FileType::SNAPSHOT:   return new Snapshot(path);
        case FileType::SCRIPT:     return new Script(path);
        case FileType::CRT:        return new CRTFile(path);
        case FileType::T64:        return new T64File(path);
        case FileType::PRG:        return new PRGFile(path);
        case FileType::FOLDER:     return new Folder(path);
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

MediaFile *
MediaFile::make(const u8 *buf, isize len, FileType type)
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

MediaFile *
MediaFile::make(const class FileSystem &fs, FileType type)
{
    switch (type) {

        case FileType::T64:        return new T64File(fs);
        case FileType::PRG:        return new PRGFile(fs);
        case FileType::P00:        return new P00File(fs);
        case FileType::D64:        return new D64File(fs);

        default:
            return nullptr;
    }
}

MediaFile *
MediaFile::make(DriveAPI &drive, FileType type)
{
    auto disk = drive.disk.get();
    if (!disk) return nullptr;

    switch (type) {

        case FileType::G64:        return new G64File(*disk);

        default:
            return nullptr;
    }
}

string
MediaFile::getSizeAsString() const
{
    return util::byteCountAsString(getSize());
}

}
