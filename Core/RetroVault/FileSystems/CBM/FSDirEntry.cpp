// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FSDirEntry.h"
#include "utl/primitives.h"
#include "utl/support/Strings.h"

namespace retro::vault::cbm {

FSDirEntry::FSDirEntry()
{
    memset(&nextDirTrack, 0, sizeof(*this));
}

FSDirEntry::FSDirEntry(std::span<const u8> data)
{
    assert(sizeof(*this) == data.size());
    memcpy(&nextDirTrack, data.data(), data.size());
}

void
FSDirEntry::init(PETName<16> name, TSLink ref, isize numBlocks)
{
    fileType        = 0x82;  // PRG
    firstDataTrack  = (u8)ref.t;
    firstDataSector = (u8)ref.s;
    fileSizeLo      = LO_BYTE(numBlocks);
    fileSizeHi      = HI_BYTE(numBlocks);

    name.write(fileName);
}

void
FSDirEntry::init(const string &name, TSLink ref, isize numBlocks)
{
    auto petName = PETName<16>(name);
    init(petName, ref, numBlocks);
}

bool
FSDirEntry::empty() const noexcept
{
    return isZero(&fileType, 30);
}

bool
FSDirEntry::deleted() const noexcept
{
    return fileType == 0;
}

const string
FSDirEntry::typeString() const
{
    switch (fileType) {

        case 0x80: return "DEL";
        case 0x81: return "SEQ";
        case 0x82: return "PRG";
        case 0x83: return "USR";
        case 0x84: return "REL";

        case 0x01: return "*SEQ";
        case 0x02: return "*PRG";
        case 0x03: return "*USR";

        case 0xA0: return "DEL";
        case 0xA1: return "SEQ";
        case 0xA2: return "PRG";
        case 0xA3: return "USR";

        case 0xC0: return "DEL <";
        case 0xC1: return "SEQ <";
        case 0xC2: return "PRG <";
        case 0xC3: return "USR <";
        case 0xC4: return "REL <";
    }

    return "";
}

isize
FSDirEntry::getFileSize() const
{
    return fileSizeHi << 8 | fileSizeLo;
}

bool
FSDirEntry::isHidden() const
{
    return typeString() == "";
}

fs::path
FSDirEntry::getFileSystemRepresentation() const
{
    auto name = getName().str();
    string illegal = "#%&{}\\<>*?/$!'\":@+`|=";
    string result;

    for (const auto c: name) {

        if (illegal.find(c) != std::string::npos) {
            result += "%" + hexstr<2>(c);
        } else {
            result += c;
        }
    }
    return fs::path(result);
}

FSFileType
FSDirEntry::getFileType() const
{
    return (FSFileType)(fileType & 0b111);
}

}
