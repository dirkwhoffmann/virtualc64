// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {

    // Updates all visible values in the debug panel
    func refresh() {
        
        if let id = debugPanel.selectedTabViewItem?.identifier as? String {
            switch id {

            case "MEM":
                memTableView.refresh()

            case "VIC":
                refreshVIC()

            case "SID":
                refreshSID()

            default:
                break
            }
        }
    }
    
    func refreshFormatters(hex: Bool) {

        func assignFormatter(_ formatter: Formatter, _ controls: [NSControl]) {
            for control in controls {
                control.abortEditing()
                control.formatter = formatter
                control.needsDisplay = true
            }
        }
        
        // Create formatters
        let fmt3 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0x7)
        let fmt4 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0xF)
        let fmt8 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0xFF)
        let fmt9 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0x1FF)
        let fmt11 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0x7FF)
        let fmt12 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0xFFF)
        let fmt16 = MyFormatter.init(radix: (hex ? 16 : 10), min: 0, max: 0xFFFF)

        // Assign formatters
        assignFormatter(fmt3,
                        [vicDx, vicDy])
        
        assignFormatter(fmt4,
                        [attackRate, decayRate, sustainRate, releaseRate,
                         filterResonance, volume])
        
        assignFormatter(fmt8,
                        [sp, a, x, y])
        
        assignFormatter(fmt8,
                        [ciaPA, ciaPB,
                         todHours, todMinutes, todSeconds, todTenth,
                         alarmHours, alarmMinutes, alarmSeconds, alarmTenth,
                         ciaImr, ciaIcr])
        assignFormatter(fmt8,
                        [vicCycle, spriteY, spritePtr, potX, potY])
    
        assignFormatter(fmt9,
                        [vicRasterline, vicIrqRasterline, spriteX, vicXCounter])

        assignFormatter(fmt11,
                        [filterCutoff])

        assignFormatter(fmt12,
                        [pulseWidth])

        assignFormatter(fmt16,
                        [pc, breakAt,
                         ciaTimerA, ciaLatchA, ciaTimerB, ciaLatchB,
                         frequency])
        
        let columnFormatters = [
            "addr": fmt16,
            "hex0": fmt8,
            "hex1": fmt8,
            "hex2": fmt8,
            "hex3": fmt8
        ]

        for (column, formatter) in columnFormatters {
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
        
        let controls: [NSControl] = [
            // CPU panel
            pc, sp, a, x, y,
            nflag, zflag, cflag, iflag, bflag, dflag, vflag,
            
            // CIA panel
            ciaPRA, ciaPRB, ciaDDRA, ciaDDRB, ciaLatchA, ciaLatchB,
            
            // SID panel
            frequency, pulseWidth, attackRate, decayRate, sustainRate, releaseRate,
            filterCutoff, filterResonance,
            volume,
            
            // VIC panel
            vicDx, vicDy, vicIrqRasterline,
            spriteX, spriteY, spritePtr
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
        if c64.isPoweredOff {
            stopAndGoButton.image = NSImage.init(named: "continueTemplate")
            stopAndGoButton.toolTip = "Run"
            stopAndGoButton.isEnabled = false
            stepIntoButton.isEnabled = false
            stepOverButton.isEnabled = false
        } else if c64.isPaused {
            stopAndGoButton.image = NSImage.init(named: "continueTemplate")
            stopAndGoButton.toolTip = "Run"
            stopAndGoButton.isEnabled = true
            stepIntoButton.isEnabled = true
            stepOverButton.isEnabled = true
        } else {
            stopAndGoButton.image = NSImage.init(named: "pauseTemplate")
            stopAndGoButton.toolTip = "Pause"
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

extension MyController: NSTabViewDelegate {

    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        refresh()
    }
}

//
// Panel independent controls
//

extension MyController {
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.isRunning {
            c64.pause()
            debugger.open()
        }
        refresh()
    }
    
    @IBAction func continueAction(_ sender: Any!) {
        
        if !c64.isRunning {
            c64.run()
        }
        refresh()
    }
}
