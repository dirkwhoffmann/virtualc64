// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSToolbarItem.Identifier {

    static let progress = NSToolbarItem.Identifier("Inspector.Progress")
    static let execution = NSToolbarItem.Identifier("Inspector.Execution")
    static let target = NSToolbarItem.Identifier("Inspector.Target")
    static let format = NSToolbarItem.Identifier("Inspector.Format")
    static let newInspector = NSToolbarItem.Identifier("Inspector.New")
}

@MainActor
class InspectorToolbar: NSToolbar, NSToolbarDelegate {

    var inspector: Inspector!
    var controller: MyController { inspector.parent }
    var emu: EmulatorProxy! { controller.emu! }

    var progress: MyToolbarItem!
    var execution: MyToolbarItemGroup!
    var target: MyToolbarMenuItem!
    var format: MyToolbarMenuItem!
    var newInspector: MyToolbarItem!

    // Set to true to gray out all toolbar items
    var globalDisable = false

    init() {

        super.init(identifier: "InspectorToolbar")
        self.delegate = self
        self.allowsUserCustomization = true
        self.displayMode = .iconOnly
    }

    override init(identifier: NSToolbar.Identifier) {

        super.init(identifier: identifier)
        self.delegate = self
        self.allowsUserCustomization = true
        self.displayMode = .iconOnly
    }

    convenience init(inspector: Inspector) {

        self.init(identifier: "InspectorToolbar")
        self.inspector = inspector
    }

    func toolbarAllowedItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .progress,
                 .execution,
                 .target,
                 .format,
                 .newInspector,
                 .flexibleSpace ]
    }

    func toolbarDefaultItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {

        return [ .progress,
                 .flexibleSpace,
                 .execution,
                 .flexibleSpace,
                 .target,
                 .format,
                 .newInspector ]
    }

    func toolbar(_ toolbar: NSToolbar,
                 itemForItemIdentifier itemIdentifier: NSToolbarItem.Identifier,
                 willBeInsertedIntoToolbar flag: Bool) -> NSToolbarItem? {

        switch itemIdentifier {

        case .progress:

            progress = MyToolbarItem(identifier: .progress,
                                     image: nil,
                                     action: nil,
                                     target: self,
                                     size: CGSize(width: 128, height: 24),
                                     label: "Progress")
            return progress


        case .execution:

            let images: [SFSymbol] = [

                .pause,
                .stepInto,
                .stepOver,
                .stepCycle,
                .stepLine,
                .stepFrame
            ]

            let actions: [Selector] = [

                #selector(pauseAction),
                #selector(stepIntoAction),
                #selector(stepOverAction),
                #selector(stepCycleAction),
                #selector(finishLineAction),
                #selector(finishFrameAction)
            ]

            execution = MyToolbarItemGroup(identifier: .execution,
                                           images: images,
                                           actions: actions,
                                           target: self,
                                           label: "Execution")
            return execution

        case .target:

            let menuItems = [ (SFSymbol.chip, "CPU", 0),
                              (SFSymbol.chip, "Memory", 1),
                              (SFSymbol.bus, "Bus", 2),
                              (SFSymbol.chip, "CIAs", 3),
                              (SFSymbol.chip, "VICII", 4),
                              (SFSymbol.chip, "SID", 5),
                              (SFSymbol.alarm, "Events", 6) ]

            target = MyToolbarMenuItem(identifier: .target,
                                       menuItems: menuItems,
                                       image: .magnifyingglass,
                                       action: #selector(targetAction(_:)),
                                       target: self,
                                       label: "Target")
            return target

        case .format:

            let menuItems = [ (SFSymbol.bulletlist, "Hexadecimal", 0),
                              (SFSymbol.bulletlist, "Hexadecimal, zero padded", 1),
                              (SFSymbol.bulletlist, "Decimal", 2),
                              (SFSymbol.bulletlist, "Decimal, zero padded", 3) ]

            format = MyToolbarMenuItem(identifier: .format,
                                       menuItems: menuItems,
                                       image: .bulletlist,
                                       action: #selector(formatAction(_:)),
                                       target: self,
                                       label: "Format")
            return format

        case .newInspector:

            newInspector = MyToolbarItem(identifier: .newInspector,
                                         image: .plus,
                                         action: #selector(plusAction),
                                         target: self,
                                         label: "New")
            return newInspector

        default:
            return nil
        }
    }

    override func validateVisibleItems() {

    }

    /*
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
     */

    func updateToolbar(full: Bool) {

        let info = emu.c64.info

        let frame = info.frame
        let vpos = info.vpos
        let hpos = info.hpos

        if full {

            progress.button(at: 0)?.font = NSFont.monospacedSystemFont(ofSize: 14, weight: .regular)

            // Take care of the global disable flag
            for item in items { item.isEnabled = !globalDisable }

            if emu.running {

                execution.setToolTip("Pause", forSegment: 0)
                execution.setImage(SFSymbol.get(.pause), forSegment: 0)

            } else {

                execution.setToolTip("Run", forSegment: 0)
                execution.setImage(SFSymbol.get(.play), forSegment: 0)
            }
        }

        var format = ""
        if inspector.hex {
            format = inspector.padding ? "%X:%03X:%03X" : "%X:%X:%X"
        } else {
            format = inspector.padding ? "%d:%03d:%03d" : "%d:%d:%d"
        }
        progress.button(at: 0)?.title = String(format: format, frame, vpos, hpos)
    }

    //
    // Action methods
    //

    @objc private func execAction(_ sender: NSSegmentedControl) {

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

    @objc private func pauseAction() {

        emu.running ? emu.pause() : try? emu.run()
    }

    @objc private func stepOverAction() {

        emu.stepOver()
    }

    @objc private func stepIntoAction() {

        emu.stepInto()
    }

    @objc private func stepCycleAction() {

        emu.stepCycle()
    }

    @objc private func finishLineAction() {

        emu.finishLine()
    }

    @objc private func finishFrameAction() {

        emu.finishFrame()
    }

    @objc private func targetAction(_ sender: NSMenuItem) {

        inspector.selectPanel(sender.tag)
    }

    @objc private func formatAction(_ sender: NSMenuItem) {

        inspector.format = sender.tag
    }

    @objc private func plusAction(_ sender: NSButton) {

        inspector.parent.addInspector()
    }
}
