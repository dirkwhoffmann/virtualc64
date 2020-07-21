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
let fmt24 = MyFormatter.init(radix: 16, min: 0, max: 0xFFFFFF)
let fmt32 = MyFormatter.init(radix: 16, min: 0, max: 0xFFFFFFFF)
let fmt8b = MyFormatter.init(radix: 2, min: 0, max: 255)

class Inspector: DialogController {
    
    // Commons
    @IBOutlet weak var panel: NSTabView!
    
    // Cached state of all Amiga components
    // var cpuInfo: CPUInfo?
    // var ciaInfo: CIAInfo?
    var isRunning = true
    
    // Used to determine the items to be refreshed
    var refreshCnt = 0
    
    override func showWindow(_ sender: Any?) {

        track()

        super.showWindow(self)
        c64.enableDebugging()
        // updateInspectionTarget()
    }
    
    // Assigns a number formatter to a control
    func assignFormatter(_ formatter: Formatter, _ control: NSControl) {
        
        control.abortEditing()
        control.formatter = formatter
        control.needsDisplay = true
    }
    
    func triggerRefresh() {
        
        if isRunning { refresh(count: refreshCnt) }
        isRunning = c64.isRunning()
        refreshCnt += 1
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if window?.isVisible == false { return }

        /*
        if let id = panel.selectedTabViewItem?.label {

            switch id {
                
            case "CPU": refreshCPU(count: count, full: full)
            case "CIA": refreshCIA(count: count, full: full)
            case "Memory": refreshMemory(count: count, full: full)
            default: break
            }
        }
        */
    }
    
    func fullRefresh() {
        
        refresh(full: true)
    }
    
}

extension Inspector: NSWindowDelegate {
    
    func windowWillClose(_ notification: Notification) {
        
        track("Closing inspector")
        
        // Leave debug mode
        c64?.disableDebugging()
        // amiga?.clearInspectionTarget()
    }
}

extension Inspector: NSTabViewDelegate {
    
    func updateInspectionTarget() {
        
        /*
         if let id = panel.selectedTabViewItem?.label {
         
         switch id {
         
         case "CPU":     parent?.amiga.setInspectionTarget(INS_CPU)
         case "CIA":     parent?.amiga.setInspectionTarget(INS_CIA)
         case "Memory":  parent?.amiga.setInspectionTarget(INS_MEM)
         case "Agnus":   parent?.amiga.setInspectionTarget(INS_AGNUS)
         case "Copper and Blitter":  parent?.amiga.setInspectionTarget(INS_AGNUS)
         case "Denise":  parent?.amiga.setInspectionTarget(INS_DENISE)
         case "Paula":   parent?.amiga.setInspectionTarget(INS_PAULA)
         case "Ports":   parent?.amiga.setInspectionTarget(INS_PORTS)
         case "Events":  parent?.amiga.setInspectionTarget(INS_EVENTS)
         default:        break
         }
         
         fullRefresh()
         }
         */
    }
    
    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {
        
        if tabView === panel {
            updateInspectionTarget()
        }
        /*
         if tabView === cpuTab {
         instrTableView.refresh(full: true)
         traceTableView.refresh(full: true)
         }
         */
    }
}
