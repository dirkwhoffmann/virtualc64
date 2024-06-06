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
MediaFile::type(const string &path)
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

}
