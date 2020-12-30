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

extension FSFileType {

    var description: String {
        
        switch self {
        
        case .DEL:  return "DEL"
        case .SEQ:  return "SEQ"
        case .PRG:  return "PRG"
        case .USR:  return "USR"
        case .REL:  return "REL"
            
        default:    fatalError()
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

extension CartridgeType {
    
    var description: String {
        
        switch self {
        case .CRT_NORMAL: return "Standard Cartridge"
        case .CRT_ACTION_REPLAY: return "Action Replay Cartridge"
        case .CRT_KCS_POWER: return "KCS Power Cartridge"
        case .CRT_FINAL_III: return "Final Cartridge III"
        case .CRT_SIMONS_BASIC: return "Simons Basic Cartridge"
        case .CRT_OCEAN: return "Ocean Cartridge"
        case .CRT_EXPERT: return "Expert Cartridge"
        case .CRT_FUNPLAY: return "Fun Play Cartridge"
        case .CRT_SUPER_GAMES: return "Super Games Cartridge"
        case .CRT_ATOMIC_POWER: return "Atomic Power Cartridge"
        case .CRT_EPYX_FASTLOAD: return "Epyx Fastload Cartridge"
        case .CRT_WESTERMANN: return "Westermann Cartridge"
        case .CRT_REX: return "REX Cartridge"
        case .CRT_FINAL_I: return "Final Cartridge I"
        case .CRT_MAGIC_FORMEL: return "Magic Formel Cartridge"
        case .CRT_GAME_SYSTEM_SYSTEM_3: return "Game Cartridge System 3"
        case .CRT_WARPSPEED: return "WarpSpeed Cartridge"
        case .CRT_DINAMIC: return "Dinamic Cartridge"
        case .CRT_ZAXXON: return "Zaxxon Cartridge (SEGA)"
        case .CRT_MAGIC_DESK: return "Magic Desk Cartridge"
        case .CRT_SUPER_SNAPSHOT_V5: return "Super Snapshot Cartridge"
        case .CRT_COMAL80: return "Comal 80 Cartridge"
        case .CRT_STRUCTURED_BASIC: return "Structured Basic Cartridge"
        case .CRT_ROSS: return "Ross Cartridge"
        case .CRT_DELA_EP64: return "Dela EP64 Cartridge"
        case .CRT_DELA_EP7x8: return "Dela EP7x8 Cartridge"
        case .CRT_DELA_EP256: return "Dela EP256 Cartridge"
        case .CRT_REX_EP256: return "Rex EP256 Cartridge"
        case .CRT_MIKRO_ASS: return "Mikro Assembler Cartridge"
        case .CRT_FINAL_PLUS: return "Final Plus Cartridge"
        case .CRT_ACTION_REPLAY4: return "Action replay 4 Cartridge"
        case .CRT_STARDOS: return "Stardos Cartridge"
        case .CRT_EASYFLASH: return "EasyFlash Cartridge"
        case .CRT_EASYFLASH_XBANK: return "EasyFlash (XBank)"
        case .CRT_CAPTURE: return "Capture Cartridge"
        case .CRT_ACTION_REPLAY3: return "Action Replay Cartridge 3"
        case .CRT_RETRO_REPLAY: return "Metro Replay Cartridge"
        case .CRT_MMC64: return "MMC 64 Cartridge"
        case .CRT_MMC_REPLAY: return "MMC Replay Cartridge"
        case .CRT_IDE64: return "IDE 64 Cartridge"
        case .CRT_SUPER_SNAPSHOT: return "Super Snapshot Cartridge"
        case .CRT_IEEE488: return "IEEE 488 Cartridge"
        case .CRT_GAME_KILLER: return "Game Killer Cartridge"
        case .CRT_P64: return "P64 Cartridge"
        case .CRT_EXOS: return "Exos Cartridge"
        case .CRT_FREEZE_FRAME: return "Freeze Frame Cartridge"
        case .CRT_FREEZE_MACHINE: return "Freeze Machine Cartridge"
        case .CRT_SNAPSHOT64: return "Snapshot 64 Cartridge"
        case .CRT_SUPER_EXPLODE_V5: return "Super Explode V5 Cartridge"
        case .CRT_MAGIC_VOICE: return "Magic Voice Cartridge"
        case .CRT_ACTION_REPLAY2: return "Action Replay Cartridge 2"
        case .CRT_MACH5: return "Mach 5 Cartridge"
        case .CRT_DIASHOW_MAKER: return "Diashow Maker Cartridge"
        case .CRT_PAGEFOX: return "Pagefox Cartridge"
        case .CRT_KINGSOFT: return "Kingsoft Cartridge"
        case .CRT_SILVERROCK_128: return "Silverrock 128"
        case .CRT_FORMEL64: return "Formel 64 Cartridge"
        case .CRT_RGCD: return "RGCD Cartridge"
        case .CRT_RRNETMK3: return "RRNETMK3 Cartridge"
        case .CRT_EASYCALC: return "Easy Calc Cartridge"
        case .CRT_GMOD2: return "GMOD 2 Cartridge"
            
        default: return "TAP type \(self.rawValue) (Unknown)"
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
