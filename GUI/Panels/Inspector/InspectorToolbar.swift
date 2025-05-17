// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class InspectorToolbar: NSToolbar {
    
    @IBOutlet weak var inspector: Inspector!
    @IBOutlet weak var timeStamp: NSButton!
    @IBOutlet weak var execSegCtrl: NSSegmentedControl!
    @IBOutlet weak var selectorPopup: NSPopUpButton!
    @IBOutlet weak var selectorToolbarItem: NSToolbarItem!
    @IBOutlet weak var formatPopup: NSPopUpButton!
    @IBOutlet weak var formatToolbarItem: NSToolbarItem!

    var emu: EmulatorProxy! { return inspector.parent.emu }
    
    override func validateVisibleItems() {

        // Update icons
        let running = emu.running
        let label = running ? "Pause" : "Run"
        let image = running ? "pauseTemplate" : "runTemplate"
        execSegCtrl.setToolTip(label, forSegment: 0)
        execSegCtrl.setImage(NSImage(named: image), forSegment: 0)
        for i in 1...5 { execSegCtrl.setEnabled(!running, forSegment: i) }
        
        // Disable shrinked popup buttons to prevent macOS from crashing
        selectorPopup.isEnabled = selectorToolbarItem.isVisible
        formatPopup.isEnabled = formatToolbarItem.isVisible
    }
    
    func updateToolbar(full: Bool) {
        
        let info = emu.c64.info
        
        let frame = info.frame
        let vpos = info.vpos
        let hpos = info.hpos
        
        if full {
                    
            timeStamp.font = NSFont.monospacedSystemFont(ofSize: 12, weight: .regular)
        }
        
        var format = ""
        if inspector.hex {
            format = inspector.padding ? "%X:%03X:%03X" : "%X:%X:%X"
        } else {
            format = inspector.padding ? "%d:%03d:%03d" : "%d:%d:%d"
        }
        timeStamp.title = String(format: format, frame, vpos, hpos)
    }
    
    //
    // Action methods
    //
    
    @IBAction
    func panelAction(_ sender: Any) {

        if let popup = sender as? NSPopUpButton {
            inspector.selectPanel(popup.selectedTag())
        }
    }
 
    @IBAction
    func formatAction(_ sender: Any) {

        if let popup = sender as? NSPopUpButton {
            inspector.format = popup.selectedTag()
        }
    }

    @IBAction
    func execAction(_ sender: NSSegmentedControl) {
        
        switch sender.selectedSegment {
            
        case 0: emu.running ? emu.pause() : try? emu.run()
        case 1: emu.stepOver()
        case 2: emu.stepInto()
        case 3: emu.stepCycle()
        case 4: emu.finishLine()
        case 5: emu.finishFrame()
            
        default:
            fatalError()
        }
    }
    
    @IBAction
    func plusAction(_ sender: NSButton) {

        inspector.parent.addInspector()
    }
}
