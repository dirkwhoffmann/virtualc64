// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DeviceTypes.h"

namespace retro::vault::amiga {

using namespace utl;

struct FSBlock;

enum class FSFormat : long
{
    OFS      = 0,    // Original File System
    FFS      = 1,    // Fast File System
    OFS_INTL = 2,    // "International" (not supported)
    FFS_INTL = 3,    // "International" (not supported)
    OFS_DC   = 4,    // "Directory Cache" (not supported)
    FFS_DC   = 5,    // "Directory Cache" (not supported)
    OFS_LNFS = 6,    // "Long Filenames" (not supported)
    FFS_LNFS = 7,    // "Long Filenames" (not supported)
    NODOS
};

struct FSFormatEnum : Reflectable<FSFormatEnum, FSFormat>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFormat::NODOS);
    
    static const char *_key(FSFormat value)
    {
        switch (value) {
                
            case FSFormat::OFS:       return "OFS";
            case FSFormat::FFS:       return "FFS";
            case FSFormat::OFS_INTL:  return "OFS_INTL";
            case FSFormat::FFS_INTL:  return "FFS_INTL";
            case FSFormat::OFS_DC:    return "OFS_DC";
            case FSFormat::FFS_DC:    return "FFS_DC";
            case FSFormat::OFS_LNFS:  return "OFS_LNFS";
            case FSFormat::FFS_LNFS:  return "FFS_LNFS";
            case FSFormat::NODOS:     return "NODOS";
        }
        return "???";
    }
    
    static const char *help(FSFormat value)
    {
        return "";
    }
    
    static FSFormat fromDosType(u32 value)
    {
        switch (value) {
                
            case 0x444F5300:    return FSFormat::OFS;
            case 0x444F5301:    return FSFormat::FFS;
            case 0x444F5302:    return FSFormat::OFS_INTL;
            case 0x444F5303:    return FSFormat::FFS_INTL;
            case 0x444F5304:    return FSFormat::OFS_DC;
            case 0x444F5305:    return FSFormat::FFS_DC;
            case 0x444F5306:    return FSFormat::OFS_LNFS;
            case 0x444F5307:    return FSFormat::FFS_LNFS;
            default:            return FSFormat::NODOS;
        }
    }
};

inline bool isOFSVolumeType(FSFormat value)
{
    switch (value) {
            
        case FSFormat::OFS:
        case FSFormat::OFS_INTL:
        case FSFormat::OFS_DC:
        case FSFormat::OFS_LNFS:    return true;
        default:                    return false;
    }
}

inline bool isFFSVolumeType(FSFormat value)
{
    switch (value) {
            
        case FSFormat::FFS:
        case FSFormat::FFS_INTL:
        case FSFormat::FFS_DC:
        case FSFormat::FFS_LNFS:    return true;
        default:                    return false;
    }
}

inline bool isINTLVolumeType(FSFormat value)
{
    switch (value) {

        case FSFormat::OFS_INTL:
        case FSFormat::FFS_INTL:
        case FSFormat::OFS_DC:
        case FSFormat::FFS_DC:
        case FSFormat::OFS_LNFS:
        case FSFormat::FFS_LNFS:    return true;
        default:                    return false;
    }
}

enum class FSBlockType : long
{
    UNKNOWN,
    EMPTY,
    BOOT,
    ROOT,
    BITMAP,
    BITMAP_EXT,
    USERDIR,
    FILEHEADER,
    FILELIST,
    DATA_OFS,
    DATA_FFS
};

struct FSBlockTypeEnum : Reflectable<FSBlockTypeEnum, FSBlockType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSBlockType::DATA_FFS);
    
    static const char *_key(FSBlockType value)
    {
        switch (value) {
                
            case FSBlockType::UNKNOWN:     return "UNKNOWN";
            case FSBlockType::EMPTY:       return "EMPTY";
            case FSBlockType::BOOT:        return "BOOT";
            case FSBlockType::ROOT:        return "ROOT";
            case FSBlockType::BITMAP:      return "BITMAP";
            case FSBlockType::BITMAP_EXT:  return "BITMAP_EXT";
            case FSBlockType::USERDIR:     return "USERDIR";
            case FSBlockType::FILEHEADER:  return "FILEHEADER";
            case FSBlockType::FILELIST:    return "FILELIST";
            case FSBlockType::DATA_OFS:    return "DATA_OFS";
            case FSBlockType::DATA_FFS:    return "DATA_FFS";
        }
        return "???";
    }
    
    static const char *help(FSBlockType value)
    {
        switch (value) {
                
            case FSBlockType::UNKNOWN:     return "Unknown";
            case FSBlockType::EMPTY:       return "Empty block";
            case FSBlockType::BOOT:        return "Boot block";
            case FSBlockType::ROOT:        return "Root block";
            case FSBlockType::BITMAP:      return "Bitmap block";
            case FSBlockType::BITMAP_EXT:  return "Bitmap extension block";
            case FSBlockType::USERDIR:     return "User directory block";
            case FSBlockType::FILEHEADER:  return "File header block";
            case FSBlockType::FILELIST:    return "File list block";
            case FSBlockType::DATA_OFS:    return "Data block (OFS)";
            case FSBlockType::DATA_FFS:    return "Data block (FFS)";
        }
        return "???";
    }
};

enum class FSItemType
{
    UNKNOWN,
    UNUSED,
    DOS_HEADER,
    DOS_VERSION,
    BOOTCODE,
    TYPE_ID,
    SUBTYPE_ID,
    SELF_REF,
    CHECKSUM,
    HASHTABLE_SIZE,
    HASH_REF,
    PROT_BITS,
    BCPL_STRING_LENGTH,
    BCPL_DISK_NAME,
    BCPL_DIR_NAME,
    BCPL_FILE_NAME,
    BCPL_COMMENT,
    CREATED_DAY,
    CREATED_MIN,
    CREATED_TICKS,
    MODIFIED_DAY,
    MODIFIED_MIN,
    MODIFIED_TICKS,
    NEXT_HASH_REF,
    PARENT_DIR_REF,
    FILEHEADER_REF,
    EXT_BLOCK_REF,
    BITMAP_BLOCK_REF,
    BITMAP_EXT_BLOCK_REF,
    BITMAP_VALIDITY,
    FILESIZE,
    DATA_BLOCK_NUMBER,
    DATA_BLOCK_REF_COUNT,
    FIRST_DATA_BLOCK_REF,
    NEXT_DATA_BLOCK_REF,
    DATA_BLOCK_REF,
    DATA_COUNT,
    DATA,
    BITMAP
};

struct FSItemTypeEnum : Reflectable<FSItemTypeEnum, FSItemType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSItemType::BITMAP);
    
    static const char *_key(FSItemType value)
    {
        switch (value) {
                
            case FSItemType::UNKNOWN:               return "UNKNOWN";
            case FSItemType::UNUSED:                return "UNUSED";
            case FSItemType::DOS_HEADER:            return "DOS_HEADER";
            case FSItemType::DOS_VERSION:           return "DOS_VERSION";
            case FSItemType::BOOTCODE:              return "BOOTCODE";
            case FSItemType::TYPE_ID:               return "TYPE_ID";
            case FSItemType::SUBTYPE_ID:            return "SUBTYPE_ID";
            case FSItemType::SELF_REF:              return "SELF_REF";
            case FSItemType::CHECKSUM:              return "CHECKSUM";
            case FSItemType::HASHTABLE_SIZE:        return "HASHTABLE_SIZE";
            case FSItemType::HASH_REF:              return "HASH_REF";
            case FSItemType::PROT_BITS:             return "PROT_BITS";
            case FSItemType::BCPL_STRING_LENGTH:    return "BCPL_STRING_LENGTH";
            case FSItemType::BCPL_DISK_NAME:        return "BCPL_DISK_NAME";
            case FSItemType::BCPL_DIR_NAME:         return "BCPL_DIR_NAME";
            case FSItemType::BCPL_FILE_NAME:        return "BCPL_FILE_NAME";
            case FSItemType::BCPL_COMMENT:          return "BCPL_COMMENT";
            case FSItemType::CREATED_DAY:           return "CREATED_DAY";
            case FSItemType::CREATED_MIN:           return "CREATED_MIN";
            case FSItemType::CREATED_TICKS:         return "CREATED_TICKS";
            case FSItemType::MODIFIED_DAY:          return "MODIFIED_DAY";
            case FSItemType::MODIFIED_MIN:          return "MODIFIED_MIN";
            case FSItemType::MODIFIED_TICKS:        return "MODIFIED_TICKS";
            case FSItemType::NEXT_HASH_REF:         return "NEXT_HASH_REF";
            case FSItemType::PARENT_DIR_REF:        return "PARENT_DIR_REF";
            case FSItemType::FILEHEADER_REF:        return "FILEHEADER_REF";
            case FSItemType::EXT_BLOCK_REF:         return "EXT_BLOCK_REF";
            case FSItemType::BITMAP_BLOCK_REF:      return "BITMAP_BLOCK_REF";
            case FSItemType::BITMAP_EXT_BLOCK_REF:  return "BITMAP_EXT_BLOCK_REF";
            case FSItemType::BITMAP_VALIDITY:       return "BITMAP_VALIDITY";
            case FSItemType::FILESIZE:              return "FILESIZE";
            case FSItemType::DATA_BLOCK_NUMBER:     return "DATA_BLOCK_NUMBER";
            case FSItemType::DATA_BLOCK_REF_COUNT:  return "DATA_BLOCK_REF_COUNT";
            case FSItemType::FIRST_DATA_BLOCK_REF:  return "FIRST_DATA_BLOCK_REF";
            case FSItemType::NEXT_DATA_BLOCK_REF:   return "NEXT_DATA_BLOCK_REF";
            case FSItemType::DATA_BLOCK_REF:        return "DATA_BLOCK_REF";
            case FSItemType::DATA_COUNT:            return "DATA_COUNT";
            case FSItemType::DATA:                  return "DATA";
            case FSItemType::BITMAP:                return "BITMAP";
        }
        return "???";
    }

    static const char *help(FSItemType value)
    {
        switch (value) {

            case FSItemType::UNKNOWN:               return "Unknown";
            case FSItemType::UNUSED:                return "Unused";
            case FSItemType::DOS_HEADER:            return "AmigaDOS header signature";
            case FSItemType::DOS_VERSION:           return "AmigaDOS version number";
            case FSItemType::BOOTCODE:              return "Boot code instruction";
            case FSItemType::TYPE_ID:               return "Type identifier";
            case FSItemType::SUBTYPE_ID:            return "Subtype identifier";
            case FSItemType::SELF_REF:              return "Block reference to itself";
            case FSItemType::CHECKSUM:              return "Checksum";
            case FSItemType::HASHTABLE_SIZE:        return "Hashtable size";
            case FSItemType::HASH_REF:              return "Hashtable entry";
            case FSItemType::PROT_BITS:             return "Protection status bits";
            case FSItemType::BCPL_STRING_LENGTH:    return "BCPL string Length";
            case FSItemType::BCPL_DISK_NAME:        return "Disk name (BCPL character)";
            case FSItemType::BCPL_DIR_NAME:         return "Directory name (BCPL character)";
            case FSItemType::BCPL_FILE_NAME:        return "File name (BCPL character)";
            case FSItemType::BCPL_COMMENT:          return "Comment (BCPL character)";
            case FSItemType::CREATED_DAY:           return "Creation date (days)";
            case FSItemType::CREATED_MIN:           return "Creation date (minutes)";
            case FSItemType::CREATED_TICKS:         return "Creation date (ticks)";
            case FSItemType::MODIFIED_DAY:          return "Modification date (day)";
            case FSItemType::MODIFIED_MIN:          return "Modification date (minutes)";
            case FSItemType::MODIFIED_TICKS:        return "Modification date (ticks)";
            case FSItemType::NEXT_HASH_REF:         return "Reference to the next hash block";
            case FSItemType::PARENT_DIR_REF:        return "Parent directory block reference";
            case FSItemType::FILEHEADER_REF:        return "File header block reference";
            case FSItemType::EXT_BLOCK_REF:         return "Next extension block reference";
            case FSItemType::BITMAP_BLOCK_REF:      return "Bitmap block reference";
            case FSItemType::BITMAP_EXT_BLOCK_REF:  return "Extension bitmap block reference";
            case FSItemType::BITMAP_VALIDITY:       return "Bitmap validity bits";
            case FSItemType::DATA_BLOCK_REF_COUNT:  return "Number of data block references";
            case FSItemType::FILESIZE:              return "File size";
            case FSItemType::DATA_BLOCK_NUMBER:     return "Position in the data block chain";
            case FSItemType::DATA_BLOCK_REF:        return "Data block reference";
            case FSItemType::FIRST_DATA_BLOCK_REF:  return "Reference to the first data block";
            case FSItemType::NEXT_DATA_BLOCK_REF:   return "Reference to next data block";
            case FSItemType::DATA_COUNT:            return "Number of stored data bytes";
            case FSItemType::DATA:                  return "Data byte";
            case FSItemType::BITMAP:                return "Block allocation table";
        }
        return "???";
    }
};


enum class FSBlockError : long
{
    OK,
    EXPECTED_VALUE,
    EXPECTED_SMALLER_VALUE,
    EXPECTED_NO_REF,
    EXPECTED_REF,
    EXPECTED_SELFREF,
    EXPECTED_BITMAP_BLOCK,
    EXPECTED_BITMAP_EXT_BLOCK,
    EXPECTED_HASHABLE_BLOCK,
    EXPECTED_USERDIR_OR_ROOT,
    EXPECTED_DATA_BLOCK,
    EXPECTED_FILE_HEADER_BLOCK,
    EXPECTED_FILE_LIST_BLOCK,
    EXPECTED_DATABLOCK_NR,
    INVALID_HASHTABLE_SIZE
};

struct FSBlockErrorEnum : Reflectable<FSBlockErrorEnum, FSBlockError>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSBlockError::INVALID_HASHTABLE_SIZE);

    static const char *_key(FSBlockError value)
    {
        switch (value) {

            case FSBlockError::OK:                          return "OK";
            case FSBlockError::EXPECTED_VALUE:              return "EXPECTED_VALUE";
            case FSBlockError::EXPECTED_SMALLER_VALUE:      return "EXPECTED_SMALLER_VALUE";
            case FSBlockError::EXPECTED_NO_REF:             return "EXPECTED_NO_REF";
            case FSBlockError::EXPECTED_REF:                return "EXPECTED_REF";
            case FSBlockError::EXPECTED_SELFREF:            return "EXPECTED_SELFREF";
            case FSBlockError::EXPECTED_BITMAP_BLOCK:       return "EXPECTED_BITMAP_BLOCK";
            case FSBlockError::EXPECTED_BITMAP_EXT_BLOCK:   return "EXPECTED_BITMAP_EXT_BLOCK";
            case FSBlockError::EXPECTED_HASHABLE_BLOCK:     return "EXPECTED_HASHABLE_BLOCK";
            case FSBlockError::EXPECTED_USERDIR_OR_ROOT:    return "EXPECTED_USERDIR_OR_ROOT";
            case FSBlockError::EXPECTED_DATA_BLOCK:         return "EXPECTED_DATA_BLOCK";
            case FSBlockError::EXPECTED_FILE_HEADER_BLOCK:  return "EXPECTED_FILE_HEADER_BLOCK";
            case FSBlockError::EXPECTED_FILE_LIST_BLOCK:    return "EXPECTED_FILE_LIST_BLOCK";
            case FSBlockError::EXPECTED_DATABLOCK_NR:       return "EXPECTED_DATABLOCK_NR";
            case FSBlockError::INVALID_HASHTABLE_SIZE:      return "INVALID_HASHTABLE_SIZE";
        }
        return "???";
    }

    static const char *help(FSBlockType value)
    {
        return "";
    }
};

enum class BootBlockType
{
    STANDARD,
    VIRUS,
    CUSTOM
};

struct BootBlockTypeEnum : Reflectable<BootBlockTypeEnum, BootBlockType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(BootBlockType::CUSTOM);

    static const char *_key(BootBlockType value)
    {
        switch (value) {

            case BootBlockType::STANDARD:  return "STANDARD";
            case BootBlockType::VIRUS:     return "VIRUS";
            case BootBlockType::CUSTOM:    return "CUSTOM";
        }
        return "???";
    }
    static const char *help(BootBlockType value)
    {
        return "";
    }
};

enum class BootBlockId
{
    NONE,
    AMIGADOS_13,
    AMIGADOS_20,
    SCA,
    BYTE_BANDIT
};

struct BootBlockIdEnum : Reflectable<BootBlockIdEnum, BootBlockId>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(BootBlockId::BYTE_BANDIT);

    static const char *_key(BootBlockId value)
    {
        switch (value) {

            case BootBlockId::NONE:         return "NONE";
            case BootBlockId::AMIGADOS_13:  return "AMIGADOS_13";
            case BootBlockId::AMIGADOS_20:  return "AMIGADOS_20";
            case BootBlockId::SCA:          return "SCA";
            case BootBlockId::BYTE_BANDIT:  return "BYTE_BANDIT";
        }
        return "???";
    }
    static const char *help(BootBlockId value)
    {
        switch (value) {

            case BootBlockId::NONE:         return "Empty block";
            case BootBlockId::AMIGADOS_13:  return "Kickstart 1.3 boot block";
            case BootBlockId::AMIGADOS_20:  return "Kickstart 2.0 boot block";
            case BootBlockId::SCA:          return "SCA Virus";
            case BootBlockId::BYTE_BANDIT:  return "Byte Bandit Virus";
        }
        return "???";
    }
};


//
// Structures
//

struct FSTraits
{
    FSFormat dos = FSFormat::NODOS;

    isize blocks = 0;
    isize bytes = 0;
    isize bsize = 512;
    isize reserved = 2;

    bool ofs() const { return isOFSVolumeType(dos); }
    bool ffs() const { return isFFSVolumeType(dos); }
    bool intl() const { return isINTLVolumeType(dos); }
    bool adf() const;
};

typedef struct
{
    // Block errors
    std::vector<BlockNr> blockErrors;

    // Bitmap errors
    std::vector<BlockNr> usedButUnallocated;
    std::vector<BlockNr> unusedButAllocated;
}
FSDiagnosis;

}
