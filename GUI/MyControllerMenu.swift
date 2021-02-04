// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController: NSMenuItemValidation {
    
    open func validateMenuItem(_ item: NSMenuItem) -> Bool {

        let powered = c64.poweredOn
        let running = c64.running
 
        var driveID: DriveID { return DriveID.init(rawValue: item.tag)! }
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
            
        // File menu
        case #selector(MyController.importConfigAction(_:)),
             #selector(MyController.exportConfigAction(_:)),
             #selector(MyController.resetConfigAction(_:)):
            return !powered
            
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
            let title = c64.expansionport.getButtonTitle(1)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil
            
        case #selector(MyController.pressCartridgeButton2Action(_:)):
            let title = c64.expansionport.getButtonTitle(2)
            item.title = title ?? ""
            item.isHidden = title == nil
            return title != nil
            
        case #selector(MyController.setSwitchDummyAction(_:)):
            return c64.expansionport.hasSwitch()
            
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

        // Debug menu
        case #selector(MyController.traceAction(_:)):
            return !c64.isReleaseBuild
                        
        case #selector(MyController.dumpStateAction(_:)):
            return !c64.isReleaseBuild
            
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
                PreferencesController.make(parent: self,
                                           nibName: NSNib.Name("Preferences"))
        }
        myAppDelegate.prefController?.showWindow(self)
    }
    
    func importPrefs(_ prefixes: [String]) {
        
        track("Importing user defaults with prefixes \(prefixes)")
        
        let panel = NSOpenPanel()
        panel.prompt = "Import"
        panel.allowedFileTypes = ["vc64conf"]
        
        panel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = panel.url {
                    self.loadUserDefaults(url: url, prefixes: prefixes)
                }
            }
        })
    }
    
    func exportPrefs(_ prefixes: [String]) {
        
        track("Exporting user defaults with prefixes \(prefixes)")
        
        let panel = NSSavePanel()
        panel.prompt = "Export"
        panel.allowedFileTypes = ["vc64conf"]
        
        panel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = panel.url {
                    track()
                    self.saveUserDefaults(url: url, prefixes: prefixes)
                }
            }
        })
    }
    
    @IBAction func importConfigAction(_ sender: Any!) {
        
        importPrefs(["VC64_ROM", "VC64_HW", "VC64_VID"])
    }
    
    @IBAction func exportConfigAction(_ sender: Any!) {
        
        exportPrefs(["VC64_ROM", "VC64_HW", "VC64_VID"])
    }
    
    @IBAction func resetConfigAction(_ sender: Any!) {
        
        track()
        
        UserDefaults.resetRomUserDefaults()
        UserDefaults.resetHardwareUserDefaults()
        UserDefaults.resetPeripheralsUserDefaults()
        UserDefaults.resetAudioUserDefaults()
        UserDefaults.resetVideoUserDefaults()
        
        c64.suspend()
        config.loadRomUserDefaults()
        config.loadHardwareUserDefaults()
        config.loadPeripheralsUserDefaults()
        config.loadAudioUserDefaults()
        config.loadVideoUserDefaults()
        c64.resume()
    }
    
    //
    // Action methods (File menu)
    //
    
    func openConfigurator(tab: String = "") {
        
        if configurator == nil {
            let name = NSNib.Name("Configuration")
            configurator = ConfigurationController.make(parent: self, nibName: name)
        }
        configurator?.showSheet(tab: tab)
    }
    
    @IBAction func configureAction(_ sender: Any!) {
        
        openConfigurator()
    }

    @IBAction func inspectorAction(_ sender: Any!) {
        
        if inspector == nil {
            inspector = Inspector.make(parent: self, nibName: "Inspector")
        }
        inspector?.showWindow(self)
    }
    
    @IBAction func monitorAction(_ sender: Any!) {
        
        if monitor == nil {
            monitor = Monitor.make(parent: self, nibName: "Monitor")
        }
        monitor?.showWindow(self)
    }
    
    @IBAction func takeSnapshotAction(_ sender: Any!) {
        
        c64.requestUserSnapshot()
    }
    
    @IBAction func restoreSnapshotAction(_ sender: Any!) {
        
        if !restoreLatestSnapshot() {
            NSSound.beep()
        }
    }
    
    @IBAction func browseSnapshotsAction(_ sender: Any!) {
        
        track()
        if snapshotBrowser == nil {
            let name = NSNib.Name("SnapshotDialog")
            snapshotBrowser = SnapshotDialog.make(parent: self, nibName: name)
        }
        snapshotBrowser?.showSheet()
    }
    
    @IBAction func takeScreenshotAction(_ sender: Any!) {
        
        track()
        
        // Determine screenshot format
        let format = ScreenshotSource.init(rawValue: pref.screenshotSource)!
        
        // Take screenshot
        guard let screen = renderer.screenshot(source: format) else {
            track("Failed to create screenshot")
            return
        }

        // Convert to Screenshot object
        let screenshot = Screenshot.init(screen: screen, format: pref.screenshotTarget)

        // Save to disk
        try? screenshot.save(id: mydocument.bootDiskID)
        
        // Create a visual effect
        renderer.blendIn(steps: 20)
    }
    
    @IBAction func browseScreenshotsAction(_ sender: Any!) {
        
        track()
        
        if screenshotBrowser == nil {
            let name = NSNib.Name("ScreenshotDialog")
            screenshotBrowser = ScreenshotDialog.make(parent: self, nibName: name)
        }
        screenshotBrowser?.checksum = mydocument.bootDiskID
        screenshotBrowser?.showSheet()
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
        
        if c64.paused { c64.run() }
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

        track()

        renderer.rotateLeft()
        c64.reset()
        c64.run()
    }

    @IBAction func powerAction(_ sender: Any!) {
        
        var error = ErrorCode.OK

        if c64.poweredOn {
            c64.powerOff()
            return
        }
        
        if !c64.isReady(&error) {
            VC64Error.init(error).warning("Unable to power up the emulator")
            return
        }
        
        c64.run()
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

    /*
    func openVirtualKeyboard() {
        
        track()
        if virtualKeyboard == nil {
            let name = NSNib.Name("VirtualKeyboard")
            virtualKeyboard = VirtualKeyboardController.make(parent: self, nibName: name)
        }
        virtualKeyboard?.showSheet()
    }
    */
    
    @IBAction func stickyKeyboardAction(_ sender: Any!) {
        
        track()

        if virtualKeyboard == nil {
            let name = NSNib.Name("VirtualKeyboard")
            virtualKeyboard = VirtualKeyboardController.make(parent: self, nibName: name)
        }
        if virtualKeyboard?.window?.isVisible == true {
            track("Virtual keyboard already open")
        } else {
            track("Opeining virtual keyboard as a window")
            virtualKeyboard?.showWindow()
        }
    }
    
    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        
        track()
        c64.keyboard.releaseAll()
    }

    // -----------------------------------------------------------------
    @IBAction func runstopAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.runStop, duration: 4)
    }
    @IBAction func restoreAction(_ sender: Any!) {
        keyboard.pressKey(C64Key.restore, duration: 4)
    }
    @IBAction func runstopRestoreAction(_ sender: Any!) {
        keyboard.pressKeys([C64Key.runStop, C64Key.restore], duration: 4)
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
        
        let id = DriveID(rawValue: sender.tag)!
        c64.drive(id)?.insertNewDisk(pref.driveBlankDiskFormat)
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: id)
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
            mydocument.mountAttachmentAsDisk(drive: drive)
                        
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
        
        track("drive: \(id) slot: \(slot)")
        
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
        
        let drive = DriveID(rawValue: sender.tag)!
        
        if proceedWithUnexportedDisk(drive: drive) {
            
            c64.drive(drive)?.ejectDisk()
            myAppDelegate.clearRecentlyExportedDiskURLs(drive: drive)
        }
    }
    
    @IBAction func exportDiskAction(_ sender: NSMenuItem!) {

        let drive = DriveID(rawValue: sender.tag)!
        
        let nibName = NSNib.Name("ExportDialog")
        let exportPanel = ExportDialog.make(parent: self, nibName: nibName)
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
        
        track()
        
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
            mydocument.mountAttachmentAsTape()
            
            // Remember the URL
            myAppDelegate.noteNewRecentlyUsedURL(url)
            
        } catch {

            (error as? VC64Error)?.cantOpen(url: url)
        }
    }

    @IBAction func ejectTapeAction(_ sender: Any!) {
        track()
        c64.datasette.ejectTape()
    }
    
    @IBAction func playOrStopAction(_ sender: Any!) {
        track()
        if c64.datasette.playKey {
            c64.datasette.pressStop()
        } else {
            c64.datasette.pressPlay()
        }
    }
    
    @IBAction func rewindAction(_ sender: Any!) {
        track()
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
            mydocument.mountAttachmentAsCartridge()
            
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
    
    //
    // Action methods (Debug menu)
    //

    @IBAction func traceAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
    
    @IBAction func dumpStateAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
       
    @IBAction func dumpC64(_ sender: Any!) { c64.dump() }
    @IBAction func dumpC64CPU(_ sender: Any!) { c64.cpu.dump() }
    @IBAction func dumpC64CIA1(_ sender: Any!) {c64.cia1.dump() }
    @IBAction func dumpC64CIA2(_ sender: Any!) { c64.cia2.dump() }
    @IBAction func dumpC64VIC(_ sender: Any!) { c64.vic.dump() }
    @IBAction func dumpC64SID(_ sender: Any!) { c64.sid.dump() }
    @IBAction func dumpC64Memory(_ sender: Any!) { c64.mem.dump() }
    @IBAction func dumpVC1541(_ sender: Any!) { c64.drive8.dump() }
    @IBAction func dumpVC1541CPU(_ sender: Any!) { c64.drive8.dump() }
    @IBAction func dumpVC1541VIA1(_ sender: Any!) { c64.drive8.via1.dump() }
    @IBAction func dumpVC1541VIA2(_ sender: Any!) { c64.drive8.via2.dump() }
    @IBAction func dumpDisk(_ sender: Any!) { c64.drive8.disk.dump() }
    @IBAction func dumpKeyboard(_ sender: Any!) { c64.keyboard.dump() }
    @IBAction func dumpC64JoystickA(_ sender: Any!) { c64.port1.dump() }
    @IBAction func dumpC64JoystickB(_ sender: Any!) { c64.port2.dump(); gamePadManager.listDevices()}
    @IBAction func dumpIEC(_ sender: Any!) { c64.iec.dump() }
    @IBAction func dumpC64ExpansionPort(_ sender: Any!) { c64.expansionport.dump() }
}
