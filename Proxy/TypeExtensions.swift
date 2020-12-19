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
        case .CBM_DOS:  return "Commodore DOS"
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
        case .FSI_UNKNOWN:     return "Unknown"
        case .FSI_UNUSED:      return "Unused"
        case .FSI_TRACK_REF:   return "Link to the next track"
        case .FSI_SECTOR_REF:  return "Link to the next sector"
        case .FSI_BAM_BLOCK:   return ""
        case .FSI_DIR_BLOCK:   return ""
        case .FSI_DATA:        return "Data byte"
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
        case .EXPECTED_VALUE:
            return String.init(format: "Expected $%02X", exp)
        case .EXPECTED_SMALLER_VALUE:
            return String.init(format: "Expected a value less or equal $%02X", exp)
        default:
            track("\(self)")
            fatalError()
        }
    }
}
