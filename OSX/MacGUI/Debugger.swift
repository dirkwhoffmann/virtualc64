//
//  Debugger.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.05.18.
//

import Foundation

extension MyController {

    func setupDebugger() {
        
        selectedVoice = 0
        
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
    
    // Updates all visible values in the debug panel
    func refresh() {
        
        if let id = debugPanel.selectedTabViewItem?.identifier as? String {
            switch id {
            case "CPU":
                refreshCPU()
                cpuTableView.refresh()
                break
            case "MEM":
                memTableView.refresh()
                break
            case "CIA":
                refreshCIA()
                break
            case "VIC":
                refreshVIC()
                break
            case "SID":
                refreshSID()
                break
            default:
                break
            }
        }
    }
    
    func refresh(byteFormatter: Formatter, wordFormatter: Formatter, threeDigitFormatter: Formatter) {

        let byteFormatterControls:[NSControl] = [
            // CPU panel
            sp, a, x, y,
            // CIA panel
            ciaPA, ciaPB,
            todHours, todMinutes, todSeconds, todTenth,
            alarmHours, alarmMinutes, alarmSeconds, alarmTenth,
            ciaImr, ciaIcr,
            // VIC panel
            //vicSpriteY,
            // SID panel
            attackRate, decayRate, sustainRate, releaseRate,
            filterResonance,
            volume, potX, potY
        ]
       
        let wordFormatterControls:[NSControl] = [
            // CPU panel
            pc, breakAt,
            // CIA panel
            ciaTimerA, ciaLatchA, ciaTimerB, ciaLatchB,
            // VIC panel
            vicRasterline, vicRasterInterrupt,
            // SID panel
            frequency, pulseWidth, filterCutoff
        ]
        
        let threeDigitFormatterControls:[NSControl] = [
            // VIC panel
            // vicSpriteX,
        ]
    
        // Bind formatters
        for control in byteFormatterControls {
            control.abortEditing()
            control.formatter = byteFormatter
            control.needsDisplay = true
        }
        for control in wordFormatterControls {
            control.abortEditing()
            control.formatter = wordFormatter
            control.needsDisplay = true
        }
        for control in threeDigitFormatterControls {
            control.abortEditing()
            control.formatter = threeDigitFormatter
            control.needsDisplay = true
        }
 
        let columnFormatters = [
            "addr" : wordFormatter,
            "hex0" : byteFormatter,
            "hex1" : byteFormatter,
            "hex2" : byteFormatter,
            "hex3" : byteFormatter
        ]

        for (column,formatter) in columnFormatters {
            let columnId = NSUserInterfaceItemIdentifier(rawValue: column)
            if let tableColumn = memTableView.tableColumn(withIdentifier: columnId) {
                if let cell = tableColumn.dataCell as? NSCell {
                    cell.formatter = formatter
                }
            }
        }
        
        refresh()
    }
    
    func setUserEditing(_ enabled: Bool) {
        
        let controls:[NSControl] = [
            // CPU panel
            pc, sp, a, x, y,
            nflag, zflag, cflag, iflag, bflag, dflag, vflag,
            
            // SID panel
            frequency, pulseWidth, attackRate, decayRate, sustainRate, releaseRate,
            filterCutoff, filterResonance,
            volume,
            
            // VIC panel
            // vicSpriteX,
        ]
  
        for control in controls {
            control.isEnabled = enabled
        }
    
        // Enable / disable table columns
        let columns = ["hex0", "hex1", "hex2", "hex3"]
        
        for column in columns {
            let columnId = NSUserInterfaceItemIdentifier(rawValue: column)
            if let tableColumn = memTableView.tableColumn(withIdentifier: columnId) {
                tableColumn.isEditable = enabled
            }
        }
        
    // Change image and state of debugger control buttons
        if !c64.isRunnable() {
            stopAndGoButton.image = NSImage.init(named: NSImage.Name(rawValue: "play32"))
            stopAndGoButton.isEnabled = false
            stepIntoButton.isEnabled = false
            stepOverButton.isEnabled = false
        } else if c64.isHalted() {
            stopAndGoButton.image = NSImage.init(named: NSImage.Name(rawValue: "play32"))
            stopAndGoButton.isEnabled = true
            stepIntoButton.isEnabled = true
            stepOverButton.isEnabled = true
        } else {
            stopAndGoButton.image = NSImage.init(named: NSImage.Name(rawValue: "pause32"))
            stopAndGoButton.isEnabled = true
            stepIntoButton.isEnabled = false
            stepOverButton.isEnabled = false
        }
    }
    
    func enableUserEditing() {
        setUserEditing(true)
    }

    func disableUserEditing() {
        setUserEditing(false)
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
            debugger.open()
        }
        refresh()
    }
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.isRunning() {
            c64.halt()
            debugger.open()
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
        refresh(byteFormatter: bF, wordFormatter: wF, threeDigitFormatter: sF)
    }
    
    @IBAction func setHexadecimalAction(_ sender: Any!) {
        
        hex = true
        cpuTableView.setHex(true)

        let bF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,2}", outFormat: "%02X", radix: 16)
        let sF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,3}", outFormat: "%03X", radix: 16)
        let wF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,4}", outFormat: "%04X", radix: 16)
        refresh(byteFormatter: bF, wordFormatter: wF, threeDigitFormatter: sF)
    }
}








