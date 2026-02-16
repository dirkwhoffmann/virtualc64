// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

// #include "utl/abilities/Reflectable.h"
#include "DeviceTypes.h"

namespace retro::vault::cbm {

using namespace utl;

struct FSBlock;

enum class FSFormat : long
{
    NODOS, // No file system
    CBM    // C64 CBM file system
};

struct FSFormatEnum : Reflectable<FSFormatEnum, FSFormat>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFormat::CBM);
    
    static const char *_key(FSFormat value)
    {
        switch (value) {
                
            case FSFormat::CBM:       return "CBM";
            case FSFormat::NODOS:     return "NODOS";
        }
        return "???";
    }
    
    static const char *help(FSFormat value)
    {
        return "";
    }
};

enum class FSBlockType : long
{
    UNKNOWN,
    EMPTY,
    BAM,
    DIR,
    DATA
};

struct FSBlockTypeEnum : Reflectable<FSBlockTypeEnum, FSBlockType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSBlockType::DATA);
    
    static const char *_key(FSBlockType value)
    {
        switch (value) {
                
            case FSBlockType::UNKNOWN:  return "UNKNOWN";
            case FSBlockType::EMPTY:    return "EMPTY";
            case FSBlockType::BAM:      return "BAM";
            case FSBlockType::DIR:      return "USERDIR";
            case FSBlockType::DATA:     return "DATA";
        }
        return "???";
    }
    
    static const char *help(FSBlockType value)
    {
        switch (value) {
                
            case FSBlockType::UNKNOWN:  return "Unknown";
            case FSBlockType::EMPTY:    return "Empty block";
            case FSBlockType::BAM:      return "Block Availability Map (BAM)";
            case FSBlockType::DIR:      return "Directory block";
            case FSBlockType::DATA:     return "Data block";
        }
        return "???";
    }
};

enum class FSFileType : long
{
    DEL,
    SEQ,
    PRG,
    USR,
    REL
};

struct FSFileTypeEnum : Reflectable<FSFileTypeEnum, FSFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFileType::REL);

    static const char *_key(FSFileType value)
    {
        switch (value) {

            case FSFileType::DEL:    return "DEL";
            case FSFileType::SEQ:    return "SEQ";
            case FSFileType::PRG:    return "PRG";
            case FSFileType::USR:    return "USR";
            case FSFileType::REL:    return "REL";
        }
        return "???";
    }

    static const char *help(FSFileType value)
    {
        return "";
    }
};

enum class FSItemType
{
    UNKNOWN,
    UNUSED,
    DOS_VERSION,
    DOS_TYPE,
    ALLOCATION_BITS,
    DISK_ID,
    DISK_NAME,
    TRACK_LINK,
    SECTOR_LINK,
    FIRST_FILE_TRACK,
    FIRST_FILE_SECTOR,
    FIRST_REL_TRACK,
    FIRST_REL_SECTOR,
    FIRST_DIR_TRACK,
    FIRST_DIR_SECTOR,
    FILE_TYPE,
    FILE_NAME,
    FILE_LENGTH_LO,
    FILE_LENGTH_HI,
    REL_RECORD_LENGTH,
    GEOS,
    DATA
};

struct FSItemTypeEnum : Reflectable<FSItemTypeEnum, FSItemType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSItemType::DATA);

    static const char *_key(FSItemType value)
    {
        switch (value) {

            case FSItemType::UNKNOWN:           return "UNKNOWN";
            case FSItemType::UNUSED:            return "UNUSED";
            case FSItemType::DOS_VERSION:       return "DOS_VERSION";
            case FSItemType::DOS_TYPE:          return "DOS_TYPE";
            case FSItemType::ALLOCATION_BITS:   return "ALLOCATION_BITS";
            case FSItemType::DISK_ID:           return "DISK_ID";
            case FSItemType::DISK_NAME:         return "DISK_NAME";
            case FSItemType::TRACK_LINK:        return "TRACK_LINK";
            case FSItemType::SECTOR_LINK:       return "SECTOR_LINK";
            case FSItemType::FIRST_FILE_TRACK:  return "FIRST_FILE_TRACK";
            case FSItemType::FIRST_FILE_SECTOR: return "FIRST_FILE_SECTOR";
            case FSItemType::FIRST_REL_TRACK:   return "FIRST_REL_TRACK";
            case FSItemType::FIRST_REL_SECTOR:  return "FIRST_REL_SECTOR";
            case FSItemType::FIRST_DIR_TRACK:   return "FIRST_DIR_TRACK";
            case FSItemType::FIRST_DIR_SECTOR:  return "FIRST_DIR_SECTOR";
            case FSItemType::FILE_TYPE:         return "FILE_TYPE";
            case FSItemType::FILE_NAME:         return "FILE_NAME";
            case FSItemType::FILE_LENGTH_LO:    return "FILE_LENGTH_LO";
            case FSItemType::FILE_LENGTH_HI:    return "FILE_LENGTH_HI";
            case FSItemType::REL_RECORD_LENGTH: return "REL_RECORD_LENGTH";
            case FSItemType::GEOS:              return "GEOS";
            case FSItemType::DATA:              return "DATA";
        }
        return "???";
    }

    static const char *help(FSItemType value)
    {
        switch (value) {

            case FSItemType::UNKNOWN:           return "Unknown";
            case FSItemType::UNUSED:            return "Unused";
            case FSItemType::DOS_VERSION:       return "DOS version";
            case FSItemType::DOS_TYPE:          return "DOS type";
            case FSItemType::ALLOCATION_BITS:   return "Allocation bits";
            case FSItemType::DISK_ID:           return "Disk ID";
            case FSItemType::DISK_NAME:         return "Disk name";
            case FSItemType::TRACK_LINK:        return "Track link";
            case FSItemType::SECTOR_LINK:       return "Sector link";
            case FSItemType::FIRST_FILE_TRACK:  return "Track of first data block";
            case FSItemType::FIRST_FILE_SECTOR: return "Sector of first data block";
            case FSItemType::FIRST_REL_TRACK:   return "First REL track";
            case FSItemType::FIRST_REL_SECTOR:  return "First REL sector";
            case FSItemType::FIRST_DIR_TRACK:   return "First DIR track";
            case FSItemType::FIRST_DIR_SECTOR:  return "First DIR track";
            case FSItemType::FILE_TYPE:         return "File type";
            case FSItemType::FILE_NAME:         return "File name";
            case FSItemType::FILE_LENGTH_LO:    return "Block size (low byte)";
            case FSItemType::FILE_LENGTH_HI:    return "Block size (high byte)";
            case FSItemType::REL_RECORD_LENGTH: return "REL record length";
            case FSItemType::GEOS:              return "GEOS only";
            case FSItemType::DATA:              return "File data";
        }
        return "???";
    }
};

enum class FSBlockError : long
{
    OK,
    EXPECTED_VALUE,
    EXPECTED_SMALLER_VALUE,
    EXPECTED_LARGER_VALUE
};

struct FSBlockErrorEnum : Reflectable<FSBlockErrorEnum, FSBlockError>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSBlockError::EXPECTED_LARGER_VALUE);

    static const char *_key(FSBlockError value)
    {
        switch (value) {

            case FSBlockError::OK:                          return "OK";
            case FSBlockError::EXPECTED_VALUE:              return "EXPECTED_VALUE";
            case FSBlockError::EXPECTED_SMALLER_VALUE:      return "EXPECTED_SMALLER_VALUE";
            case FSBlockError::EXPECTED_LARGER_VALUE:       return "EXPECTED_LARGER_VALUE";
        }
        return "???";
    }

    static const char *help(FSBlockType value)
    {
        return "";
    }
};


//
// Structures
//

struct FSAttr {
    
    isize size;         // File size in bytes
    isize blocks;       // Number of occupied blocks
    bool isDir;         // Is it a directory?
};

struct FSStat {

    // Meta data

    string name;
    isize bsize;        // Block size
    isize blocks;       // File system capacity in blocks

    // Usage information

    isize freeBlocks;   // Available blocks
    isize usedBlocks;   // Occupied blocks
    isize cachedBlocks; // Total number of cached blocks
    isize dirtyBlocks;  // Number of modified cached blocks

    // Access statistics

    isize generation;   // File system generation counter
};

struct FSDiagnosis
{
    // Block errors
    std::vector<BlockNr> blockErrors;

    // Bitmap errors
    std::vector<BlockNr> usedButUnallocated;
    std::vector<BlockNr> unusedButAllocated;
};

}
