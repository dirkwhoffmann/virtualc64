// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

let fmt4  = MyFormatter.init(radix: 16, min: 0, max: 0xF)
let fmt8  = MyFormatter.init(radix: 16, min: 0, max: 0xFF)
let fmt16 = MyFormatter.init(radix: 16, min: 0, max: 0xFFFF)
let fmt8b = MyFormatter.init(radix: 2, min: 0, max: 255)

class Inspector: DialogController {
    
    // Commons
    @IBOutlet weak var panel: NSTabView!
    @IBOutlet weak var cpuTab: NSTabView!
    @IBOutlet weak var stopAndGoButton: NSButton!
    @IBOutlet weak var stepIntoButton: NSButton!
    @IBOutlet weak var stepOverButton: NSButton!
    @IBOutlet weak var numberFormatSelector: NSMatrix!

    // CPU panel
    @IBOutlet weak var cpuInstrView: InstrTableView!
    @IBOutlet weak var cpuTraceView: TraceTableView!
    @IBOutlet weak var cpuBreakView: BreakTableView!
    @IBOutlet weak var cpuWatchView: BreakTableView!
    @IBOutlet weak var cpuPc: NSTextField!
    @IBOutlet weak var cpuSp: NSTextField!
    @IBOutlet weak var cpuA: NSTextField!
    @IBOutlet weak var cpuX: NSTextField!
    @IBOutlet weak var cpuY: NSTextField!
    @IBOutlet weak var cpuN: NSButton!
    @IBOutlet weak var cpuZ: NSButton!
    @IBOutlet weak var cpuC: NSButton!
    @IBOutlet weak var cpuI: NSButton!
    @IBOutlet weak var cpuB: NSButton!
    @IBOutlet weak var cpuD: NSButton!
    @IBOutlet weak var cpuV: NSButton!
    @IBOutlet weak var cpuPortReg: NSTextField!
    @IBOutlet weak var cpuPortDir: NSTextField!
    @IBOutlet weak var cpuIrq: NSButton!
    @IBOutlet weak var cpuNmi: NSButton!
    @IBOutlet weak var cpuRdy: NSButton!
    
    // Memory panel
    @IBOutlet weak var memSearchField: NSSearchField!
    @IBOutlet weak var memBankTableView: BankTableView!
    @IBOutlet weak var memTableView: MemTableView!
    @IBOutlet weak var memExrom: NSButton!
    @IBOutlet weak var memGame: NSButton!
    @IBOutlet weak var memCharen: NSButton!
    @IBOutlet weak var memHiram: NSButton!
    @IBOutlet weak var memLoram: NSButton!
    @IBOutlet weak var memSource: NSPopUpButton!

    var bank = 0
    var memSrc = 0
    
    // CIA panel
    @IBOutlet weak var ciaSelector: NSSegmentedControl!
    @IBOutlet weak var ciaPRA: NSTextField!
    @IBOutlet weak var ciaPRAbinary: NSTextField!
    @IBOutlet weak var ciaDDRA: NSTextField!
    @IBOutlet weak var ciaDDRAbinary: NSTextField!
    @IBOutlet weak var ciaPA7: NSButton!
    @IBOutlet weak var ciaPA6: NSButton!
    @IBOutlet weak var ciaPA5: NSButton!
    @IBOutlet weak var ciaPA4: NSButton!
    @IBOutlet weak var ciaPA3: NSButton!
    @IBOutlet weak var ciaPA2: NSButton!
    @IBOutlet weak var ciaPA1: NSButton!
    @IBOutlet weak var ciaPA0: NSButton!
    @IBOutlet weak var ciaPRB: NSTextField!
    @IBOutlet weak var ciaPRBbinary: NSTextField!
    @IBOutlet weak var ciaDDRB: NSTextField!
    @IBOutlet weak var ciaDDRBbinary: NSTextField!
    @IBOutlet weak var ciaPB7: NSButton!
    @IBOutlet weak var ciaPB6: NSButton!
    @IBOutlet weak var ciaPB5: NSButton!
    @IBOutlet weak var ciaPB4: NSButton!
    @IBOutlet weak var ciaPB3: NSButton!
    @IBOutlet weak var ciaPB2: NSButton!
    @IBOutlet weak var ciaPB1: NSButton!
    @IBOutlet weak var ciaPB0: NSButton!
    @IBOutlet weak var ciaTA: NSTextField!
    @IBOutlet weak var ciaTAlatch: NSTextField!
    @IBOutlet weak var ciaTArunning: NSButton!
    @IBOutlet weak var ciaTAtoggle: NSButton!
    @IBOutlet weak var ciaTApbout: NSButton!
    @IBOutlet weak var ciaTAoneShot: NSButton!
    @IBOutlet weak var ciaTB: NSTextField!
    @IBOutlet weak var ciaTBlatch: NSTextField!
    @IBOutlet weak var ciaTBrunning: NSButton!
    @IBOutlet weak var ciaTBtoggle: NSButton!
    @IBOutlet weak var ciaTBpbout: NSButton!
    @IBOutlet weak var ciaTBoneShot: NSButton!
    @IBOutlet weak var ciaICR: NSTextField!
    @IBOutlet weak var ciaICRbinary: NSTextField!
    @IBOutlet weak var ciaIMR: NSTextField!
    @IBOutlet weak var ciaIMRbinary: NSTextField!
    @IBOutlet weak var ciaIntLineLow: NSButton!
    @IBOutlet weak var ciaTodHours: NSTextField!
    @IBOutlet weak var ciaTodMinutes: NSTextField!
    @IBOutlet weak var ciaTodSeconds: NSTextField!
    @IBOutlet weak var ciaTodTenth: NSTextField!
    @IBOutlet weak var ciaAlarmHours: NSTextField!
    @IBOutlet weak var ciaAlarmMinutes: NSTextField!
    @IBOutlet weak var ciaAlarmSeconds: NSTextField!
    @IBOutlet weak var ciaTodIntEnable: NSButton!
    @IBOutlet weak var ciaAlarmTenth: NSTextField!
    @IBOutlet weak var ciaSDR: NSTextField!
    @IBOutlet weak var ciaSSR: NSTextField!
    @IBOutlet weak var ciaSSRbinary: NSTextField!
    @IBOutlet weak var ciaIdleCycles: NSTextField!
    @IBOutlet weak var ciaIdleLevelText: NSTextField!
    @IBOutlet weak var ciaIdleLevel: NSLevelIndicator!
    
    // Cached state of all Amiga components
    var cpuInfo: CPUInfo!
    var ciaInfo: CIAInfo!
    var isRunning = true
    
    // Used to determine the items to be refreshed
    var refreshCnt = 0
    
    override func showWindow(_ sender: Any?) {

        track()

        super.showWindow(self)
        c64.enableDebugging()
        updateInspectionTarget()
    }
    
    // Assigns a number formatter to a control
    func assignFormatter(_ formatter: Formatter, _ control: NSControl) {
        
        control.abortEditing()
        control.formatter = formatter
        control.needsDisplay = true
    }
    
    func fullRefresh() {
        
        refresh(full: true)
    }
    
    func continuousRefresh() {
        
        if isRunning { refresh(count: refreshCnt) }
        isRunning = c64.isRunning
        refreshCnt += 1
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if window?.isVisible == false { return }
        
        if full {
            
            if c64.isRunning {
                stopAndGoButton.image = NSImage.init(named: "pauseTemplate")
                stepIntoButton.isEnabled = false
                stepOverButton.isEnabled = false
            } else {
                stopAndGoButton.image = NSImage.init(named: "continueTemplate")
                stepIntoButton.isEnabled = true
                stepOverButton.isEnabled = true
            }
        }
        
        if let id = panel.selectedTabViewItem?.label {
            
            switch id {
                
            case "CPU": refreshCPU(count: count, full: full)
            case "CIA": refreshCIA(count: count, full: full)
            case "Memory": refreshMemory(count: count, full: full)
            default: break
            }
        }
    }
}

extension Inspector: NSWindowDelegate {
    
    func windowWillClose(_ notification: Notification) {
        
        track("Closing inspector")
        
        // Leave debug mode
        c64?.disableDebugging()
        c64?.clearInspectionTarget()
    }
}

extension Inspector: NSTabViewDelegate {
    
    func updateInspectionTarget() {
        
        if let id = panel.selectedTabViewItem?.label {
            
            switch id {
                
            case "CPU":     parent.c64.setInspectionTarget(INSPECT_CPU)
            case "Memory":  parent.c64.setInspectionTarget(INSPECT_MEM)
            case "CIA":     parent.c64.setInspectionTarget(INSPECT_CIA)
            case "VIC":     parent.c64.setInspectionTarget(INSPECT_VIC)
            case "SID":     parent.c64.setInspectionTarget(INSPECT_SID)
            default:        break
            }
            
            fullRefresh()
        }
    }
    
    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        
        if tabView === panel {
            updateInspectionTarget()
        }
        
        if tabView === cpuTab {
            cpuInstrView.refresh(full: true)
            cpuTraceView.refresh(full: true)
        }
    }
}
