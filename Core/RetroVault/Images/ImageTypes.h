// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace retro::vault {

using namespace utl;

//
// Enumerations
//

enum class ImageType : long
{
    UNKNOWN,
    FLOPPY,
    HARDDISK,
    TAPE,
    CARTRIDGE,
    EXECUTABLE
};

struct ImageTypeEnum : utl::Reflectable<ImageTypeEnum, ImageType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ImageType::EXECUTABLE);

    static const char *_key(ImageType value)
    {
        switch (value) {

            case ImageType::UNKNOWN:    return "UNKNOWN";
            case ImageType::FLOPPY:     return "FLOPPY";
            case ImageType::HARDDISK:   return "HARDDISK";
            case ImageType::TAPE:       return "TAPE";
            case ImageType::CARTRIDGE:  return "CARTRIDGE";
            case ImageType::EXECUTABLE: return "EXECUTABLE";
        }
        return "???";
    }
    static const char *help(ImageType value)
    {
        switch (value) {

            case ImageType::UNKNOWN:    return "Unknown";
            case ImageType::FLOPPY:     return "Floppy Disk Image";
            case ImageType::HARDDISK:   return "Hard Drive Image";
            case ImageType::TAPE:       return "Tape Image";
            case ImageType::CARTRIDGE:  return "Cartridge Image";
            case ImageType::EXECUTABLE: return "File Image";
        }
        return "???";
    }
};

enum class ImageFormat : long
{
    UNKNOWN,
    ADF,
    ADZ,
    EADF,
    HDF,
    HDZ,
    IMG,
    ST,
    DMS,
    EXE,
    D64
};

struct ImageFormatEnum : utl::Reflectable<ImageFormatEnum, ImageFormat>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ImageFormat::D64);

    static const char *_key(ImageFormat value)
    {
        switch (value) {

            case ImageFormat::UNKNOWN: return "UNKNOWN";
            case ImageFormat::ADF:     return "ADF";
            case ImageFormat::ADZ:     return "ADZ";
            case ImageFormat::EADF:    return "EADF";
            case ImageFormat::HDF:     return "HDF";
            case ImageFormat::HDZ:     return "HDZ";
            case ImageFormat::IMG:     return "IMG";
            case ImageFormat::ST:      return "ST";
            case ImageFormat::DMS:     return "DMS";
            case ImageFormat::EXE:     return "EXE";
            case ImageFormat::D64:     return "D64";
        }
        return "???";
    }
    static const char *help(ImageFormat value)
    {
        switch (value) {

            case ImageFormat::UNKNOWN: return "Unknown";
            case ImageFormat::ADF:     return "Amiga Disk File";
            case ImageFormat::ADZ:     return "Compressed Amiga Disk File";
            case ImageFormat::EADF:    return "Extended Amiga Disk File";
            case ImageFormat::HDF:     return "Hard Disk File";
            case ImageFormat::HDZ:     return "Compressed Hard Disk File";
            case ImageFormat::IMG:     return "PC Disk Image";
            case ImageFormat::ST:      return "AtariST Disk Image";
            case ImageFormat::DMS:     return "Disk Masher System";
            case ImageFormat::EXE:     return "Amiga Executable";
            case ImageFormat::D64:     return "C64 Floppy Disk";
        }
        return "???";
    }
};

enum class FSFamily : long
{
    UNKNOWN,
    AMIGA,
    CBM,
    DOS
};

struct FSFamilyEnum : utl::Reflectable<FSFamilyEnum, FSFamily>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFamily::DOS);

    static const char *_key(FSFamily value)
    {
        switch (value) {

            case FSFamily::UNKNOWN:    return "UNKNOWN";
            case FSFamily::AMIGA:      return "AMIGA";
            case FSFamily::CBM:        return "CBM";
            case FSFamily::DOS:        return "DOS";
        }
        return "???";
    }
    static const char *help(FSFamily value)
    {
        switch (value) {

            case FSFamily::UNKNOWN:    return "Unknown";
            case FSFamily::AMIGA:      return "Amiga File System";
            case FSFamily::CBM:        return "CBM File System";
            case FSFamily::DOS:        return "DOS File System";
        }
        return "???";
    }
};


//
// Structures
//

struct ImageInfo {

    ImageType   type;
    ImageFormat format;
};

}
