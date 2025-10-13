// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

extension MyController: NSMenuItemValidation {
    
    open func validateMenuItem(_ item: NSMenuItem) -> Bool {

        guard let emu = emu else { return true }

        let info = emu.info
        let powered = info.powered
        let running = info.running

        var driveID: Int { return item.tag }
        var drive: DriveProxy { return emu.drive(driveID) }

        func validateURLlist(_ list: [URL], image: NSImage) -> Bool {

            let slot = item.tag % 10

            if let url = MediaManager.getRecentlyUsedURL(slot, from: list) {
                item.title = url.lastPathComponent
                item.isHidden = false
                item.image = image
            } else {
                item.title = ""
                item.isHidden = true
                item.image = nil
            }

            return true
        }

        switch item.action {

            // Edit menu
        case #selector(MyController.grabMouseAction(_:)):
            item.title = metal.gotMouse ? "Release Mouse" : "Retain Mouse"
            return true

        case #selector(MyController.stopAndGoAction(_:)):
            item.title = running ? "Pause" : "Continue"
            return true

        case #selector(MyController.powerAction(_:)):
            item.title = powered ? "Power Off" : "Power On"
            return true

            // View menu
        case #selector(MyController.toggleStatusBarAction(_:)):
            item.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
            return true

            // Keyboard menu
        case #selector(MyController.mapCapsLockWarpAction(_:)):
            item.state = myAppDelegate.mapCapsLockWarp ? .on : .off
            return true
        case #selector(MyController.shiftLockAction(_:)):
            item.state = emu.keyboard.isPressed(.shiftLock) ? .on : .off
            return true

            // Drive menu
        case #selector(MyController.insertRecentDiskAction(_:)):
            return validateURLlist(MediaManager.insertedFloppyDisks, image: smallDisk)

        case #selector(MyController.ejectDiskAction(_:)),
            #selector(MyController.exportDiskAction(_:)),
            #selector(MyController.inspectDiskAction(_:)),
            #selector(MyController.inspectVolumeAction(_:)):
            return drive.info.hasDisk

        case #selector(MyController.exportRecentDiskDummyAction8(_:)):
            let empty = mm.getRecentlyExportedDiskURL(0, drive: DRIVE8) == nil
            return emu.drive8.info.hasDisk && !empty

        case #selector(MyController.exportRecentDiskDummyAction9(_:)):
            let empty = mm.getRecentlyExportedDiskURL(0, drive: DRIVE9) == nil
            return emu.drive9.info.hasDisk && !empty

        case #selector(MyController.exportRecentDiskAction(_:)):
            return validateURLlist(mm.exportedFloppyDisks[driveID], image: smallDisk)

        case #selector(MyController.writeProtectAction(_:)):
            item.state = drive.info.hasProtectedDisk ? .on : .off
            return drive.info.hasDisk

        case #selector(MyController.drivePowerAction(_:)):
            item.title = drive.config.switchedOn ? "Switch off" : "Switch on"
            return true

            // Tape menu
        case #selector(MyController.insertRecentTapeAction(_:)):
            return validateURLlist(MediaManager.insertedTapes, image: smallTape)

        case #selector(MyController.ejectTapeAction(_:)):
            return emu.datasette.info.hasTape

        case #selector(MyController.playOrStopAction(_:)):
            item.title = emu.datasette.info.playKey ? "Press Stop Key" : "Press Play On Tape"
            return emu.datasette.info.hasTape

        case #selector(MyController.rewindAction(_:)):
            return emu.datasette.info.hasTape

            // Cartridge menu
        case #selector(MyController.attachRecentCartridgeAction(_:)):
            return validateURLlist(MediaManager.attachedCartridges, image: smallCart)

        case #selector(MyController.attachReuDummyAction(_:)):
            item.state = (emu.expansionport.traits.type == .REU) ? .on : .off

        case #selector(MyController.attachReuAction(_:)):
            item.state = (emu.expansionport.traits.type == .REU &&
                          emu.expansionport.traits.memory == item.tag * 1024) ? .on : .off

        case #selector(MyController.attachGeoRamDummyAction(_:)):
            item.state = (emu.expansionport.traits.type == .GEO_RAM) ? .on : .off

        case #selector(MyController.attachGeoRamAction(_:)):
            item.state = (emu.expansionport.traits.type == .GEO_RAM &&
                          emu.expansionport.traits.memory == item.tag * 1024) ? .on : .off

        case #selector(MyController.attachIsepicAction(_:)):
            item.state = (emu.expansionport.traits.type == .ISEPIC) ? .on : .off

        case #selector(MyController.detachCartridgeAction(_:)):
            return emu.expansionport.cartridgeAttached()

        case #selector(MyController.inspectCartridgeAction(_:)):
            return emu.expansionport.cartridgeAttached()

        case #selector(MyController.pressButtonDummyAction(_:)):
            return emu.expansionport.traits.buttons > 0

        case #selector(MyController.pressCartridgeButton1Action(_:)):
            let title = String(charptr: emu.expansionport.traits.button1)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil

        case #selector(MyController.pressCartridgeButton2Action(_:)):
            let title = String(charptr: emu.expansionport.traits.button2)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil

        case #selector(MyController.setSwitchDummyAction(_:)):
            return emu.expansionport.traits.switches > 0

        case #selector(MyController.setSwitchNeutralAction(_:)):
            let title = String(charptr: emu.expansionport.traits.switchNeutral)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = emu.expansionport.info.switchPos == 0 ? .on : .off
            return title != nil

        case #selector(MyController.setSwitchLeftAction(_:)):
            let title = String(charptr: emu.expansionport.traits.switchLeft)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = emu.expansionport.info.switchPos < 0 ? .on : .off
            return title != nil

        case #selector(MyController.setSwitchRightAction(_:)):
            let title = String(charptr: emu.expansionport.traits.switchRight)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = emu.expansionport.info.switchPos > 0 ? .on : .off
            return title != nil

        default:
            return true
        }

        return true
    }

    func hideOrShowDriveMenus() {
        
        myAppDelegate.drive8Menu.isHidden = !config.drive8Connected
        myAppDelegate.drive9Menu.isHidden = !config.drive9Connected
        myAppDelegate.datasetteMenu.isHidden = !config.datasetteConnected
    }
    
    //
    // Action methods (App menu)
    //

    @IBAction func settingsAction(_ sender: Any?) {

        if myAppDelegate.settingsController == nil {

            let sb = NSStoryboard(name: "Settings", bundle: nil)
            let id = "SettingsWindowController"

            myAppDelegate.settingsController =
            sb.instantiateController(withIdentifier: id) as? SettingsWindowController
        }

        if let sc = myAppDelegate.settingsController {

            sc.window?.level = .floating
            sc.showWindow(self)
            sc.window?.makeKeyAndOrderFront(nil)
            NSApp.activate(ignoringOtherApps: true)
        }
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {

        let defaults = EmulatorProxy.defaults!

        // Power off the emulator if the user doesn't object
        if !askToPowerOff() { return }

        // Reset settings
        defaults.removeAll()
        defaults.resetSearchPaths()
        defaults.save()

        // Apply new settings
        config.applyUserDefaults()
        pref.applyUserDefaults()

        // Launch the onboarding agent
        renderer.onboarding.open(delay: 1.0)
    }

    @IBAction func importConfigAction(_ sender: Any!) {

        // let openPanel = NSOpenPanel()

        // Power off the emulator if the user doesn't object
        if !askToPowerOff() { return }

        myOpenPanel.configure(types: [ .retrosh ], prompt: "Import")
        myOpenPanel.open(for: window, { result in
            
            if result == .OK, let url = self.myOpenPanel.url {

                do {
                    try self.mm.mount(url: url, allowedTypes: [.SCRIPT])
                } catch {
                    self.showAlert(.cantOpen(url: url), error: error, async: true)
                }
            }
        })
        
        // Show file panel
        /*
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = true
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Import"
        openPanel.allowedContentTypes = [.ini]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in

            if result == .OK, let url = openPanel.url {

                do {
                    try self.mm.addMedia(url: url, allowedTypes: [.SCRIPT])
                } catch {
                    self.showAlert(.cantOpen(url: url), error: error, async: true)
                }
            }
        })
        */
    }

    @IBAction func exportConfigAction(_ sender: Any!) {

        let savePanel = NSSavePanel()

        // Show file panel
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.nameFieldStringValue = "virtualc64.ini"
        savePanel.canCreateDirectories = true
        savePanel.beginSheetModal(for: window!, completionHandler: { result in

            if result == .OK, let url = savePanel.url {

                do {
                    try self.emu?.exportConfig(url: url)
                } catch {
                    self.showAlert(.cantExport(url: url), error: error, async: true)
                }
            }
        })
    }

    //
    // Action methods (Machine menu)
    //
    
    func addInspector() {
    
        let count = inspectors.count
        
        // Allow 8 inspectors at a time
        if count < 8, let inspector = Inspector(with: self, nibName: "Inspector") {
            
            inspectors.append(inspector)
            inspector.showAsWindow()

        } else {
         
            NSSound.beep();
        }
    }
    
    @IBAction func inspectorAction(_ sender: Any!) {
        
        if inspectors.isEmpty {
            addInspector()
        } else {
            inspectors[0].showAsWindow()
        }
    }
    
    func addDashboard(type: PanelType = .Combined) {
    
        let count = dashboards.count
        
        // Allow 24 dashboards at a time
        if count < 24 {
            
            let myStoryboard = NSStoryboard(name: "Dashboard", bundle: nil)
            
            if let newDashboard = myStoryboard.instantiateController(withIdentifier: "MyWindowController") as? Dashboard {
                
                dashboards.append(newDashboard)
                newDashboard.setController(self)
                newDashboard.showWindow(self)
                newDashboard.viewController?.type = type
                return
            }
        }

        NSSound.beep();
    }
    
    @IBAction func dashboardAction(_ sender: Any!) {
        
        if dashboards.isEmpty {
            addDashboard()
        } else {
            dashboards[0].showWindow(self)
        }
    }
    
    @IBAction func consoleAction(_ sender: Any!) {
        
        if renderer.console.isVisible {
            renderer.console.close(delay: 0.25)
        } else {
            renderer.console.open(delay: 0.25)
        }
    }
    
    @IBAction func takeSnapshotAction(_ sender: Any!) {

        guard let snapshot = emu?.c64.takeSnapshot(pref.snapshotCompressor) else {

            NSSound.beep()
            return
        }
        if !mydocument.appendSnapshot(file: snapshot) {

            NSSound.beep()
            return
        }

        renderer.flash()
    }
    
    @IBAction func restoreSnapshotAction(_ sender: Any!) {
        
        do {
            try restoreLatestSnapshot()
        } catch {
            showAlert(.cantRestore, error: error)
        }
    }
    
    @IBAction func browseSnapshotsAction(_ sender: Any!) {
        
        if snapshotBrowser == nil {
            snapshotBrowser = SnapshotViewer(with: self, nibName: "SnapshotViewer")
        }
        snapshotBrowser?.showAsSheet()
    }
    
    @IBAction func takeScreenshotAction(_ sender: Any!) {

        // Take screenshot
        guard let screen = renderer.canvas.screenshot(source: pref.screenshotSource,
                                                      cutout: pref.screenshotCutout,
                                                      width: pref.screenshotWidth,
                                                      height: pref.screenshotHeight) else {

            warn("Failed to create screenshot")
            return
        }

        // Convert to Screenshot object
        let screenshot = Screenshot(screen: screen, format: pref.screenshotFormat)

        // Save to disk
        try? screenshot.save()

        // Save to disk
        try? screenshot.save()
        
        // Create a visual effect
        renderer.flash()
    }
    
    @IBAction func browseScreenshotsAction(_ sender: Any!) {

        if screenshotBrowser == nil {
            screenshotBrowser = ScreenshotViewer(with: self, nibName: "ScreenshotViewer")
        }
        screenshotBrowser?.showAsSheet()
    }

    /*
    @IBAction func captureScreenAction(_ sender: Any!) {

        if let emu = emu {

            if emu.recorder.recording {

                emu.recorder.stopRecording()
                exportVideoAction(self)
                return
            }

            if !emu.recorder.hasFFmpeg {

                if pref.ffmpegPath != "" {
                    showAlert(.noFFmpegFound(exec: pref.ffmpegPath))
                } else {
                    showAlert(.noFFmpegInstalled)
                }
                return
            }

            var rect: CGRect
            if pref.captureSource == 0 {
                rect = renderer.canvas.visible
            } else {
                rect = renderer.canvas.entire
            }

            do {
                try emu.recorder.startRecording(rect: rect)
            } catch {

                showAlert(.cantRecord, error: error)
            }
        }
    }
    
    @IBAction func exportVideoAction(_ sender: Any!) {

        let exporter = VideoExporter(with: self, nibName: "VideoExporter")
        exporter?.showAsSheet()
    }
    */

    //
    // Action methods (Edit menu)
    //
    
    @IBAction func paste(_ sender: Any!) {

        let pasteBoard = NSPasteboard.general
        guard let text = pasteBoard.string(forType: .string) else {
            warn("Cannot paste. No text in pasteboard")
            return
        }
        
        keyboard.type(text)
    }

    @IBAction func grabMouseAction(_ sender: Any!) {

        metal.gotMouse ? metal.releaseMouse() : metal.retainMouse()
    }
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if let emu = emu {
            if emu.running { emu.pause() }
        }
    }

    @IBAction func continueAction(_ sender: Any!) {
        
        if let emu = emu {
            if emu.paused { try? emu.run() }
        }
    }

    @IBAction func stopAndGoAction(_ sender: Any!) {
        
        if let emu = emu {
            if emu.running { emu.pause() } else { try? emu.run() }
        }
    }
    
    @IBAction func stepIntoAction(_ sender: Any!) {

        if let emu = emu {
            emu.stepInto()
        }
    }
    
    @IBAction func stepOverAction(_ sender: Any!) {
        
        if let emu = emu {
            emu.stepOver()
        }
    }
    
    @IBAction func resetAction(_ sender: Any!) {

        if let emu = emu {

            renderer.rotateLeft()
            emu.hardReset()
            try? emu.run()
        }
    }

    @IBAction func softResetAction(_ sender: Any!) {

        if let emu = emu {
            emu.softReset()
        }
    }

    @IBAction func powerAction(_ sender: Any!) {

        if let emu = emu {

            if emu.poweredOn {

                emu.powerOff()

            } else {

                do {
                    try emu.run()
                } catch {
                    showAlert(.cantRun, error: error)
                }
            }
        }
    }
     
    @IBAction func brkAction(_ sender: Any!) {

        if let emu = emu {
            emu.put(.CPU_BRK)
        }
    }

    //
    // Action methods (View menu)
    //

    @IBAction func toggleStatusBarAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.toggleStatusBarAction(sender)
        }
        
        showStatusBar(!statusBar)
    }

    //
    // Action methods (Keyboard menu)
    //
    
    @IBAction func stickyKeyboardAction(_ sender: Any!) {
        
        if virtualKeyboard == nil {
            virtualKeyboard = VirtualKeyboardController.make(parent: self)
        }
        if virtualKeyboard?.window?.isVisible == true {
            debug(.lifetime, "Virtual keyboard already open")
        } else {
            debug(.lifetime, "Opeining virtual keyboard as a window")
        }

        virtualKeyboard?.showAsWindow()
    }

    /*
    @IBAction func mapLeftCmdKeyAction(_ sender: NSMenuItem!) {

        let s = sender.state
        let tag = sender.tag
        print("State: \(s) Tag: \(tag)")

        myAppDelegate.mapLeftCmdKey = sender.state == .off ? C64Key(sender.tag) : nil
        refreshStatusBar()
    }

    @IBAction func mapRightCmdKeyAction(_ sender: NSMenuItem!) {

        let s = sender.state
        let tag = sender.tag
        print("State: \(s) Tag: \(tag)")

        myAppDelegate.mapRightCmdKey = sender.state == .off ? C64Key(sender.tag) : nil
        refreshStatusBar()
    }
    */

    @IBAction func mapCapsLockWarpAction(_ sender: NSMenuItem!) {

        myAppDelegate.mapCapsLockWarp = !myAppDelegate.mapCapsLockWarp
        refreshStatusBar()
    }

    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        if let emu = emu {
            emu.keyboard.releaseAll()
        }
    }

    // -----------------------------------------------------------------
    @IBAction func pressAction(_ sender: NSMenuItem!) {
     
        keyboard.pressKey(key: C64Key(sender.tag), duration: 0.08)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func pressWithShiftAction(_ sender: NSMenuItem!) {
        
        keyboard.pressKeyCombination(key1: C64Key(sender.tag), key2: C64Key.shift, duration: 0.08)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func pressRunstopRestoreAction(_ sender: Any!) {

        keyboard.pressKeyCombination(key1: C64Key.runStop, key2: C64Key.restore, duration: 0.08)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func runstopAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.runStop, duration: 0.08)
    }
    
    @IBAction func restoreAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.restore, duration: 0.08)
    }
    
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.commodore, duration: 0.08)
    }
    
    @IBAction func clearKeyAction(_ sender: Any!) {
        keyboard.pressKeyCombination(key1: C64Key.home, key2: C64Key.shift, duration: 0.08)
    }
    
    @IBAction func homeKeyAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.home, duration: 0.08)
    }
    
    @IBAction func insertKeyAction(_ sender: Any!) {
        keyboard.pressKeyCombination(key1: C64Key.delete, key2: C64Key.shift, duration: 0.08)
    }
    
    @IBAction func deleteKeyAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.delete, duration: 0.08)
    }
    
    @IBAction func leftarrowKeyAction(_ sender: Any!) {
        keyboard.pressKey(key: C64Key.leftArrow, duration: 0.08)
    }
    
    @IBAction func shiftLockAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.shiftLockAction(sender)
        }
        keyboard.toggleKey(key: C64Key.shiftLock)
    }

    // -----------------------------------------------------------------
    @IBAction func loadDirectoryAction(_ sender: Any!) {
        keyboard.type("load \"$\",8:\n")
    }
    @IBAction func listAction(_ sender: Any!) {
        keyboard.type("list:\n")
    }
    @IBAction func loadFirstFileAction(_ sender: Any!) {
        keyboard.type("load \"*\",8,1:\n")
    }
    @IBAction func runProgramAction(_ sender: Any!) {
        keyboard.type("run:")
    }
    @IBAction func formatDiskAction(_ sender: Any!) {
        keyboard.type("open 1,8,15,\"n:test, id\": close 1\n:")
    }
    
    //
    // Action methods (Drive menu)
    //

    @IBAction func newDiskAction(_ sender: NSMenuItem!) {

        if let emu = emu {

            let drive = emu.drive(sender)

            // Ask the user if a modified hard drive should be detached
            if !proceedWithUnsavedFloppyDisk(drive: drive) { return }

            let panel = DiskCreator(with: self, nibName: "DiskCreator")
            panel?.showSheet(forDrive: drive.info.id)

            mm.clearRecentlyExportedDiskURLs(drive: drive.info.id)
        }
    }
    
    @IBAction func insertDiskAction(_ sender: NSMenuItem!) {
        
        if let emu = emu {

            let id = sender.tag
            let drive = emu.drive(sender)

            // Ask user to continue if the current disk contains modified data
            if !proceedWithUnsavedFloppyDisk(drive: drive) { return }

            myOpenPanel.configure(types: [ .t64, .prg, .p00, .d64, .g64, .zip, .gzip ], prompt: "Insert")
            myOpenPanel.open(for: window, { result in

                if result == .OK, let url = self.myOpenPanel.url {

                    do {
                        try self.mm.mount(url: url,
                                             allowedTypes: [ .D64, .T64, .PRG, .P00, .G64 ],
                                             drive: id,
                                             options: [.force, .remember])

                    } catch {
                        self.showAlert(.cantInsert, error: error, async: true)
                    }
                }
            })
        }
    }
    
    @IBAction func insertRecentDiskAction(_ sender: NSMenuItem!) {
        
        let drive = sender.tag < 10 ? DRIVE8 : DRIVE9
        let slot  = sender.tag % 10

        insertRecentDiskAction(drive: drive, slot: slot)
    }

    func insertRecentDiskAction(drive: Int, slot: Int) {

        let types: [vc64.FileType] = [ .D64, .T64, .PRG, .P00, .G64 ]

        if let url = MediaManager.getRecentlyInsertedDiskURL(slot) {

            do {
                try self.mm.mount(url: url, allowedTypes: types, drive: drive)
            } catch {
                self.showAlert(.cantInsert, error: error)
            }
        }
    }

    @IBAction func exportRecentDiskDummyAction8(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction9(_ sender: NSMenuItem!) {}

    @IBAction func exportRecentDiskAction(_ sender: NSMenuItem!) {
                
        let drive = sender.tag < 10 ? DRIVE8 : DRIVE9
        let slot = sender.tag % 10
        
        exportRecentDiskAction(drive: drive, slot: slot)
    }
    
    func exportRecentDiskAction(drive id: Int, slot: Int) {
                
        if let url = mm.getRecentlyExportedDiskURL(slot, drive: id) {
            
            do {
                try mm.export(drive: id, to: url)
            } catch {
                showAlert(.cantExport(url: url), error: error)
            }
        }
    }
    
    @IBAction func clearRecentlyInsertedDisksAction(_ sender: Any!) {
        MediaManager.insertedFloppyDisks = []
    }

    @IBAction func clearRecentlyExportedDisksAction(_ sender: NSMenuItem!) {

        let drive = sender.tag
        mm.clearRecentlyExportedDiskURLs(drive: drive)
    }

    @IBAction func clearRecentlyInsertedTapesAction(_ sender: Any!) {
        MediaManager.insertedTapes = []
    }
    
    @IBAction func clearRecentlyAttachedCartridgesAction(_ sender: Any!) {
        MediaManager.attachedCartridges = []
    }
    
    @IBAction func ejectDiskAction(_ sender: NSMenuItem!) {

        ejectDiskAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    func ejectDiskAction(drive nr: Int) {

        if let emu = emu {

            let drive = emu.drive(nr)

            if proceedWithUnsavedFloppyDisk(drive: drive) {

                drive.ejectDisk()
                mm.clearRecentlyExportedDiskURLs(drive: nr)
            }
        }
    }

    @IBAction func exportDiskAction(_ sender: NSMenuItem!) {

        exportDiskAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    func exportDiskAction(drive nr: Int) {

        let nibName = NSNib.Name("DiskExporter")
        let exportPanel = DiskExporter(with: self, nibName: nibName)
        exportPanel?.showSheet(diskDrive: nr)
    }

    @IBAction func inspectDiskAction(_ sender: NSMenuItem!) {

        inspectDiskAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    func inspectDiskAction(drive nr: Int) {

        do {

            let panel = DiskInspector(with: self, nibName: "DiskInspector")
            try panel?.show(diskDrive: nr)

        } catch {

            showAlert(.cantDecode, error: error, window: window)
        }
    }

    @IBAction func inspectVolumeAction(_ sender: NSMenuItem!) {

        inspectVolumeAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    func inspectVolumeAction(drive nr: Int) {

        do {

            let panel = VolumeInspector(with: self, nibName: "VolumeInspector")
            try panel?.show(diskDrive: nr)

        } catch {

            showAlert(.cantDecode, error: error, window: window)
        }
    }

    @IBAction func writeProtectAction(_ sender: NSMenuItem!) {

        writeProtectAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    func writeProtectAction(drive nr: Int) {

        if let emu = emu {

            precondition(nr == DRIVE8 || nr == DRIVE9)
            emu.put(.DSK_TOGGLE_WP, value: nr)
        }
    }

    @IBAction func drivePowerAction(_ sender: NSMenuItem!) {
        
        drivePowerAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }
    
    func drivePowerAction(drive nr: Int) {
                
        switch nr {

        case DRIVE8: config.drive8PowerSwitch = !config.drive8PowerSwitch
        case DRIVE9: config.drive9PowerSwitch = !config.drive9PowerSwitch

        default:
            fatalError()
        }         
    }
        
    //
    // Action methods (Datasette menu)
    //
    
    @IBAction func insertTapeAction(_ sender: Any!) {
        
        myOpenPanel.configure(types: [ .tap, .zip, .gzip ], prompt: "Insert")
        myOpenPanel.open(for: window, { result in

            if result == .OK, let url = self.myOpenPanel.url {
                self.insertTapeAction(from: url)
            }
        })
    }
    
    @IBAction func insertRecentTapeAction(_ sender: NSMenuItem!) {
        
        let slot  = sender.tag
        
        if let url = MediaManager.getRecentlyInsertedTapeURL(slot) {
            insertTapeAction(from: url)
        }
    }
    
    func insertTapeAction(from url: URL) {

        do {
            try mm.mount(url: url, allowedTypes: [ .TAP ])
        } catch {
            self.showAlert(.cantInsertTape, error: error)
        }
    }

    @IBAction func ejectTapeAction(_ sender: Any!) {

        if let emu = emu {

            emu.datasette.ejectTape()
        }
    }
    
    @IBAction func playOrStopAction(_ sender: Any!) {

        if let emu = emu {

            if emu.datasette.info.playKey {
                emu.datasette.pressStop()
            } else {
                emu.datasette.pressPlay()
            }
        }
    }
    
    @IBAction func rewindAction(_ sender: Any!) {

        if let emu = emu {
            emu.datasette.rewind()
        }
    }

    //
    // Action methods (Cartridge menu)
    //

    @IBAction func attachCartridgeAction(_ sender: Any!) {
        
        myOpenPanel.configure(types: [ .crt, .zip, .gzip ], prompt: "Insert")
        myOpenPanel.open(for: window, { result in

            if result == .OK, let url = self.myOpenPanel.url {
                
                do {
                    try self.mm.mount(url: url, allowedTypes: [ .CRT ])
                } catch {
                    self.showAlert(.cantAttach, error: error, async: true)
                }
            }
        })        
    }
    
    @IBAction func attachRecentCartridgeAction(_ sender: NSMenuItem!) {
        
        let slot  = sender.tag
        
        if let url = MediaManager.getRecentlyAtachedCartridgeURL(slot) {

            do {
                try self.mm.mount(url: url, allowedTypes: [ .CRT ])
            } catch {
                self.showAlert(.cantAttach, error: error, async: true)
            }
        }
    }

    @IBAction func detachCartridgeAction(_ sender: Any!) {

        if let emu = emu {

            emu.expansionport.detachCartridge()
            emu.hardReset()
        }
    }

    @IBAction func attachReuDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachReuAction(_ sender: NSMenuItem!) {

        if let emu = emu {

            let capacity = sender.tag
            emu.expansionport.attachReuCartridge(capacity)
        }
    }

    @IBAction func attachGeoRamDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachGeoRamAction(_ sender: NSMenuItem!) {

        if let emu = emu {

            let capacity = sender.tag
            emu.expansionport.attachGeoRamCartridge(capacity)
        }
    }

    @IBAction func attachIsepicAction(_ sender: Any!) {

        if let emu = emu {
            emu.expansionport.attachIsepicCartridge()
        }
    }

    @IBAction func pressCartridgeButton1Action(_ sender: NSButton!) {
        
        if let emu = emu {

            emu.put(.CRT_BUTTON_PRESS, value: 1)

            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                self.emu!.put(.CRT_BUTTON_RELEASE, value: 1)
            }
        }
    }

    @IBAction func pressCartridgeButton2Action(_ sender: NSButton!) {
        
        if let emu = emu {

            emu.put(.CRT_BUTTON_PRESS, value: 2)

            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                self.emu!.put(.CRT_BUTTON_RELEASE, value: 2)
            }
        }
    }
    
    @IBAction func pressButtonDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func setSwitchNeutralAction(_ sender: Any!) {
        
        if let emu = emu {
            emu.put(.CRT_SWITCH_NEUTRAL)
        }
    }

    @IBAction func setSwitchLeftAction(_ sender: Any!) {
        
        if let emu = emu {
            emu.put(.CRT_SWITCH_LEFT)
        }
    }

    @IBAction func setSwitchRightAction(_ sender: Any!) {
        
        if let emu = emu {
            emu.put(.CRT_SWITCH_RIGHT)
        }
    }

    @IBAction func setSwitchDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction  func inspectCartridgeAction(_ sender: Any!) {

        if let emu = emu {

            let panel = CartridgeInspector(with: self, nibName: "CartridgeInspector")
            panel?.show(expansionPort: emu.expansionport)
        }
    }

    //
    // Action methods (Window menu)
    //

    // Resizes the window such that every texture line hits a display line
    @IBAction func autoResizeWindow(_ sender: NSMenuItem!) {

        let height = renderer.canvas.visible.height * 2

        debug(.metal, "Old metal view: \(metal.frame)")
        debug(.metal, "Visible texture lines: \(height)")

        adjustWindowSize(height: height)

        debug(.metal, "New metal view: \(metal.frame)")
    }
}
