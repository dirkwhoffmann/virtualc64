// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension DOSType {

    var description: String {
        
        switch self {
        
        case .NODOS:  return "No File System"
        case .CBM:    return "Commodore File System (CBM DOS 2.6)"
            
        default:      fatalError()
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
        
        case .UNKNOWN:  return "Unknown block type"
        case .BAM:      return "Block Allocation Map (BAM)"
        case .DIR:      return "Directory Block"
        case .DATA:     return "Data Block"
            
        default:        fatalError()
        }
    }
}

extension FSUsage {
    
    var description: String {
        
        switch self {
        
        case .UNKNOWN:           return "Unknown"
        case .UNUSED:            return "Unused"
        case .DOS_VERSION:       return "DOS version"
        case .DOS_TYPE:          return "DOS type"
        case .ALLOCATION_BITS:   return "Block allocation bits"
        case .DISK_ID:           return "Disk ID"
        case .DISK_NAME:         return "Disk name (PETSCII character)"
        case .TRACK_LINK:        return "Link to the next block (track number)"
        case .SECTOR_LINK:       return "Link to the next block (sector number)"
        case .FIRST_FILE_TRACK:  return "File start (track number)"
        case .FIRST_FILE_SECTOR: return "File start (sector number)"
        case .FIRST_REL_TRACK:   return "First side-sector block (track number)"
        case .FIRST_REL_SECTOR:  return "First side-sector block (sector number)"
        case .FIRST_DIR_TRACK:   return "Track number of the first directory block"
        case .FIRST_DIR_SECTOR:  return "Sector number of the first directory block"
        case .FILE_TYPE:         return "File type"
        case .FILE_NAME:         return "File name (PETSCII character)"
        case .FILE_LENGTH_LO:    return "File length (low byte)"
        case .FILE_LENGTH_HI:    return "File length (high byte)"
        case .REL_RECORD_LENGTH: return "REL file record length"
        case .GEOS:              return "Unused (except for GEOS)"
        case .DATA:              return "Data byte"
            
        default:
            fatalError()
        }
    }
}

extension ErrorCode {
    
    func description(expected exp: Int = 0) -> String {
        
        switch self {
        
        case .OK:
            return ""
        case .FS_EXPECTED_VAL:
            return String.init(format: "Expected $%02X", exp)
        case .FS_EXPECTED_MIN:
            return String.init(format: "Expected a value greater or equal %d", exp)
        case .FS_EXPECTED_MAX:
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
        case .NORMAL: return "Standard Cartridge"
        case .ACTION_REPLAY: return "Action Replay Cartridge"
        case .KCS_POWER: return "KCS Power Cartridge"
        case .FINAL_III: return "Final Cartridge III"
        case .SIMONS_BASIC: return "Simons Basic Cartridge"
        case .OCEAN: return "Ocean Cartridge"
        case .EXPERT: return "Expert Cartridge"
        case .FUNPLAY: return "Fun Play Cartridge"
        case .SUPER_GAMES: return "Super Games Cartridge"
        case .ATOMIC_POWER: return "Atomic Power Cartridge"
        case .EPYX_FASTLOAD: return "Epyx Fastload Cartridge"
        case .WESTERMANN: return "Westermann Cartridge"
        case .REX: return "REX Cartridge"
        case .FINAL_I: return "Final Cartridge I"
        case .MAGIC_FORMEL: return "Magic Formel Cartridge"
        case .GAME_SYSTEM_SYSTEM_3: return "Game Cartridge System 3"
        case .WARPSPEED: return "WarpSpeed Cartridge"
        case .DINAMIC: return "Dinamic Cartridge"
        case .ZAXXON: return "Zaxxon Cartridge (SEGA)"
        case .MAGIC_DESK: return "Magic Desk Cartridge"
        case .SUPER_SNAPSHOT_V5: return "Super Snapshot Cartridge"
        case .COMAL80: return "Comal 80 Cartridge"
        case .STRUCTURED_BASIC: return "Structured Basic Cartridge"
        case .ROSS: return "Ross Cartridge"
        case .DELA_EP64: return "Dela EP64 Cartridge"
        case .DELA_EP7X8: return "Dela EP7x8 Cartridge"
        case .DELA_EP256: return "Dela EP256 Cartridge"
        case .REX_EP256: return "Rex EP256 Cartridge"
        case .MIKRO_ASS: return "Mikro Assembler Cartridge"
        case .FINAL_PLUS: return "Final Plus Cartridge"
        case .ACTION_REPLAY4: return "Action replay 4 Cartridge"
        case .STARDOS: return "Stardos Cartridge"
        case .EASYFLASH: return "EasyFlash Cartridge"
        case .EASYFLASH_XBANK: return "EasyFlash (XBank)"
        case .CAPTURE: return "Capture Cartridge"
        case .ACTION_REPLAY3: return "Action Replay Cartridge 3"
        case .RETRO_REPLAY: return "Metro Replay Cartridge"
        case .MMC64: return "MMC 64 Cartridge"
        case .MMC_REPLAY: return "MMC Replay Cartridge"
        case .IDE64: return "IDE 64 Cartridge"
        case .SUPER_SNAPSHOT: return "Super Snapshot Cartridge"
        case .IEEE488: return "IEEE 488 Cartridge"
        case .GAME_KILLER: return "Game Killer Cartridge"
        case .P64: return "P64 Cartridge"
        case .EXOS: return "Exos Cartridge"
        case .FREEZE_FRAME: return "Freeze Frame Cartridge"
        case .FREEZE_MACHINE: return "Freeze Machine Cartridge"
        case .SNAPSHOT64: return "Snapshot 64 Cartridge"
        case .SUPER_EXPLODE_V5: return "Super Explode V5 Cartridge"
        case .MAGIC_VOICE: return "Magic Voice Cartridge"
        case .ACTION_REPLAY2: return "Action Replay Cartridge 2"
        case .MACH5: return "Mach 5 Cartridge"
        case .DIASHOW_MAKER: return "Diashow Maker Cartridge"
        case .PAGEFOX: return "Pagefox Cartridge"
        case .KINGSOFT: return "Kingsoft Cartridge"
        case .SILVERROCK_128: return "Silverrock 128"
        case .FORMEL64: return "Formel 64 Cartridge"
        case .RGCD: return "RGCD Cartridge"
        case .RRNETMK3: return "RRNETMK3 Cartridge"
        case .EASYCALC: return "Easy Calc Cartridge"
        case .GMOD2: return "GMOD 2 Cartridge"
            
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
