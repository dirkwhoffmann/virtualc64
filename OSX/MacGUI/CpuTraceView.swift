//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class CpuTraceView : NSTableView {
    
    var c : MyController? = nil

    var cycle : [Int:UInt64] = [:]
    var pc : [Int:String] = [:]
    var a : [Int:String] = [:]
    var x : [Int:String] = [:]
    var y : [Int:String] = [:]
    var flags : [Int:String] = [:]
    var instr : [Int:String] = [:]
    
    var hex = true
    
    override func awakeFromNib() {
        
        dataSource = self
        target = self
    }
    
    func setHex(_ value: Bool) {
        
        hex = value
        refresh()
    }
    
    func refresh() {

        cycle = [:]
        pc = [:]
        a = [:]
        x = [:]
        y = [:]
        flags = [:]
        instr = [:]

        if let count = c?.c64.cpu.recordedInstructions() {
            if count > 0 {
                for i in 0...(count - 1) {

                    let rec = c!.c64.cpu.readRecordedInstruction(i)
                    var diss = c!.c64.cpu.disassembleRecordedInstr(rec, hex: hex)
                    
                    let pcStr = String.init(utf8String:&diss.pc.0)!
                    let aStr = String.init(utf8String:&diss.a.0)!
                    let xStr = String.init(utf8String:&diss.x.0)!
                    let yStr = String.init(utf8String:&diss.y.0)!
                    let flagsStr = String.init(utf8String:&diss.flags.0)!
                    let commandStr = String.init(utf8String:&diss.command.0)!

                    // let str = "\(pc): \(a) \(x) \(y) \(flags) \(command)"
                    /*
                    let str = String.init(format: "%s: %s %s %s   %s %s %s %s %s %s\n",
                                          pc,
                                          byte1, byte2, byte3,
                                          a, x, y, sp,
                                          flags,
                                          command)
                     */
                    let index = count - i - 1
                    cycle[index] = rec.cycle
                    pc[index] = pcStr
                    a[index] = aStr
                    x[index] = xStr
                    y[index] = yStr
                    flags[index] = flagsStr
                    instr[index] = commandStr
                }
            }
        }
        
        reloadData()
    }
}

extension CpuTraceView : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        return 1023;
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        switch(tableColumn?.identifier.rawValue) {
        case "cycle":
            return cycle[row]
        case "PC":
            return pc[row]
        case "A":
            return a[row]
        case "X":
            return x[row]
        case "Y":
            return y[row]
        case "Flags":
            return flags[row]
        case "Instruction":
            return instr[row] ?? ""
        default:
            return "?"
        }
    }
}

