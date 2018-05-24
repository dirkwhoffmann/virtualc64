//
//  Debugger.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.05.18.
//

import Foundation

extension MyController {
    
    func setupDebugger() {
        
        // Start with hexadecimal number format
        setHexadecimalAction(self)
        
        // Create and assign binary number formatter
        let bF = MyFormatter.init(inFormat: "[0-1]{0,7}", outFormat: "", radix: 2)
        ciaPAbinary.formatter = bF
        ciaPRA.formatter = bF
        ciaDDRA.formatter = bF
        ciaPBbinary.formatter = bF
        ciaPRB.formatter = bF
        ciaDDRB.formatter = bF
        ciaIcrBinary.formatter = bF
        ciaImrBinary.formatter = bF
    }
}

//
// Panel independent controls
//

extension MyController {
    
    @IBAction func stepIntoAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        c64.step()
        refresh()
    }
  
    @IBAction func stepOverAction(_ sender: Any!) {

        document?.updateChangeCount(.changeDone)
        
        // If the next instruction is a JSR instruction, ...
        if (c64.cpu.readPC() == 0x20) {

            // we set soft breakpoint at next command
            c64.cpu.setSoftBreakpoint(c64.cpu.addressOfNextInstruction())
            c64.run()

        } else {
            
            // Same as step
            stepIntoAction(self)
        }
    }
    
    @IBAction func stopAndGoAction(_ sender: Any!) {
    
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        } else {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.isRunning() {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func continueAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        }
        refresh()
    }
    
    @IBAction func setDecimalAction(_ sender: Any!) {
  
        hex = false
        cpuTableView.setHex(false)

        let bF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", radix: 10)
        let sF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", radix: 10)
        let wF = MyFormatter.init(inFormat: "[0-9]{0,5}", outFormat: "%05d", radix: 10)
        refresh(bF, word: wF, threedigit: sF)
    }
    
    @IBAction func setHexadecimalAction(_ sender: Any!) {
        
        hex = true
        cpuTableView.setHex(true)

        let bF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,2}", outFormat: "%02X", radix: 16)
        let sF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,3}", outFormat: "%03X", radix: 16)
        let wF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,4}", outFormat: "%04X", radix: 16)
        refresh(bF, word: wF, threedigit: sF)
    }
}








