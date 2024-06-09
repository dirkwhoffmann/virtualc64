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
#include "MediaFile.h"
#include "VirtualC64.h"
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

MediaFile *
MediaFile::make(const fs::path &path)
{
    return make(path, type(path));
}

MediaFile *
MediaFile::make(const fs::path &path, FileType type)
{
    switch (type) {

        case FILETYPE_SNAPSHOT:   return new Snapshot(path);
        case FILETYPE_SCRIPT:     return new Script(path);
        case FILETYPE_CRT:        return new CRTFile(path);
        case FILETYPE_T64:        return new T64File(path);
        case FILETYPE_PRG:        return new PRGFile(path);
        case FILETYPE_FOLDER:     return new Folder(path);
        case FILETYPE_P00:        return new P00File(path);
        case FILETYPE_D64:        return new D64File(path);
        case FILETYPE_G64:        return new G64File(path);
        case FILETYPE_TAP:        return new TAPFile(path);
        case FILETYPE_BASIC_ROM:  return new RomFile(path);
        case FILETYPE_CHAR_ROM:   return new RomFile(path);
        case FILETYPE_KERNAL_ROM: return new RomFile(path);
        case FILETYPE_VC1541_ROM: return new RomFile(path);

        default:
            return nullptr;
    }
}

MediaFile *
MediaFile::make(const u8 *buf, isize len, FileType type)
{
    switch (type) {
            
        case FILETYPE_SNAPSHOT:   return new Snapshot(buf, len);
        case FILETYPE_SCRIPT:     return new Script(buf, len);
        case FILETYPE_CRT:        return new CRTFile(buf, len);
        case FILETYPE_T64:        return new T64File(buf, len);
        case FILETYPE_PRG:        return new PRGFile(buf, len);
        case FILETYPE_P00:        return new P00File(buf, len);
        case FILETYPE_D64:        return new D64File(buf, len);
        case FILETYPE_G64:        return new G64File(buf, len);
        case FILETYPE_TAP:        return new TAPFile(buf, len);
        case FILETYPE_BASIC_ROM:  return new RomFile(buf, len);
        case FILETYPE_CHAR_ROM:   return new RomFile(buf, len);
        case FILETYPE_KERNAL_ROM: return new RomFile(buf, len);
        case FILETYPE_VC1541_ROM: return new RomFile(buf, len);
            
        default:
            return nullptr;
    }
}

MediaFile *
MediaFile::make(class FileSystem &fs, FileType type)
{
    switch (type) {

        case FILETYPE_T64:        return new T64File(fs);
        case FILETYPE_PRG:        return new PRGFile(fs);
        case FILETYPE_P00:        return new P00File(fs);
        case FILETYPE_D64:        return new D64File(fs);

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

        case FILETYPE_G64:        return new G64File(*disk);

        default:
            return nullptr;
    }
}

}
