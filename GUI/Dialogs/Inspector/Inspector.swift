// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Inspector: DialogController {

    let fmt3  = MyFormatter(radix: 16, min: 0, max: 0x7)
    let fmt4  = MyFormatter(radix: 16, min: 0, max: 0xF)
    let fmt6  = MyFormatter(radix: 16, min: 0, max: 0x3F)
    let fmt8  = MyFormatter(radix: 16, min: 0, max: 0xFF)
    let fmt9  = MyFormatter(radix: 16, min: 0, max: 0x1FF)
    let fmt10 = MyFormatter(radix: 16, min: 0, max: 0x3FF)
    let fmt12 = MyFormatter(radix: 16, min: 0, max: 0xFFF)
    let fmt16 = MyFormatter(radix: 16, min: 0, max: 0xFFFF)
    let fmt8b = MyFormatter(radix: 2, min: 0, max: 0xFF)
    let fmt16b = MyFormatter(radix: 2, min: 0, max: 0xFFFF)

    var format = 0 {
        didSet {
            switch format {
            case 0: hex = true; padding = false
            case 1: hex = true; padding = true
            case 2: hex = false; padding = false
            case 3: hex = false; padding = true
            default:
                fatalError()
            }
            emu?.set(.DASM_NUMBERS, value: format)
        }
    }
    var hex = true {
        didSet {
            fmt3.radix = hex ? 16 : 10
            fmt4.radix = hex ? 16 : 10
            fmt6.radix = hex ? 16 : 10
            fmt8.radix = hex ? 16 : 10
            fmt9.radix = hex ? 16 : 10
            fmt12.radix = hex ? 16 : 10
            fmt16.radix = hex ? 16 : 10
            fullRefresh()
        }
    }
    var padding = false {
        didSet {
            fmt3.padding = padding
            fmt4.padding = padding
            fmt6.padding = padding
            fmt8.padding = padding
            fmt9.padding = padding
            fmt12.padding = padding
            fmt16.padding = padding
            fullRefresh()
        }
    }
    
    // Commons
    @IBOutlet weak var panel: NSTabView!
    @IBOutlet weak var message: NSTextField!

    // CPU panel
    @IBOutlet weak var cpuTab: NSTabView!
    @IBOutlet weak var cpuInstrView: InstrTableView!
    @IBOutlet weak var cpuTraceView: TraceTableView!
    @IBOutlet weak var cpuBreakView: BreakTableView!
    @IBOutlet weak var cpuWatchView: WatchTableView!
    @IBOutlet weak var cpuPc: NSTextField!
    @IBOutlet weak var cpuPcWarning: NSTextField!
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
    @IBOutlet weak var memTab: NSTabView!
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
    @IBOutlet weak var memHeatmapView: HeatmapView!

    var bankType: [Int: vc64.MemType] = [:]
    var displayedBank = 0
    var layoutIsDirty = true
    var nextLayoutRefresh = 0

    // Bus panel
    @IBOutlet weak var busDebug: NSButton!
    @IBOutlet weak var busChannelR: NSButton!
    @IBOutlet weak var busChannelI: NSButton!
    @IBOutlet weak var busChannelC: NSButton!
    @IBOutlet weak var busChannelG: NSButton!
    @IBOutlet weak var busChannelP: NSButton!
    @IBOutlet weak var busChannelS: NSButton!
    
    @IBOutlet weak var busColorR: NSColorWell!
    @IBOutlet weak var busColorI: NSColorWell!
    @IBOutlet weak var busColorC: NSColorWell!
    @IBOutlet weak var busColorG: NSColorWell!
    @IBOutlet weak var busColorP: NSColorWell!
    @IBOutlet weak var busColorS: NSColorWell!

    @IBOutlet weak var busOverlay: NSButton!
    @IBOutlet weak var busDisplayMode: NSPopUpButton!
    @IBOutlet weak var busOpacity: NSSlider!

    @IBOutlet weak var busDmaView: DmaView!
    
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
    @IBOutlet weak var vicYCounter: NSTextField!
    @IBOutlet weak var vicXCounter: NSTextField!
    @IBOutlet weak var vicVC: NSTextField!
    @IBOutlet weak var vicVCBase: NSTextField!
    @IBOutlet weak var vicRC: NSTextField!
    @IBOutlet weak var vicVMLI: NSTextField!
    @IBOutlet weak var vicCtrl1: NSTextField!
    @IBOutlet weak var vicCtrl2: NSTextField!
    @IBOutlet weak var vicDy: NSTextField!
    @IBOutlet weak var vicDx: NSTextField!
    @IBOutlet weak var vicDenBit: NSButton!
    @IBOutlet weak var vicBadLine: NSButton!
    @IBOutlet weak var vicDisplayState: NSButton!
    @IBOutlet weak var vicVBlank: NSButton!
    @IBOutlet weak var vicScreenGeometry: NSPopUpButton!
    @IBOutlet weak var vicVFlop: NSButton!
    @IBOutlet weak var vicHFlop: NSButton!
    @IBOutlet weak var vicDisplayMode: NSPopUpButton!
    @IBOutlet weak var vicBorderColor: NSColorWell!
    @IBOutlet weak var vicBgColor0: NSColorWell!
    @IBOutlet weak var vicBgColor1: NSColorWell!
    @IBOutlet weak var vicBgColor2: NSColorWell!
    @IBOutlet weak var vicBgColor3: NSColorWell!
    @IBOutlet weak var vicMemSelect: NSTextField!
    @IBOutlet weak var vicUltimax: NSButton!
    @IBOutlet weak var vicMemoryBankAddr: NSPopUpButton!
    @IBOutlet weak var vicScreenMemoryAddr: NSPopUpButton!
    @IBOutlet weak var vicCharMemoryAddr: NSPopUpButton!
    @IBOutlet weak var vicIrqRasterline: NSTextField!
    @IBOutlet weak var vicImr: NSTextField!
    @IBOutlet weak var vicImrLP: NSButton!
    @IBOutlet weak var vicImrSS: NSButton!
    @IBOutlet weak var vicImrSB: NSButton!
    @IBOutlet weak var vicImrRaster: NSButton!
    @IBOutlet weak var vicIrr: NSTextField!
    @IBOutlet weak var vicIrrIrq: NSButton!
    @IBOutlet weak var vicIrrLP: NSButton!
    @IBOutlet weak var vicIrrSS: NSButton!
    @IBOutlet weak var vicIrrSB: NSButton!
    @IBOutlet weak var vicIrrRaster: NSButton!
    @IBOutlet weak var latchedLPX: NSTextField!
    @IBOutlet weak var latchedLPY: NSTextField!
    @IBOutlet weak var vicLpLine: NSButton!
    @IBOutlet weak var vicLpIrqHasOccurred: NSButton!
    
    @IBOutlet weak var sprSelector: NSSegmentedControl!
    @IBOutlet weak var sprX: NSTextField!
    @IBOutlet weak var sprY: NSTextField!
    @IBOutlet weak var sprEnabled: NSButton!
    @IBOutlet weak var sprExpandX: NSButton!
    @IBOutlet weak var sprExpandY: NSButton!
    @IBOutlet weak var sprPriority: NSButton!
    @IBOutlet weak var sprMulticolor: NSButton!
    @IBOutlet weak var sprSSCollision: NSButton!
    @IBOutlet weak var sprSBCollision: NSButton!
    @IBOutlet weak var sprColor: NSColorWell!
    @IBOutlet weak var sprExtra1: NSColorWell!
    @IBOutlet weak var sprExtra2: NSColorWell!

    @IBOutlet weak var cutEnable: NSButton!
    @IBOutlet weak var cutBorder: NSButton!
    @IBOutlet weak var cutForeground: NSButton!
    @IBOutlet weak var cutBackground: NSButton!
    @IBOutlet weak var cutSprites: NSButton!
    @IBOutlet weak var cutSprite0: NSButton!
    @IBOutlet weak var cutSprite1: NSButton!
    @IBOutlet weak var cutSprite2: NSButton!
    @IBOutlet weak var cutSprite3: NSButton!
    @IBOutlet weak var cutSprite4: NSButton!
    @IBOutlet weak var cutSprite5: NSButton!
    @IBOutlet weak var cutSprite6: NSButton!
    @IBOutlet weak var cutSprite7: NSButton!
    @IBOutlet weak var cutOpacity: NSSlider!
    
    // SID panel
    @IBOutlet weak var sidSelector: NSSegmentedControl!
    @IBOutlet weak var sidWaveform1: NSPopUpButton!
    @IBOutlet weak var sidFrequency1: NSTextField!
    @IBOutlet weak var sidPulseWidth1: NSTextField!
    @IBOutlet weak var sidPulseWidthText1: NSTextField!
    @IBOutlet weak var sidAttackRate1: NSTextField!
    @IBOutlet weak var sidDecayRate1: NSTextField!
    @IBOutlet weak var sidSustainRate1: NSTextField!
    @IBOutlet weak var sidReleaseRate1: NSTextField!
    @IBOutlet weak var sidGateBit1: NSButton!
    @IBOutlet weak var sidTestBit1: NSButton!
    @IBOutlet weak var sidSyncBit1: NSButton!
    @IBOutlet weak var sidRingBit1: NSButton!
    
    @IBOutlet weak var sidWaveform2: NSPopUpButton!
    @IBOutlet weak var sidFrequency2: NSTextField!
    @IBOutlet weak var sidPulseWidth2: NSTextField!
    @IBOutlet weak var sidPulseWidthText2: NSTextField!
    @IBOutlet weak var sidAttackRate2: NSTextField!
    @IBOutlet weak var sidDecayRate2: NSTextField!
    @IBOutlet weak var sidSustainRate2: NSTextField!
    @IBOutlet weak var sidReleaseRate2: NSTextField!
    @IBOutlet weak var sidGateBit2: NSButton!
    @IBOutlet weak var sidTestBit2: NSButton!
    @IBOutlet weak var sidSyncBit2: NSButton!
    @IBOutlet weak var sidRingBit2: NSButton!
    
    @IBOutlet weak var sidWaveform3: NSPopUpButton!
    @IBOutlet weak var sidFrequency3: NSTextField!
    @IBOutlet weak var sidPulseWidth3: NSTextField!
    @IBOutlet weak var sidPulseWidthText3: NSTextField!
    @IBOutlet weak var sidAttackRate3: NSTextField!
    @IBOutlet weak var sidDecayRate3: NSTextField!
    @IBOutlet weak var sidSustainRate3: NSTextField!
    @IBOutlet weak var sidReleaseRate3: NSTextField!
    @IBOutlet weak var sidGateBit3: NSButton!
    @IBOutlet weak var sidTestBit3: NSButton!
    @IBOutlet weak var sidSyncBit3: NSButton!
    @IBOutlet weak var sidRingBit3: NSButton!
    
    @IBOutlet weak var sidVolume: NSTextField!
    @IBOutlet weak var sidPotX: NSTextField!
    @IBOutlet weak var sidPotY: NSTextField!

    @IBOutlet weak var sidFilterType: NSPopUpButton!
    @IBOutlet weak var sidFilterCutoff: NSTextField!
    @IBOutlet weak var sidFilterResonance: NSTextField!
    @IBOutlet weak var sidFilter1: NSButton!
    @IBOutlet weak var sidFilter2: NSButton!
    @IBOutlet weak var sidFilter3: NSButton!
    
    @IBOutlet weak var sidWaveformView: WaveformView!
    @IBOutlet weak var sidWaveformSource: NSPopUpButton!
    @IBOutlet weak var sidAudioBufferLevel: NSLevelIndicator!
    @IBOutlet weak var sidAudioBufferLevelText: NSTextField!
    @IBOutlet weak var sidBufferUnderflows: NSTextField!
    @IBOutlet weak var sidBufferOverflows: NSTextField!

    // Events panel
    @IBOutlet weak var evCpuProgress: NSTextField!
    @IBOutlet weak var evVicProgress: NSTextField!
    @IBOutlet weak var evCia1Progress: NSTextField!
    @IBOutlet weak var evCia2Progress: NSTextField!
    @IBOutlet weak var evTableView: EventTableView!

    // Cached state of all C64 components
    var cpuInfo: vc64.CPUInfo!
    var ciaInfo: vc64.CIAInfo!
    var ciaStats: vc64.CIAStats!
    var memInfo: vc64.MemInfo!
    var vicInfo: vc64.VICIIInfo!
    var sprInfo: vc64.SpriteInfo!
    var eventInfo: vc64.C64Info!
    var isRunning = true
    
    var toolbar: InspectorToolbar? { return window?.toolbar as? InspectorToolbar }
    var config: Configuration { return parent.config }
    
    // Used to determine the items to be refreshed
    var refreshCnt = 0
        
    deinit {
        debug(.lifetime)
    }
    
    override func dialogWillShow() {
        
        super.dialogWillShow()
        
        // Hide the panel selector
        // panel.tabPosition = .none
   
        // Enter debug mode
        emu?.trackOn()
        emu?.set(.MEM_HEATMAP, enable: true)
        updateInspectionTarget()
        
        // Adjust window height to match what we see in interface builder
        if let window = self.window {
            
            let contentHeight: CGFloat = 440
            let toolbarHeight = window.frame.height - window.contentView!.frame.height
            let totalHeight = contentHeight + toolbarHeight
            
            var frame = window.frame
            frame.size.height = totalHeight
            window.setFrame(frame, display: true)
        }
        
        jumpTo(addr: 0)
    }
    
    /*
    override func showWindow(_ sender: Any?) {

        super.showWindow(self)

        // Enter debug mode
        emu?.trackOn()
        emu?.set(.MEM_HEATMAP, enable: true)
        updateInspectionTarget()
    }
    */
    
    override func dialogDidShow() {
        
        super.dialogDidShow()
        refresh(full: true)
    }
    
    func fullRefresh() {
        
        refresh(full: true)
    }
    
    func continuousRefresh() {
        
        if isRunning { refresh(count: refreshCnt) }
        isRunning = emu?.running ?? false
        refreshCnt += 1
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if window?.isVisible == false { return }
        
        if full {

        }

        if let id = panel.selectedTabViewItem?.label {

            switch id {
                
            case "CPU":     refreshCPU(count: count, full: full)
            case "Memory":  refreshMemory(count: count, full: full)
            case "Bus":     refreshBus(count: count, full: full)
            case "CIA":     refreshCIA(count: count, full: full)
            case "VICII":   refreshVIC(count: count, full: full)
            case "SID":     refreshSID(count: count, full: full)
            case "Events":  refreshEvents(count: count, full: full)

            default:
                break
            }
        }
        
        toolbar?.updateToolbar(full: full)
    }
    
    func selectPanel(_ nr: Int) {
        
        if nr <  panel.numberOfTabViewItems {

            panel.selectTabViewItem(at: nr)
            fullRefresh()
        }
    }
    
    func processMessage(_ msg: vc64.Message) {
    
        var pc: Int { return Int(msg.cpu.pc) }
        // var vector: Int { return Int(msg.cpu.vector) }
        
        switch msg.type {
                        
        case .CONFIG:
            
            fullRefresh()
            
        case .POWER:
            
            message.stringValue = ""
            fullRefresh()
            
        case .RUN:
            
            message.stringValue = ""
            cpuInstrView.alertAddr = nil
            fullRefresh()

        case .PAUSE:
            
            fullRefresh()
            
        case .STEP:
            
            message.stringValue = ""
            cpuInstrView.alertAddr = nil
            fullRefresh()
            scrollToPC()
            
        case .RESET:
            
            message.stringValue = ""
            cpuInstrView.alertAddr = nil
            fullRefresh()

        case .BREAKPOINT_REACHED:
            
            cpuInstrView.alertAddr = nil
            scrollToPC(pc: pc)
            
        case .WATCHPOINT_REACHED:
            
            cpuInstrView.alertAddr = pc
            scrollToPC(pc: pc)

        case .CPU_JUMPED:
            
            fullRefresh()
            scrollToPC()
            
        case .EOF_TRAP, .EOL_TRAP, .RSH_UPDATE:
            
            fullRefresh()
            
        case .DISK_INSERT, .DISK_EJECT:
            
            fullRefresh()
            
        default:
            break
        }
    }
    
    func scrollToPC() {

        if cpuInfo != nil {
            scrollToPC(pc: Int(cpuInfo.pc0))
        }
    }

    func scrollToPC(pc: Int) {

        cpuInstrView.jumpTo(addr: pc)
    }

    func powerOn() {
    
        message.stringValue = ""
        fullRefresh()
    }

    func powerOff() {
    
        message.stringValue = ""
        fullRefresh()
    }

    func run() {
        
        message.stringValue = ""
        cpuInstrView.alertAddr = nil
        fullRefresh()
    }
    
    func pause() {
        
        fullRefresh()
    }

    func step() {

        message.stringValue = ""
        cpuInstrView.alertAddr = nil
        fullRefresh()
        scrollToPC()
    }
    
    func reset() {
        
        message.stringValue = ""
        cpuInstrView.alertAddr = nil
        fullRefresh()
    }

    /*
    func signalBreakPoint(pc: Int) {

        message.stringValue = String(format: "Breakpoint reached")
        cpuInstrView.alertAddr = nil
        scrollToPC(pc: pc)
    }

    func signalWatchPoint(pc: Int) {
    
        message.stringValue = String(format: "Watchpoint reached")
        cpuInstrView.alertAddr = pc
        scrollToPC(pc: pc)
    }
    */
    
    /*
    func signalGoto(pc: Int) {

        if isRunning { return }

        let addr = String(format: (hex ? "$%04X" : "%d"), pc)
        message.stringValue = String(format: "Program counter redirected to " + addr)
        fullRefresh()
        scrollToPC()
    }
    */
    
    @IBAction func refreshAction(_ sender: NSButton!) {

        refresh()
    }
}

extension Inspector {
    
    override func windowWillClose(_ notification: Notification) {
                
        // Unregister the inspector
        if let index = parent.inspectors.firstIndex(where: { $0 === self }) {
            
            parent.inspectors.remove(at: index)
        }

        // Leave debug mode if no more inspectors are open
        if parent.inspectors.isEmpty {

            // Leave debug mode
            emu?.trackOff()
            emu?.set(.MEM_HEATMAP, enable: false)
            emu?.c64.autoInspectionMask = 0
        }
    }
}

extension Inspector: NSTabViewDelegate {
    
    func updateInspectionTarget() {

        func mask(_ types: [Class]) -> Int {

            var result = 0
            for type in types { result = result | 1 << type.rawValue }
            return result
        }
        func mask(_ type: Class) -> Int { return mask([type]) }

        if let id = panel.selectedTabViewItem?.label {
                    
            switch id {
                
            case "CPU":     c64?.autoInspectionMask = mask([.CPU])
            case "Memory":  c64?.autoInspectionMask = mask([.Memory])
            case "Bus":     c64?.autoInspectionMask = mask([.VICII])
            case "CIA":     c64?.autoInspectionMask = mask([.CIA])
            case "VICII":   c64?.autoInspectionMask = mask([.VICII])
            case "SID":     c64?.autoInspectionMask = mask([.SID])
            case "Events":  c64?.autoInspectionMask = mask([.C64])

            default:
                break
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
