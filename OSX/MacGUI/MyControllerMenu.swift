//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {
    
    override open func validateMenuItem(_ item: NSMenuItem) -> Bool {

        // track("validateMenuItem")
        
        func firstDrive() -> Bool {
            precondition(item.tag == 1 || item.tag == 2)
            return item.tag == 1
        }
    
        func validateURLlist(_ list : [URL], image: String) -> Bool {
            
            let pos = (item.tag < 10) ? item.tag : item.tag - 10
            
            if let url = mydocument.getRecentlyUsedURL(pos, from: list) {
                item.title = url.lastPathComponent
                item.isHidden = false
                item.image = NSImage.init(named: NSImage.Name(rawValue: image))
            } else {
                item.isHidden = true
                item.image = nil
            }
            return true
        }
        
        // View menu
        if item.action == #selector(MyController.toggleStatusBarAction(_:)) {
            item.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
            return true
        }
        
        if item.action == #selector(MyController.hideMouseAction(_:)) {
            item.title = hideMouse ? "Show Mouse Cursor" : "Hide Mouse Cursor"
            return true
        }
        
        // Keyboard menu
        if item.action == #selector(MyController.mapKeysByPositionAction(_:)) {
            item.state = keyboardcontroller.mapKeysByPosition ? .on : .off
            return true
        }
        if item.action == #selector(MyController.customizeKeyMap(_:)) {
            return keyboardcontroller.mapKeysByPosition
        }
        if item.action == #selector(MyController.shiftLockAction(_:)) {
            item.state = c64.keyboard.shiftLockIsHoldDown() ? .on : .off
            return true
        }
        
        // Drive menu
        if item.action == #selector(MyController.insertRecentDiskAction(_:)) {
            return validateURLlist(mydocument.recentlyInsertedDiskURLs, image: "disk_small")
        }
        if item.action == #selector(MyController.ejectDiskAction(_:)) {
            return firstDrive() ? c64.drive1.hasDisk() : c64.drive2.hasDisk()
        }
        if item.action == #selector(MyController.exportDiskAction(_:)) {
            return firstDrive() ? c64.drive1.hasDisk() : c64.drive2.hasDisk()
        }
        if item.action == #selector(MyController.exportRecentDiskAction(_:)) {
            if item.tag < 10 {
                track("\(mydocument.recentlyExportedDisk1URLs)")
                return validateURLlist(mydocument.recentlyExportedDisk1URLs, image: "disk_small")
            } else {
                track("\(mydocument.recentlyExportedDisk2URLs)")
                return validateURLlist(mydocument.recentlyExportedDisk2URLs, image: "disk_small")
            }
        }
        if item.action == #selector(MyController.writeProtectAction(_:)) {
            let hasDisk = firstDrive() ?
                c64.drive1.hasDisk() :
                c64.drive2.hasDisk()
            let hasWriteProtecteDisk = firstDrive() ?
                c64.drive1.hasWriteProtectedDisk() :
                c64.drive2.hasWriteProtectedDisk()
            item.state = hasWriteProtecteDisk ? .on : .off
            return hasDisk
        }
        if item.action == #selector(MyController.drivePowerAction(_:)) {
            let poweredOn = firstDrive() ?
                c64.drive1.isPoweredOn() :
                c64.drive2.isPoweredOn()
            item.title = poweredOn ? "Disconnect" : "Connect"
            return true
        }

        // Tape menu
        if item.action == #selector(MyController.insertRecentTapeAction(_:)) {
            return validateURLlist(mydocument.recentlyInsertedTapeURLs, image: "tape_small")
        }
        if item.action == #selector(MyController.ejectTapeAction(_:)) {
            return c64.datasette.hasTape()
        }
        if item.action == #selector(MyController.playOrStopAction(_:)) {
            item.title = c64.datasette.playKey() ? "Press Stop Key" : "Press Play On Tape"
            return c64.datasette.hasTape()
        }
        if item.action == #selector(MyController.rewindAction(_:)) {
            return c64.datasette.hasTape()
        }
        
        // Cartridge menu
        if item.action == #selector(MyController.attachRecentCartridgeAction(_:)) {
            return validateURLlist(mydocument.recentlyAttachedCartridgeURLs, image: "cartridge_small")
        }
        if item.action == #selector(MyController.detachCartridgeAction(_:)) {
            return c64.expansionport.cartridgeAttached()
        }
        if item.action == #selector(MyController.pressButtonAction(_:)) {
            return c64.expansionport.hasFreezeButton() || c64.expansionport.hasResetButton()
        }
        if item.action == #selector(MyController.pressFreezeButtonAction(_:)) {
            return c64.expansionport.hasFreezeButton()
        }
        if item.action == #selector(MyController.pressResetButtonAction(_:)) {
            return c64.expansionport.hasResetButton()
        }
        if item.action == #selector(MyController.geoRamBatteryAction(_:)) {
            item.state = c64.expansionport.hasBattery() ? .on : .off
            return c64.expansionport.cartridgeType() == CRT_GEO_RAM
        }
        
        // Debug menu
        if item.action == #selector(MyController.pauseAction(_:)) {
            return c64.isRunning();
        }
        if item.action == #selector(MyController.continueAction(_:)) ||
            item.action == #selector(MyController.stepIntoAction(_:)) ||
            item.action == #selector(MyController.stepOverAction(_:)) ||
            item.action == #selector(MyController.stopAndGoAction(_:)) {
            return c64.isHalted();
        }
        if item.action == #selector(MyController.markIRQLinesAction(_:)) {
            item.state = c64.vic.showIrqLines() ? .on : .off
        }
        if item.action == #selector(MyController.markDMALinesAction(_:)) {
            item.state = c64.vic.showDmaLines() ? .on : .off
        }
        if item.action == #selector(MyController.hideSpritesAction(_:)) {
            item.state = c64.vic.hideSprites() ? .on : .off
        }

        if item.action == #selector(MyController.traceAction(_:)) {
            return c64.developmentMode();
        }
        if item.action == #selector(MyController.traceIecAction(_:)) {
            item.state = c64.iec.tracing() ? .on : .off
        }
        if item.action == #selector(MyController.traceVC1541CpuAction(_:)) {
            item.state = c64.drive1.cpu.tracing() ? .on : .off
        }
        if item.action == #selector(MyController.traceViaAction(_:)) {
            item.state = c64.drive1.via1.tracing() ? .on : .off
        }
        
        if item.action == #selector(MyController.dumpStateAction(_:)) {
            return c64.developmentMode();
        }

        return true
    }

    
    //
    // Action methods (File menu)
    //
    
    @IBAction func saveScreenshotDialog(_ sender: Any!) {
    
        let nibName = NSNib.Name(rawValue: "ExportScreenshotDialog")
        let exportPanel = ExportScreenshotController.init(windowNibName: nibName)
        exportPanel.showSheet(withParent: self)
    }
    
    @IBAction func quicksaveScreenshot(_ sender: Any!) {
        
        let paths = NSSearchPathForDirectoriesInDomains(.desktopDirectory, .userDomainMask, true)
        let desktopUrl = NSURL.init(fileURLWithPath: paths[0])
        if let url = desktopUrl.appendingPathComponent("Untitled.tiff") {
            let image = metalScreen.screenshot()
            let data = image?.tiffRepresentation
            do {
                try data?.write(to: url, options: .atomic)
            } catch {
                track("Cannot quicksave screenshot")
            }
        }
    }
    
    @IBAction func takeSnapshot(_ sender: Any!) {
        
        c64.takeUserSnapshot()
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
        
        keyboardcontroller.typeOnKeyboard(string: text, completion: nil)
    }

    //
    // Action methods (View menu)
    //

    @IBAction func toggleStatusBarAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.toggleStatusBarAction(sender)
        }
        
        showStatusBar(!statusBar)
    }
    
    public func showStatusBar(_ value: Bool) {
        
        if !statusBar && value {
            
            greenLED1.isHidden = false
            redLED1.isHidden = false
            progress1.isHidden = false
            diskIcon1.isHidden = !c64.drive1.hasDisk()
            greenLED2.isHidden = false
            redLED2.isHidden = false
            progress2.isHidden = false
            diskIcon2.isHidden = !c64.drive2.hasDisk()
            cartridgeIcon.isHidden = !c64.expansionport.cartridgeAttached()
            tapeIcon.isHidden = !c64.datasette.hasTape()
            tapeProgress.isHidden = false
            clockSpeed.isHidden = false
            clockSpeedBar.isHidden = false
            warpIcon.isHidden = false
            
            metalScreen.shrink()
            window?.setContentBorderThickness(24, for: .minY)
            adjustWindowSize()
            statusBar = true
        }
 
        if statusBar && !value {
            
            greenLED1.isHidden = true
            redLED1.isHidden = true
            progress1.isHidden = true
            diskIcon1.isHidden = true
            greenLED2.isHidden = true
            redLED2.isHidden = true
            progress2.isHidden = true
            diskIcon2.isHidden = true
            cartridgeIcon.isHidden = true
            tapeIcon.isHidden = true
            tapeProgress.isHidden = true
            clockSpeed.isHidden = true
            clockSpeedBar.isHidden = true
            warpIcon.isHidden = true
            
            metalScreen.expand()
            window?.setContentBorderThickness(0, for: .minY)
            adjustWindowSize()
            statusBar = false
        }
    }
    
    @IBAction func hideMouseAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.hideMouseAction(sender)
        }
        
        if (hideMouse) {
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
        
        // Open the virtual keyboard as a window
        let nibName = NSNib.Name(rawValue: "VirtualKeyboard")
        virtualKeyboard = VirtualKeyboardController.init(windowNibName: nibName)
        virtualKeyboard?.showWindow(withParent: self)
    }
    
    @IBAction func mapKeysByPositionAction(_ sender: Any!) {
        
        track()
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.mapKeysByPositionAction(sender)
        }
        
        keyboardcontroller.mapKeysByPosition = !keyboardcontroller.mapKeysByPosition
    }
    
    @IBAction func customizeKeyMap(_ sender: Any!) {
        
        track()
        let nibName = NSNib.Name(rawValue: "KeyboardDialog")
        let controller = KeyboardDialog.init(windowNibName: nibName)
        controller.showSheet(withParent: self)
        
    }
    
 
    // -----------------------------------------------------------------
    @IBAction func runstopAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.runStop)
    }
    @IBAction func restoreAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.restore)
    }
    @IBAction func runstopRestoreAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(keyList: [C64Key.runStop, C64Key.restore])
    }
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.commodore)
    }
    @IBAction func clearKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(keyList: [C64Key.home, C64Key.shift])
    }
    @IBAction func homeKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.home)
    }
    @IBAction func insertKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(keyList: [C64Key.delete, C64Key.shift])
    }
    @IBAction func deleteKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.delete)
    }
    @IBAction func leftarrowKeyAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(key: C64Key.leftArrow)
    }
    @IBAction func shiftLockAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.shiftLockAction(sender)
        }
        if c64.keyboard.shiftLockIsHoldDown() {
            c64.keyboard.unlockShift()
        } else {
            c64.keyboard.lockShift()
        }
    }
    @IBAction func clearKeyboardMatrixAction(_ sender: Any!) {
        c64.keyboard.releaseAll()
    }

    // -----------------------------------------------------------------
    @IBAction func loadDirectoryAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(string: "LOAD \"$\",8", completion: nil)
    }
    @IBAction func listAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(string: "LIST", completion: nil)
    }
    @IBAction func loadFirstFileAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(string: "LOAD \"*\",8,1", completion: nil)
    }
    @IBAction func runProgramAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(string: "RUN", completion: nil)
    }
    @IBAction func formatDiskAction(_ sender: Any!) {
        keyboardcontroller.typeOnKeyboard(string: "OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1", completion: nil)
    }

 
    //
    // Action methods (Disk menu)
    //

    func driveNr(fromTagOf menuItem: Any!) -> Int {
        let tag = (menuItem as! NSMenuItem).tag
        precondition(tag == 1 || tag == 2)
        return tag
    }
    @IBAction func newDiskAction(_ sender: Any!) {
        
        let tag = (sender as! NSMenuItem).tag
        let emptyArchive = ArchiveProxy.make()
        
        mydocument.attachment = D64Proxy.make(withAnyArchive: emptyArchive)
        mydocument.insertAttachmentAsDisk(drive: tag)
        mydocument.clearRecentlyExportedDiskURLs(drive: tag)
    }
    
    @IBAction func insertDiskAction(_ sender: Any!) {
        
        let tag = (sender as! NSMenuItem).tag
        
        // Ask user to continue if the current disk contains modified data
        if !proceedWithUnexportedDisk(drive: tag) {
            return
        }
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["t64", "prg", "p00", "d64", "g64", "nib"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument.createAttachment(from: url)
                        self.mydocument.insertAttachmentAsDisk(drive: tag)
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: Any!) {
        
        track()
        var tag = (sender as! NSMenuItem).tag
        
        // Extrace drive number from tag
        var nr: Int
        if tag < 10 { nr = 1 } else { nr = 2; tag -= 10 }
        
        // Get URL and insert
        if let url = mydocument.getRecentlyInsertedDiskURL(tag) {
            do {
                try mydocument.createAttachment(from: url)
                if (mydocument.proceedWithUnexportedDisk(drive: nr)) {
                    mydocument.insertAttachmentAsDisk(drive: nr)
                }
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func exportRecentDiskAction(_ sender: Any!) {
        
        track()
        var tag = (sender as! NSMenuItem).tag
        
        // Extract drive number from tag
        let nr = (tag < 10) ? 1 : 2
        tag = (tag < 10) ? tag : tag - 10
       
        // Get URL and export
        if let url = mydocument.getRecentlyExportedDiskURL(tag, drive: nr) {
            mydocument.export(drive: nr, to: url)
        }
    }
    
    @IBAction func clearRecentlyInsertedDisksAction(_ sender: Any!) {
        mydocument.recentlyInsertedDiskURLs = []
    }

    @IBAction func clearRecentlyExportedDisksAction(_ sender: Any!) {

        let driveNr = (sender as! NSMenuItem).tag
        mydocument.clearRecentlyExportedDiskURLs(drive: driveNr)
    }

    @IBAction func clearRecentlyInsertedTapesAction(_ sender: Any!) {
        mydocument.recentlyInsertedTapeURLs = []
    }
    
    @IBAction func clearRecentlyAttachedCartridgesAction(_ sender: Any!) {
        mydocument.recentlyAttachedCartridgeURLs = []
    }
    
    @IBAction func ejectDiskAction(_ sender: Any!) {
        
        let tag = (sender as! NSMenuItem).tag
        
        if proceedWithUnexportedDisk(drive: tag) {
            changeDisk(nil, drive: tag)
            mydocument.clearRecentlyExportedDiskURLs(drive: tag)
        }
    }
    
    @IBAction func exportDiskAction(_ sender: Any!) {

        let nr = (sender as! NSMenuItem).tag
        precondition(nr == 1 || nr == 2)
        
        let nibName = NSNib.Name(rawValue: "ExportDiskDialog")
        let exportPanel = ExportDiskController.init(windowNibName: nibName)
        exportPanel.showSheet(withParent: self, drive: nr)
    }
     
    @IBAction func writeProtectAction(_ sender: Any!) {
        
        let nr = driveNr(fromTagOf: sender)
        if (nr == 1) {
            c64.drive1.disk.toggleWriteProtection()
        } else {
            c64.drive2.disk.toggleWriteProtection()
        }
    }
    
    @IBAction func drivePowerAction(_ sender: Any!) {
        
        let sender = sender as! NSMenuItem
        precondition(sender.tag == 1 || sender.tag == 2)
        drivePowerAction(driveNr: sender.tag)
    }

    @IBAction func drivePowerButtonAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        precondition(sender.tag == 1 || sender.tag == 2)
        drivePowerAction(driveNr: sender.tag)
    }
    
    func drivePowerAction(driveNr: Int) {
        if (driveNr == 1) {
            c64.drive1.togglePowerSwitch()
        } else {
            c64.drive2.togglePowerSwitch()
        }
    }
    
    //
    // Action methods (Datasette menu)
    //
    
    @IBAction func insertTapeAction(_ sender: Any!) {
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["tap"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument.createAttachment(from: url)
                        self.mydocument.insertAttachmentAsTape()
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func insertRecentTapeAction(_ sender: Any!) {
        
        track()
        let sender = sender as! NSMenuItem
        let tag = sender.tag
        
        if let url = mydocument.getRecentlyInsertedTapeURL(tag) {
            do {
                try mydocument.createAttachment(from: url)
                mydocument.insertAttachmentAsTape()
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func ejectTapeAction(_ sender: Any!) {
        track()
        c64.datasette.ejectTape()
    }
    
    @IBAction func playOrStopAction(_ sender: Any!) {
        track()
        if c64.datasette.playKey() {
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
        
        // Show the OpenPanel
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Attach"
        openPanel.allowedFileTypes = ["crt"]
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = openPanel.url {
                    do {
                        try self.mydocument.createAttachment(from: url)
                        self.mydocument.attachAttachmentAsCartridge()
                    } catch {
                        NSApp.presentError(error)
                    }
                }
            }
        })
    }
    
    @IBAction func attachRecentCartridgeAction(_ sender: Any!) {
        
        track()
        let sender = sender as! NSMenuItem
        let tag = sender.tag
        
        if let url = mydocument.getRecentlyAtachedCartridgeURL(tag) {
            do {
                try mydocument.createAttachment(from: url)
                mydocument.attachAttachmentAsCartridge()
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func detachCartridgeAction(_ sender: Any!) {
        track()
        c64.detachCartridgeAndReset()
    }
    
    @IBAction func attachGeoRamAction(_ sender: Any!) {
        let sender = sender as! NSMenuItem
        let capacity = sender.tag
        track("RAM capacity = \(capacity)")
        c64.expansionport.attachGeoRamCartridge(capacity)
    }
    
    @IBAction func geoRamBatteryAction(_ sender: Any!) {
        c64.expansionport.setBattery(!c64.expansionport.hasBattery())
    }
    
    @IBAction func pressFreezeButtonAction(_ sender: Any!) {
        track()
        c64.expansionport.pressFreezeButton()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseFreezeButton()
        }
    }
    
    @IBAction func pressResetButtonAction(_ sender: Any!) {
        track()
        c64.expansionport.pressResetButton()
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.c64.expansionport.releaseResetButton()
        }
    }
    
    @IBAction func pressButtonAction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }

    
    //
    // Action methods (Debug menu)
    //

    @IBAction func hideSpritesAction(_ sender: Any!) {

        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.hideSpritesAction(sender)
        }
        
        c64.vic.setHideSprites(!c64.vic.hideSprites())
    }
  
    @IBAction func markIRQLinesAction(_ sender: Any!) {
    
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.markIRQLinesAction(sender)
        }
        
        c64.vic.setShowIrqLines(!c64.vic.showIrqLines())
    }
    
    @IBAction func markDMALinesAction(_ sender: Any!) {
    
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.markDMALinesAction(sender)
        }
        
        c64.vic.setShowDmaLines(!c64.vic.showDmaLines())
    }
    
    @IBAction func traceAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
    
    @IBAction func dumpStateAction(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
  
    @IBAction func traceIecAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceIecAction(sender)
        }
        
        c64.iec.setTracing(!c64.iec.tracing())
    }
 
    @IBAction func traceVC1541CpuAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceVC1541CpuAction(sender)
        }
        
        c64.drive1.cpu.setTracing(!c64.drive1.cpu.tracing())
    }
  
    @IBAction func traceViaAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceViaAction(sender)
        }
        
        c64.drive1.via1.setTracing(!c64.drive1.via1.tracing())
        c64.drive1.via2.setTracing(!c64.drive1.via2.tracing())
    }
    
    @IBAction func dumpC64(_ sender: Any!) { c64.dumpState() }
    @IBAction func dumpC64CPU(_ sender: Any!) { c64.cpu.dump() }
    @IBAction func dumpC64CIA1(_ sender: Any!) {c64.cia1.dump() }
    @IBAction func dumpC64CIA2(_ sender: Any!) { c64.cia2.dump() }
    @IBAction func dumpC64VIC(_ sender: Any!) { c64.vic.dump() }
    @IBAction func dumpC64SID(_ sender: Any!) { c64.sid.dump() }
    @IBAction func dumpC64Memory(_ sender: Any!) { c64.mem.dump() }
    @IBAction func dumpVC1541(_ sender: Any!) { c64.drive1.dump() }
    @IBAction func dumpVC1541CPU(_ sender: Any!) { c64.drive1.dump() }
    @IBAction func dumpVC1541VIA1(_ sender: Any!) { c64.drive1.via1.dump() }
    @IBAction func dumpVC1541VIA2(_ sender: Any!) { c64.drive1.via2.dump() }
    @IBAction func dumpDisk(_ sender: Any!) { c64.drive1.disk.dump() }
    @IBAction func dumpKeyboard(_ sender: Any!) { c64.keyboard.dump() }
    @IBAction func dumpC64JoystickA(_ sender: Any!) { c64.port1.dump() }
    @IBAction func dumpC64JoystickB(_ sender: Any!) { c64.port2.dump(); gamePadManager.listDevices()}
    @IBAction func dumpIEC(_ sender: Any!) { c64.iec.dump() }
    @IBAction func dumpC64ExpansionPort(_ sender: Any!) { c64.expansionport.dump() }
    
    //
    // Action methods (Toolbar)
    //
    
    @IBAction func resetAction(_ sender: Any!) {
        
        mydocument.updateChangeCount(.changeDone)
        metalScreen.rotateBack()
        c64.powerUp()
        refresh()
    }
    
}
