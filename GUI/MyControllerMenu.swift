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
        
        var driveID: DriveID { return DriveID(rawValue: item.tag)! }
        var drive: DriveProxy { return c64.drive(driveID) }
        
        func validateURLlist(_ list: [URL], image: NSImage) -> Bool {
            
            let slot = item.tag % 10
            
            if let url = myAppDelegate.getRecentlyUsedURL(slot, from: list) {
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
            
        case #selector(MyController.hideMouseAction(_:)):
            item.title = hideMouse ? "Show Mouse Cursor" : "Hide Mouse Cursor"
            return true
            
        // Keyboard menu
        case #selector(MyController.shiftLockAction(_:)):
            item.state = c64.keyboard.shiftLockIsPressed() ? .on : .off
            return true
            
        // Drive menu
        case #selector(MyController.insertRecentDiskAction(_:)):
            return validateURLlist(myAppDelegate.recentlyInsertedDiskURLs, image: smallDisk)
            
        case  #selector(MyController.ejectDiskAction(_:)),
              #selector(MyController.exportDiskAction(_:)):
            return drive.hasDisk()
            
        case #selector(MyController.exportRecentDiskDummyAction8(_:)):
            return c64.drive8.hasDisk()
            
        case #selector(MyController.exportRecentDiskDummyAction9(_:)):
            return c64.drive9.hasDisk()
                        
        case #selector(MyController.exportRecentDiskAction(_:)):
            switch item.tag {
            case 8: return validateURLlist(myAppDelegate.recentlyExportedDisk8URLs, image: smallDisk)
            case 9: return validateURLlist(myAppDelegate.recentlyExportedDisk9URLs, image: smallDisk)
            default: fatalError()
            }
            
        case #selector(MyController.writeProtectAction(_:)):
            item.state = drive.hasWriteProtectedDisk() ? .on : .off
            return drive.hasDisk()
            
        case #selector(MyController.drivePowerAction(_:)):
            track()
            item.title = drive.isSwitchedOn() ? "Switch off" : "Switch on"
            return true
            
        // Tape menu
        case #selector(MyController.insertRecentTapeAction(_:)):
            return validateURLlist(myAppDelegate.recentlyInsertedTapeURLs, image: smallTape)
            
        case #selector(MyController.ejectTapeAction(_:)):
            return c64.datasette.hasTape
            
        case #selector(MyController.playOrStopAction(_:)):
            item.title = c64.datasette.playKey ? "Press Stop Key" : "Press Play On Tape"
            return c64.datasette.hasTape
            
        case #selector(MyController.rewindAction(_:)):
            return c64.datasette.hasTape
            
        // Cartridge menu
        case #selector(MyController.attachRecentCartridgeAction(_:)):
            return validateURLlist(myAppDelegate.recentlyAttachedCartridgeURLs, image: smallCart)
            
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

        // Wipe out all settings
        defaults.removeAll()
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
        
        track()
        if snapshotBrowser == nil {
            let name = NSNib.Name("SnapshotDialog")
            snapshotBrowser = SnapshotDialog(with: self, nibName: name)
        }
        snapshotBrowser?.showSheet()
    }
    
    @IBAction func takeScreenshotAction(_ sender: Any!) {
        
        track()
        
        // Determine screenshot format
        let format = ScreenshotSource(rawValue: pref.screenshotSource)!
        
        // Take screenshot
        guard let screen = renderer.canvas.screenshot(source: format) else {

            log(warning: "Failed to create screenshot")
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
            screenshotBrowser = ScreenshotDialog(with: self, nibName: "ScreenshotDialog")
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
        
        track()
        
        let name = NSNib.Name("ExportVideoDialog")
        let exporter = ExportVideoDialog(with: self, nibName: name)
        
        exporter?.showSheet()
    }
    
    //
    // Action methods (Edit menu)
    //
    
    @IBAction func paste(_ sender: Any!) {
        
        track()
        
        let pasteBoard = NSPasteboard.general
        guard let text = pasteBoard.string(forType: .string) else {
            track("Cannot paste. No text in pasteboard")
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
            } catch let error as VC64Error {
                error.warning("Unable to power up the emulator")
            } catch {
                fatalError()
            }
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
    
    @IBAction func hideMouseAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.hideMouseAction(sender)
        }
        
        if hideMouse {
            NSCursor.unhide()
            CGAssociateMouseAndMouseCursorPosition(boolean_t(truncating: true))
        } else {
            NSCursor.hide()
            CGAssociateMouseAndMouseCursorPosition(boolean_t(truncating: false))
        }
        
        hideMouse = !hideMouse
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
            track("Virtual keyboard already open")
        } else {
            track("Opeining virtual keyboard as a window")
        }

        virtualKeyboard?.showWindow()
    }
    
    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        c64.keyboard.releaseAll()
    }

    // -----------------------------------------------------------------
    @IBAction func pressAction(_ sender: NSMenuItem!) {
     
        keyboard.pressKey(C64Key(sender.tag), duration: 4)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func pressWithShiftAction(_ sender: NSMenuItem!) {
        
        keyboard.pressKeys([C64Key.shift, C64Key(sender.tag)], duration: 4)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func pressRunstopRestoreAction(_ sender: Any!) {
        
        keyboard.pressKeys([C64Key.runStop, C64Key.restore], duration: 4)
        virtualKeyboard?.refresh()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.virtualKeyboard?.refresh()
        }
    }

    @IBAction func runstopAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.runStop, duration: 4)
    }
    
    @IBAction func restoreAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.restore, duration: 4)
    }
    
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.commodore, duration: 4)
    }
    
    @IBAction func clearKeyAction(_ sender: Any!) {
        keyboard.pressKeys([C64Key.home, C64Key.shift], duration: 4)
    }
    
    @IBAction func homeKeyAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.home, duration: 4)
    }
    
    @IBAction func insertKeyAction(_ sender: Any!) {
        keyboard.pressKeys([C64Key.delete, C64Key.shift], duration: 4)
    }
    
    @IBAction func deleteKeyAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.delete, duration: 4)
    }
    
    @IBAction func leftarrowKeyAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.leftArrow, duration: 4)
    }
    
    @IBAction func shiftLockAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) { targetSelf in
            targetSelf.shiftLockAction(sender)
        }
        c64.keyboard.toggleShiftLock()
    }

    // -----------------------------------------------------------------
    @IBAction func loadDirectoryAction(_ sender: Any!) {
        keyboard.type("LOAD \"$\",8")
    }
    @IBAction func listAction(_ sender: Any!) {
        keyboard.type("LIST")
    }
    @IBAction func loadFirstFileAction(_ sender: Any!) {
        keyboard.type("LOAD \"*\",8,1")
    }
    @IBAction func runProgramAction(_ sender: Any!) {
        keyboard.type("RUN")
    }
    @IBAction func formatDiskAction(_ sender: Any!) {
        keyboard.type("OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1")
    }
    
    //
    // Action methods (Drive menu)
    //

    @IBAction func newDiskAction(_ sender: NSMenuItem!) {
        
        let drive = c64.drive(sender)
        
        drive.insertNewDisk(config.blankDiskFormat)
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive.id)
    }
    
    @IBAction func insertDiskAction(_ sender: NSMenuItem!) {
        
        let id = DriveID(rawValue: sender.tag)!
        
        // Ask user to continue if the current disk contains modified data
        if !proceedWithUnexportedDisk(drive: id) {
            return
        }
        
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["t64", "prg", "p00", "d64", "g64", "zip", "gz"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK, let url = openPanel.url {
                self.insertDiskAction(from: url, drive: id)
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: NSMenuItem!) {
        
        let drive = sender.tag < 10 ? DriveID.DRIVE8 : DriveID.DRIVE9
        let slot  = sender.tag % 10
                
        if let url = myAppDelegate.getRecentlyInsertedDiskURL(slot) {
            insertDiskAction(from: url, drive: drive)
        }
    }
    
    func insertDiskAction(from url: URL, drive: DriveID) {
        
        let types = [ FileType.D64,
                      FileType.T64,
                      FileType.PRG,
                      FileType.P00,
                      FileType.G64 ]
        
        do {
            // Try to create a file proxy
            try mydocument.createAttachment(from: url, allowedTypes: types)

            // Ask the user if an unsafed disk should be replaced
            if !proceedWithUnexportedDisk(drive: drive) { return }
            
            // Insert the disk
            try mydocument.mountAttachment(drive: drive)
                        
            // Remember the URL
            myAppDelegate.noteNewRecentlyUsedURL(url)
            
        } catch {
            
            (error as? VC64Error)?.cantOpen(url: url)
        }
    }
    
    @IBAction func exportRecentDiskDummyAction8(_ sender: NSMenuItem!) {}
    @IBAction func exportRecentDiskDummyAction9(_ sender: NSMenuItem!) {}

    @IBAction func exportRecentDiskAction(_ sender: NSMenuItem!) {
                
        track()

        let drive = sender.tag < 10 ? DriveID.DRIVE8 : DriveID.DRIVE9
        let slot = sender.tag % 10
        
        exportRecentDiskAction(drive: drive, slot: slot)
    }
    
    func exportRecentDiskAction(drive id: DriveID, slot: Int) {
                
        if let url = myAppDelegate.getRecentlyExportedDiskURL(slot, drive: id) {
            
            do {
                try mydocument.export(drive: id, to: url)
                
            } catch let error as VC64Error {
                error.warning("Cannot export disk to file \"\(url.path)\"")
            } catch {
                fatalError()
            }
        }
    }
    
    @IBAction func clearRecentlyInsertedDisksAction(_ sender: Any!) {
        myAppDelegate.recentlyInsertedDiskURLs = []
    }

    @IBAction func clearRecentlyExportedDisksAction(_ sender: NSMenuItem!) {

        let drive = DriveID(rawValue: sender.tag)!
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive)
    }

    @IBAction func clearRecentlyInsertedTapesAction(_ sender: Any!) {
        myAppDelegate.recentlyInsertedTapeURLs = []
    }
    
    @IBAction func clearRecentlyAttachedCartridgesAction(_ sender: Any!) {
        myAppDelegate.recentlyAttachedCartridgeURLs = []
    }
    
    @IBAction func ejectDiskAction(_ sender: NSMenuItem!) {
        
        let drive = c64.drive(sender)
        
        if proceedWithUnexportedDisk(drive: drive.id) {
            
            drive.ejectDisk()
            myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive.id)
        }
    }
    
    @IBAction func exportDiskAction(_ sender: NSMenuItem!) {

        let drive = DriveID(rawValue: sender.tag)!
        
        let nibName = NSNib.Name("ExportDialog")
        let exportPanel = ExportDialog(with: self, nibName: nibName)
        exportPanel?.showSheet(forDrive: drive)
    }
     
    @IBAction func writeProtectAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(rawValue: sender.tag)!

        if drive == .DRIVE8 {
            c64.drive8.disk.toggleWriteProtection()
        } else {
            c64.drive9.disk.toggleWriteProtection()
        }
    }
    
    @IBAction func drivePowerAction(_ sender: NSMenuItem!) {
        
        let drive = DriveID(rawValue: sender.tag)!
        drivePowerAction(drive: drive)
    }
    
    func drivePowerAction(drive: DriveID) {
                
        switch drive {
        case .DRIVE8: config.drive8PowerSwitch = !config.drive8PowerSwitch
        case .DRIVE9: config.drive9PowerSwitch = !config.drive9PowerSwitch
        default: fatalError()
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
        
        if let url = myAppDelegate.getRecentlyInsertedTapeURL(slot) {
            insertTapeAction(from: url)
        }
    }
    
    func insertTapeAction(from url: URL) {
        
        let types = [ FileType.TAP ]
        
        do {
            // Try to create a file proxy
            try mydocument.createAttachment(from: url, allowedTypes: types)
            
            // Insert the tape
            try mydocument.mountAttachment()
            
            // Remember the URL
            myAppDelegate.noteNewRecentlyUsedURL(url)
            
        } catch {

            (error as? VC64Error)?.cantOpen(url: url)
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
                self.attachCartridgeAction(from: url)
            }
        })
    }
    
    @IBAction func attachRecentCartridgeAction(_ sender: NSMenuItem!) throws {
        
        let slot  = sender.tag
        
        if let url = myAppDelegate.getRecentlyAtachedCartridgeURL(slot) {
            attachCartridgeAction(from: url)
        }
    }
    
    func attachCartridgeAction(from url: URL) {
        
        let types = [ FileType.CRT ]
        
        do {
            // Try to create a file proxy
            try mydocument.createAttachment(from: url, allowedTypes: types)
            
            // Attach the cartridge
            try mydocument.mountAttachment()
            
            // Remember the URL
            myAppDelegate.noteNewRecentlyUsedURL(url)
            
        } catch {

            (error as? VC64Error)?.cantOpen(url: url)
        }
    }
    
    @IBAction func detachCartridgeAction(_ sender: Any!) {
        track()
        c64.expansionport.detachCartridgeAndReset()
    }

    @IBAction func attachGeoRamDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    @IBAction func attachGeoRamAction(_ sender: NSMenuItem!) {

        let capacity = sender.tag
        c64.expansionport.attachGeoRamCartridge(capacity)
    }
    
    @IBAction func attachIsepicAction(_ sender: Any!) {
        track("")
        c64.expansionport.attachIsepicCartridge()
    }
    
    @IBAction func geoRamBatteryAction(_ sender: Any!) {
        c64.expansionport.setBattery(!c64.expansionport.hasBattery())
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
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchLeftAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(-1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchRightAction(_ sender: Any!) {
        
        c64.expansionport.setSwitchPosition(1)
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            // TODO: Delete or call a method here if it is really needed.
        }
    }

    @IBAction func setSwitchDummyAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }
}
