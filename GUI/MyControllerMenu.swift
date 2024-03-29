// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController: NSMenuItemValidation {
    
    open func validateMenuItem(_ item: NSMenuItem) -> Bool {

        let powered = c64.poweredOn
        let running = c64.running
        var recording: Bool { return c64.recorder.recording }
        
        var driveID: Int { return item.tag }
        var drive: DriveProxy { return c64.drive(driveID) }
        
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
            
        // Machine menu
        case #selector(MyController.captureScreenAction(_:)):
            item.title = recording ? "Stop Recording" : "Record Screen"
            return true
        
        // Edit menu
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
        case #selector(MyController.mapLeftCmdKeyAction(_:)):
            item.state = myAppDelegate.mapLeftCmdKey?.nr == item.tag ? .on : .off
            return true
        case #selector(MyController.mapRightCmdKeyAction(_:)):
            print("item.tag = \(item.tag)")
            item.state = myAppDelegate.mapRightCmdKey?.nr == item.tag ? .on : .off
            return true
        case #selector(MyController.mapCapsLockWarpAction(_:)):
            item.state = myAppDelegate.mapCapsLockWarp ? .on : .off
            return true
        case #selector(MyController.shiftLockAction(_:)):
            item.state = c64.keyboard.shiftLockIsPressed() ? .on : .off
            return true
            
        // Drive menu
        case #selector(MyController.insertRecentDiskAction(_:)):
            return validateURLlist(MediaManager.insertedFloppyDisks, image: smallDisk)

        case  #selector(MyController.ejectDiskAction(_:)),
            #selector(MyController.exportDiskAction(_:)),
            #selector(MyController.inspectDiskAction(_:)),
            #selector(MyController.inspectVolumeAction(_:)):
            return drive.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction8(_:)):
            return c64.drive8.hasDisk
            
        case #selector(MyController.exportRecentDiskDummyAction9(_:)):
            return c64.drive9.hasDisk
                        
        case #selector(MyController.exportRecentDiskAction(_:)):
            return validateURLlist(mm.exportedFloppyDisks[driveID], image: smallDisk)
            
        case #selector(MyController.writeProtectAction(_:)):
            item.state = drive.hasProtectedDisk ? .on : .off
            return drive.hasDisk
            
        case #selector(MyController.drivePowerAction(_:)):
            item.title = drive.isSwitchedOn() ? "Switch off" : "Switch on"
            return true
            
        // Tape menu
        case #selector(MyController.insertRecentTapeAction(_:)):
            return validateURLlist(MediaManager.insertedTapes, image: smallTape)
            
        case #selector(MyController.ejectTapeAction(_:)):
            return c64.datasette.hasTape
            
        case #selector(MyController.playOrStopAction(_:)):
            item.title = c64.datasette.playKey ? "Press Stop Key" : "Press Play On Tape"
            return c64.datasette.hasTape
            
        case #selector(MyController.rewindAction(_:)):
            return c64.datasette.hasTape
            
        // Cartridge menu
        case #selector(MyController.attachRecentCartridgeAction(_:)):
            return validateURLlist(MediaManager.attachedCartridges, image: smallCart)

        case #selector(MyController.attachReuDummyAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == .REU) ? .on : .off

        case #selector(MyController.attachReuAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == .REU &&
                          c64.expansionport.ramCapacity == item.tag * 1024) ? .on : .off

        case #selector(MyController.reuBatteryAction(_:)):
            item.state = c64.expansionport.hasBattery() ? .on : .off
            return c64.expansionport.cartridgeType() == .REU

        case #selector(MyController.attachGeoRamDummyAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == .GEO_RAM) ? .on : .off
            
        case #selector(MyController.attachGeoRamAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == .GEO_RAM &&
                            c64.expansionport.ramCapacity == item.tag * 1024) ? .on : .off
            
        case #selector(MyController.geoRamBatteryAction(_:)):
            item.state = c64.expansionport.hasBattery() ? .on : .off
            return c64.expansionport.cartridgeType() == .GEO_RAM

        case #selector(MyController.attachIsepicAction(_:)):
            item.state = (c64.expansionport.cartridgeType() == .ISEPIC) ? .on : .off
            
        case #selector(MyController.detachCartridgeAction(_:)):
            return c64.expansionport.cartridgeAttached()

        case #selector(MyController.inspectCartridgeAction(_:)):
            return c64.expansionport.cartridgeAttached()

        case #selector(MyController.pressButtonDummyAction(_:)):
            return c64.expansionport.numButtons() > 0
            
        case #selector(MyController.pressCartridgeButton1Action(_:)):
            let title = c64.expansionport.getButtonTitle(1)!
            item.title = title
            item.isHidden = title == ""
            return title != ""
            
        case #selector(MyController.pressCartridgeButton2Action(_:)):
            let title = c64.expansionport.getButtonTitle(2)!
            item.title = title
            item.isHidden = title == ""
            return title != ""
            
        case #selector(MyController.setSwitchDummyAction(_:)):
            return c64.expansionport.hasSwitch
            
        case #selector(MyController.setSwitchNeutralAction(_:)):
            let title = c64.expansionport.switchDescription(0)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsNeutral() ? .on : .off
            return title != nil
            
        case #selector(MyController.setSwitchLeftAction(_:)):
            let title = c64.expansionport.switchDescription(-1)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsLeft() ? .on : .off
            return title != nil
            
        case #selector(MyController.setSwitchRightAction(_:)):
            let title = c64.expansionport.switchDescription(1)
            item.title = title ?? ""
            item.isHidden = title == nil
            item.state = c64.expansionport.switchIsRight() ? .on : .off
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
    
    @IBAction func preferencesAction(_ sender: Any!) {
        
        if myAppDelegate.prefController == nil {
            myAppDelegate.prefController =
            PreferencesController(with: self, nibName: "Preferences")
        }
        myAppDelegate.prefController?.showWindow(self)
    }
    
    @IBAction func factorySettingsAction(_ sender: Any!) {

        let defaults = C64Proxy.defaults!

        // Power off the emulator if the user doesn't object
        if !askToPowerOff() { return }

        // Reset settings
        defaults.removeAll()
        defaults.resetSearchPaths()
        defaults.save()

        // Apply new settings
        config.applyUserDefaults()
        pref.applyUserDefaults()

        // Relaunch the emulator
        try? c64.run()
    }

    //
    // Action methods (File menu)
    //
    
    func openConfigurator(tab: String = "") {
        
        if configurator == nil {
            configurator = ConfigurationController(with: self, nibName: "Configuration")
        }
        configurator?.showSheet(tab: tab)
    }
    
    @IBAction func configureAction(_ sender: Any!) {
        
        openConfigurator()
    }

    @IBAction func inspectorAction(_ sender: Any!) {
        
        if inspector == nil {
            inspector = Inspector(with: self, nibName: "Inspector")
        }
        inspector?.showWindow(self)
    }
    
    @IBAction func monitorAction(_ sender: Any!) {
        
        if monitor == nil {
            monitor = Monitor(with: self, nibName: "Monitor")
        }
        monitor?.showWindow(self)
    }
    
    @IBAction func consoleAction(_ sender: Any!) {
        
        if renderer.console.isVisible {
            renderer.console.close(delay: 0.25)
        } else {
            renderer.console.open(delay: 0.25)
        }
    }
    
    @IBAction func takeSnapshotAction(_ sender: Any!) {
        
        c64.requestUserSnapshot()
    }
    
    @IBAction func restoreSnapshotAction(_ sender: Any!) {
        
        do {
            try restoreLatestSnapshot()
        } catch {
            NSSound.beep()
        }
    }
    
    @IBAction func browseSnapshotsAction(_ sender: Any!) {
        
        if snapshotBrowser == nil {
            snapshotBrowser = SnapshotViewer(with: self, nibName: "SnapshotViewer")
        }
        snapshotBrowser?.showSheet()
    }
    
    @IBAction func takeScreenshotAction(_ sender: Any!) {

        // Determine screenshot format
        let format = ScreenshotSource(rawValue: pref.screenshotSource)!
        
        // Take screenshot
        guard let screen = renderer.canvas.screenshot(source: format) else {

            warn("Failed to create screenshot")
            return
        }

        // Convert to Screenshot object
        let screenshot = Screenshot(screen: screen, format: pref.screenshotTarget)

        // Save to disk
        try? screenshot.save()
        
        // Create a visual effect
        renderer.flash()
    }
    
    @IBAction func browseScreenshotsAction(_ sender: Any!) {

        if screenshotBrowser == nil {
            screenshotBrowser = ScreenshotViewer(with: self, nibName: "ScreenshotViewer")
        }
        screenshotBrowser?.showSheet()
    }

    @IBAction func captureScreenAction(_ sender: Any!) {

        if c64.recorder.recording {
            
            c64.recorder.stopRecording()
            exportVideoAction(self)
            return
        }

        if !c64.recorder.hasFFmpeg {

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
            try c64.recorder.startRecording(rect: rect,
                                            rate: pref.bitRate,
                                            ax: pref.aspectX,
                                            ay: pref.aspectY)
        } catch {

            showAlert(.cantRecord, error: error)
        }
    }
    
    @IBAction func exportVideoAction(_ sender: Any!) {

        let exporter = VideoExporter(with: self, nibName: "VideoExporter")
        exporter?.showSheet()
    }
    
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
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.running { c64.pause() }
    }

    @IBAction func continueAction(_ sender: Any!) {
        
        if c64.paused { try? c64.run() }
    }

    @IBAction func stopAndGoAction(_ sender: Any!) {
        
        c64.stopAndGo()
    }
    
    @IBAction func stepIntoAction(_ sender: Any!) {

        c64.stepInto()
    }
    
    @IBAction func stepOverAction(_ sender: Any!) {
        
        c64.stepOver()
    }
    
    @IBAction func resetAction(_ sender: Any!) {

        renderer.rotateLeft()
        c64.hardReset()
        try? c64.run()
    }

    @IBAction func softResetAction(_ sender: Any!) {

        c64.softReset()
    }

    @IBAction func powerAction(_ sender: Any!) {

        if c64.poweredOn {

            c64.powerOff()

        } else {

            do {
                try c64.run()
            } catch {
                showAlert(.cantRun, error: error)
            }
        }
    }
     
    @IBAction func brkAction(_ sender: Any!) {

        c64.signalBrk()
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
            let name = NSNib.Name("VirtualKeyboard")
            virtualKeyboard = VirtualKeyboardController(with: self, nibName: name)
        }
        if virtualKeyboard?.window?.isVisible == true {
            debug(.lifetime, "Virtual keyboard already open")
        } else {
            debug(.lifetime, "Opeining virtual keyboard as a window")
        }

        virtualKeyboard?.showWindow()
    }

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

    @IBAction func mapCapsLockWarpAction(_ sender: NSMenuItem!) {

        myAppDelegate.mapCapsLockWarp = !myAppDelegate.mapCapsLockWarp
        refreshStatusBar()
    }

    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        c64.keyboard.releaseAll()
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
        c64.keyboard.toggleShiftLock()
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

        let drive = c64.drive(sender)

        // Ask the user if a modified hard drive should be detached
        if !proceedWithUnsavedFloppyDisk(drive: drive) { return }

        let panel = DiskCreator(with: self, nibName: "DiskCreator")
        panel?.showSheet(forDrive: drive.id)

        mm.clearRecentlyExportedDiskURLs(drive: drive.id)
    }
    
    @IBAction func insertDiskAction(_ sender: NSMenuItem!) {
        
        let id = sender.tag
        let drive = c64.drive(sender)

        // Ask user to continue if the current disk contains modified data
        if !proceedWithUnsavedFloppyDisk(drive: drive) { return }
        
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["t64", "prg", "p00", "d64", "g64", "zip", "gz"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in

            if result == .OK, let url = openPanel.url {

                do {
                    try self.mm.addMedia(url: url,
                                         allowedTypes: [ .D64, .T64, .PRG, .P00, .G64 ],
                                         drive: id,
                                         options: [.force])
                } catch {
                    self.showAlert(.cantInsert, error: error, async: true)
                }
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: NSMenuItem!) {
        
        let drive = sender.tag < 10 ? DRIVE8 : DRIVE9
        let slot  = sender.tag % 10

        insertRecentDiskAction(drive: drive, slot: slot)
    }

    func insertRecentDiskAction(drive: Int, slot: Int) {

        let types: [FileType] = [ .D64, .T64, .PRG, .P00, .G64 ]

        if let url = MediaManager.getRecentlyInsertedDiskURL(slot) {

            do {
                try self.mm.addMedia(url: url, allowedTypes: types, drive: drive)
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

        let drive = c64.drive(nr)

        if proceedWithUnsavedFloppyDisk(drive: drive) {

            drive.ejectDisk()
            mm.clearRecentlyExportedDiskURLs(drive: nr)
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

        switch nr {

        case DRIVE8: c64.drive8.disk.toggleWriteProtection()
        case DRIVE9: c64.drive9.disk.toggleWriteProtection()

        default:
            fatalError()
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
        
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["tap", "zip", "gz"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK, let url = openPanel.url {
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
            try mm.addMedia(url: url, allowedTypes: [ .TAP ])
        } catch {
            self.showAlert(.cantInsertTape, error: error)
        }
    }

    @IBAction func ejectTapeAction(_ sender: Any!) {

        c64.datasette.ejectTape()
    }
    
    @IBAction func playOrStopAction(_ sender: Any!) {

        if c64.datasette.playKey {
            c64.datasette.pressStop()
        } else {
            c64.datasette.pressPlay()
        }
    }
    
    @IBAction func rewindAction(_ sender: Any!) {

        c64.datasette.rewind()
    }

    //
    // Action methods (Cartridge menu)
    //

    @IBAction func attachCartridgeAction(_ sender: Any!) {
        
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Attach"
        openPanel.allowedFileTypes = ["crt", "zip", "gz"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK, let url = openPanel.url {

                do {
                    try self.mm.addMedia(url: url, allowedTypes: [ .CRT ])
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
                try self.mm.addMedia(url: url, allowedTypes: [ .CRT ])
            } catch {
                self.showAlert(.cantAttach, error: error, async: true)
            }
        }
    }

    @IBAction func detachCartridgeAction(_ sender: Any!) {
        c64.expansionport.detachCartridgeAndReset()
    }

    @IBAction func attachReuDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachReuAction(_ sender: NSMenuItem!) {

        let capacity = sender.tag
        c64.expansionport.attachReuCartridge(capacity)
    }

    @IBAction func reuBatteryAction(_ sender: Any!) {
        c64.expansionport.setBattery(!c64.expansionport.hasBattery())
    }

    @IBAction func attachGeoRamDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachGeoRamAction(_ sender: NSMenuItem!) {

        let capacity = sender.tag
        c64.expansionport.attachGeoRamCartridge(capacity)
    }

    @IBAction func geoRamBatteryAction(_ sender: Any!) {
        c64.expansionport.setBattery(!c64.expansionport.hasBattery())
    }

    @IBAction func attachIsepicAction(_ sender: Any!) {
        c64.expansionport.attachIsepicCartridge()
    }

    @IBAction func pressCartridgeButton1Action(_ sender: NSButton!) {
        
        c64.expansionport.pressButton(1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseButton(1)
        }
    }

    @IBAction func pressCartridgeButton2Action(_ sender: NSButton!) {
        
        c64.expansionport.pressButton(2)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseButton(2)
        }
    }
    
    @IBAction func pressButtonDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func setSwitchNeutralAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(0)
    }

    @IBAction func setSwitchLeftAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(-1)
    }

    @IBAction func setSwitchRightAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(1)
    }

    @IBAction func setSwitchDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction  func inspectCartridgeAction(_ sender: Any!) {

        let panel = CartridgeInspector(with: self, nibName: "CartridgeInspector")
        panel?.show(expansionPort: c64.expansionport)
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
