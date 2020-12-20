// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension FSVolumeType {

    var description: String {
        
        switch self {
        
        case .NODOS:    return "No File System"
        case .CBM_DOS:  return "Commodore File System (CBM DOS 2.6)"
            
        default:        fatalError()
        }
    }
}

extension FSBlockType {
    
    var description: String {
        
        switch self {
        
        case .UNKNOWN_BLOCK:  return "Unknown block type"
        case .BAM_BLOCK:      return "Block Allocation Map (BAM)"
        case .DIR_BLOCK:      return "Directory Block"
        case .DATA_BLOCK:     return "Data Block"
            
        default:              fatalError()
        }
    }
}

extension FSItemType {
    
    var description: String {
        
        switch self {
        
        case .FSI_UNKNOWN:           return "Unknown"
        case .FSI_UNUSED:            return "Unused"
        case .FSI_DOS_VERSION:       return "DOS version"
        case .FSI_DOS_TYPE:          return "DOS type"
        case .FSI_ALLOCATION_BITS:   return "Block allocation bits"
        case .FSI_DISK_ID:           return "Disk ID"
        case .FSI_DISK_NAME:         return "Disk name (PETSCII character)"
        case .FSI_TRACK_LINK:        return "Link to the next block (track number)"
        case .FSI_SECTOR_LINK:       return "Link to the next block (sector number)"
        case .FSI_FIRST_FILE_TRACK:  return "File start (track number)"
        case .FSI_FIRST_FILE_SECTOR: return "File start (sector number)"
        case .FSI_FIRST_REL_TRACK:   return "First side-sector block (track number)"
        case .FSI_FIRST_REL_SECTOR:  return "First side-sector block (sector number)"
        case .FSI_FIRST_DIR_TRACK:   return "Track number of the first directory block"
        case .FSI_FIRST_DIR_SECTOR:  return "Sector number of the first directory block"
        case .FSI_FILE_TYPE:         return "File type"
        case .FSI_FILE_NAME:         return "File name (PETSCII character)"
        case .FSI_FILE_LENGTH_LO:    return "File length (low byte)"
        case .FSI_FILE_LENGTH_HI:    return "File length (high byte)"
        case .FSI_REL_RECORD_LENGTH: return "REL file record length"
        case .FSI_GEOS:              return "Unused (except for GEOS)"
        case .FSI_DATA:              return "Data byte"
            
        default:
            fatalError()
        }
    }
}

extension FSError {
    
    func description(expected exp: Int = 0) -> String {
        
        switch self {
        
        case .OK:
            return ""
        case .EXPECTED:
            return String.init(format: "Expected $%02X", exp)
        case .EXPECTED_MIN:
            return String.init(format: "Expected a value greater or equal %d", exp)
        case .EXPECTED_MAX:
            return String.init(format: "Expected a value less or equal %d", exp)

        default:
            track("\(self)")
            fatalError()
        }
    }
}

extension TAPVersion {
    
    var description: String {
        
        switch self {
        
        case .ORIGINAL: return "TAP type 0 (Original pulse layout)"
        case .ADVANCED: return "TAP type 1 (Advanced pulse layout)"
            
        default: return "TAP type \(self.rawValue) (Unknown)"
        }
    }
}
