// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// Number formatters
let fmt8b  = MyFormatter.init(radix: 2, min: 0, max: 0xFF)
let fmt4d  = MyFormatter.init(radix: 10, min: 0, max: 0xF)
let fmt8d  = MyFormatter.init(radix: 10, min: 0, max: 0xFF)
let fmt16d = MyFormatter.init(radix: 10, min: 0, max: 0xFFFF)
let fmt4x  = MyFormatter.init(radix: 16, min: 0, max: 0xF)
let fmt8x  = MyFormatter.init(radix: 16, min: 0, max: 0xFF)
let fmt16x = MyFormatter.init(radix: 16, min: 0, max: 0xFFFF)

class Inspector: DialogController {
    
    // Commons
    @IBOutlet weak var panel: NSTabView!
    @IBOutlet weak var cpuTab: NSTabView!
    @IBOutlet weak var stopAndGoButton: NSButton!
    @IBOutlet weak var stepIntoButton: NSButton!
    @IBOutlet weak var stepOverButton: NSButton!
    @IBOutlet weak var hexDecSelector: NSMatrix!

    // CPU panel
    @IBOutlet weak var cpuInstrView: InstrTableView!
    @IBOutlet weak var cpuTraceView: TraceTableView!
    @IBOutlet weak var cpuBreakView: BreakTableView!
    @IBOutlet weak var cpuWatchView: WatchTableView!
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
    @IBOutlet weak var cpuIrqCIA: NSButton!
    @IBOutlet weak var cpuIrqVIC: NSButton!
    @IBOutlet weak var cpuIrqEXP: NSButton!
    @IBOutlet weak var cpuNmi: NSButton!
    @IBOutlet weak var cpuNmiCIA: NSButton!
    @IBOutlet weak var cpuNmiKBD: NSButton!
    @IBOutlet weak var cpuNmiEXP: NSButton!
    @IBOutlet weak var cpuRdy: NSButton!
    
    // Memory panel
    @IBOutlet weak var memLayoutButton: NSButton!
    @IBOutlet weak var memLayoutSlider: NSSlider!
    @IBOutlet weak var memRamButton: NSButton!
    @IBOutlet weak var memKernalButton: NSButton!
    @IBOutlet weak var memBasicButton: NSButton!
    @IBOutlet weak var memCharButton: NSButton!
    @IBOutlet weak var memPPButton: NSButton!
    @IBOutlet weak var memIOButton: NSButton!
    @IBOutlet weak var memCartLoButton: NSButton!
    @IBOutlet weak var memCartHiButton: NSButton!
    @IBOutlet weak var memSearchField: NSSearchField!
    @IBOutlet weak var memBankTableView: BankTableView!
    @IBOutlet weak var memTableView: MemTableView!
    @IBOutlet weak var memExrom: NSButton!
    @IBOutlet weak var memGame: NSButton!
    @IBOutlet weak var memCharen: NSButton!
    @IBOutlet weak var memHiram: NSButton!
    @IBOutlet weak var memLoram: NSButton!
    @IBOutlet weak var memSource: NSPopUpButton!
    
    var bankType: [Int: MemoryType] = [:]
    var displayedBank = 0
    var layoutIsDirty = true
    var nextLayoutRefresh = 0

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
    
    // VICII panel
    
    @IBOutlet weak var sprSelector: NSSegmentedControl!

    // Cached state of all C64 components
    var cpuInfo: CPUInfo!
    var ciaInfo: CIAInfo!
    var memInfo: MemInfo!
    var vicInfo: VICIIInfo!
    var sprInfo: SpriteInfo!
    
    var isRunning = true
    
    // Number format selection (hexadecimal or decimal)
    var hex = true
    var fmt4: MyFormatter { return hex ? fmt4x : fmt4d }
    var fmt8: MyFormatter { return hex ? fmt8x : fmt8d }
    var fmt16: MyFormatter { return hex ? fmt16x : fmt16d }

    // Used to determine the items to be refreshed
    var refreshCnt = 0
    
    override func showWindow(_ sender: Any?) {

        super.showWindow(self)
        c64.enableDebugging()
        updateInspectionTarget()
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
        
        c64.inspect()
        
        if let id = panel.selectedTabViewItem?.label {
            
            switch id {
                
            case "CPU": refreshCPU(count: count, full: full)
            case "CIA": refreshCIA(count: count, full: full)
            case "Memory": refreshMemory(count: count, full: full)
            default: break
            }
        }
    }
    
    func scrollToPC() {

        cpuInstrView.jumpTo(addr: Int(cpuInfo.reg.pc0))
    }

    @IBAction func refreshAction(_ sender: NSButton!) {

        track()
        refresh()
    }

    @IBAction func stopAndGoAction(_ sender: NSButton!) {

        track()
        c64.stopAndGo()
    }
    
    @IBAction func stepIntoAction(_ sender: NSButton!) {

        track()
        c64.stepInto()
    }
    
    @IBAction func stepOverAction(_ sender: NSButton!) {

        track()
        c64.stepOver()
    }
    
    @IBAction func hexAction(_ sender: NSButtonCell!) {
        
        hex = true
        c64.cpu.setHex()
        refresh(full: true)
    }

    @IBAction func decAction(_ sender: NSButtonCell!) {
        
        hex = false
        c64.cpu.setDec()
        refresh(full: true)
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
            case "VICII":   parent.c64.setInspectionTarget(INSPECT_VIC)
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
