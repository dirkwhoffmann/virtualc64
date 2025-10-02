// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

typealias AppClass = vc64.AppError
typealias CartridgeType = vc64.CartridgeType
typealias Class = vc64.Class
typealias ControlPortDevice = vc64.ControlPortDevice
typealias Compressor = vc64.Compressor
typealias DOSType = vc64.DOSType
typealias ErrorCode = vc64.Fault
typealias EventSlot = vc64.EventSlot
typealias EventSlotEnum = vc64.EventSlotEnum
typealias EventSlotInfo = vc64.EventSlotInfo
typealias Fault = vc64.Fault
typealias FileType = vc64.FileType
typealias FSBlockType = vc64.FSBlockType
typealias FSFileType = vc64.FSFileType
typealias FSUsage = vc64.FSUsage
typealias GamePadAction = vc64.GamePadAction
typealias Message = vc64.Message
typealias Opt = vc64.Opt
typealias RomType = vc64.RomType
typealias TAPVersion = vc64.TAPVersion

let DRIVE8 = vc64.DRIVE8
let DRIVE9 = vc64.DRIVE9

extension Compressor: CustomStringConvertible {

    public var description: String {

        switch self {
        case .NONE:     return ""
        case .GZIP:     return "GZIP"
        case .LZ4:      return "LZ4"
        case .RLE2:     return "RLE2"
        case .RLE3:     return "RLE3"
        default:        fatalError()
        }
    }
}

extension EventSlot: CustomStringConvertible {

    public var description: String { return EventSlotName(self) }
}

extension FileType {

    init?(url: URL?) {
        self = url == nil ? .UNKNOWN : MediaFileProxy.type(of: url)
    }

    static var all: [vc64.FileType] {
        return [
            .SNAPSHOT, .SCRIPT,
            .CRT,
            .T64, .PRG, .P00,
            .D64, .G64,
            .TAP,
            .BASIC_ROM, .CHAR_ROM, .KERNAL_ROM, .VC1541_ROM,
            .FOLDER
        ]
    }

    static var draggable: [vc64.FileType] {
        return [
            .SNAPSHOT, .SCRIPT,
            .CRT, .T64, .PRG, .P00,
            .D64, .G64,
            .TAP,
            .FOLDER
        ]
    }
}

extension DOSType: CustomStringConvertible {

    public var description: String {
        
        switch self {
        
        case .NODOS:  return "No File System"
        case .CBM:    return "Commodore File System"
            
        default:      fatalError()
        }
    }
}

extension FSFileType: CustomStringConvertible {

    public var description: String {
        
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

extension FSBlockType: CustomStringConvertible {

    public var description: String {
        
        switch self {
        
        case .UNKNOWN:  return "Unknown block type"
        case .BAM:      return "Block Allocation Map (BAM)"
        case .DIR:      return "Directory Block"
        case .DATA:     return "Data Block"
            
        default:        fatalError()
        }
    }
}

extension FSUsage: CustomStringConvertible {

    public var description: String {
        
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

extension Fault {

    func description(expected exp: Int = 0) -> String {
        
        switch self {
        
        case .OK:
            return ""
        case .FS_EXPECTED_VAL:
            return String(format: "Expected $%02X", exp)
        case .FS_EXPECTED_MIN:
            return String(format: "Expected a value greater or equal %d", exp)
        case .FS_EXPECTED_MAX:
            return String(format: "Expected a value less or equal %d", exp)

        default:
            fatalError("\(self)")
        }
    }
}

extension CartridgeType {
    
    var description: String {
        
        switch self {
        case .NORMAL: return "Standard"
        case .ACTION_REPLAY: return "Action Replay"
        case .KCS_POWER: return "KCS Power"
        case .FINAL_III: return "Final Cartridge III"
        case .SIMONS_BASIC: return "Simons Basic"
        case .OCEAN: return "Ocean"
        case .EXPERT: return "Expert"
        case .FUNPLAY: return "Fun Play"
        case .SUPER_GAMES: return "Super Games"
        case .ATOMIC_POWER: return "Atomic Power"
        case .EPYX_FASTLOAD: return "Epyx Fastload"
        case .WESTERMANN: return "Westermann"
        case .REX: return "REX"
        case .FINAL_I: return "Final Cartridge I"
        case .MAGIC_FORMEL: return "Magic Formel"
        case .GS: return "Game System 3"
        case .WARPSPEED: return "WarpSpeed"
        case .DINAMIC: return "Dinamic"
        case .ZAXXON: return "Zaxxon (SEGA)"
        case .MAGIC_DESK: return "Magic Desk"
        case .SUPER_SNAPSHOT_V5: return "Super Snapshot"
        case .COMAL80: return "Comal 80"
        case .STRUCTURED_BASIC: return "Structured Basic"
        case .ROSS: return "Ross"
        case .DELA_EP64: return "Dela EP64"
        case .DELA_EP7X8: return "Dela EP7x8"
        case .DELA_EP256: return "Dela EP256"
        case .REX_EP256: return "Rex EP256"
        case .MIKRO_ASSEMBLER: return "Mikro Assembler"
        case .FINAL_PLUS: return "Final Plus"
        case .ACTION_REPLAY4: return "Action Replay 4"
        case .STARDOS: return "Stardos"
        case .EASYFLASH: return "EasyFlash"
        case .EASYFLASH_XBANK: return "EasyFlash (XBank)"
        case .CAPTURE: return "Capture"
        case .ACTION_REPLAY3: return "Action Replay 3"
        case .RETRO_REPLAY: return "Retro Replay"
        case .MMC64: return "MMC 64"
        case .MMC_REPLAY: return "MMC Replay"
        case .IDE64: return "IDE 64"
        case .SUPER_SNAPSHOT: return "Super Snapshot"
        case .IEEE488: return "IEEE 488"
        case .GAME_KILLER: return "Game Killer"
        case .P64: return "P64"
        case .EXOS: return "Exos"
        case .FREEZE_FRAME: return "Freeze Frame"
        case .FREEZE_MACHINE: return "Freeze Machine"
        case .SNAPSHOT64: return "Snapshot 64"
        case .SUPER_EXPLODE_V5: return "Super Explode V5"
        case .MAGIC_VOICE: return "Magic Voice"
        case .ACTION_REPLAY2: return "Action Replay 2"
        case .MACH5: return "Mach 5"
        case .DIASHOW_MAKER: return "Diashow Maker"
        case .PAGEFOX: return "Pagefox"
        case .KINGSOFT: return "Kingsoft"
        case .SILVERROCK_128: return "Silverrock 128"
        case .FORMEL64: return "Formel 64"
        case .RGCD: return "RGCD"
        case .RRNETMK3: return "RRNETMK3"
        case .EASYCALC: return "Easy Calc"
        case .GMOD2: return "GMOD 2"
            
        default: return "CRT type \(self.rawValue) (Unknown)"
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
