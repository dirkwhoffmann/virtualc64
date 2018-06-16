//
//  MyControllerMenu.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 25.01.18.
//

import Foundation

extension MyController {
    
    override open func validateMenuItem(_ item: NSMenuItem) -> Bool {
  
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
 
        // Disk menu
        if item.action == #selector(MyController.newDiskAction(_:)) {
            return c64.iec.driveConnected()
        }
        if item.action == #selector(MyController.insertRecentDiskAction(_:)) {
            if item.tag < recentDiskURLs.count {
                item.title = recentDiskURLs[item.tag].lastPathComponent
                item.isHidden = false
                item.image = NSImage.init(named: NSImage.Name(rawValue: "icon_small"))
            } else {
                item.isHidden = true
                item.image = nil
            }
            return true
        }
        if item.action == #selector(MyController.ejectDiskAction(_:)) {
            return c64.iec.driveConnected() && c64.vc1541.hasDisk()
        }
        if item.action == #selector(MyController.exportDiskAction(_:)) {
            return c64.vc1541.hasDisk()
        }
        if item.action == #selector(MyController.writeProtectAction(_:)) {
            let hasDisk = c64.vc1541.hasDisk()
            let protected = hasDisk && c64.vc1541.disk.writeProtected()
            item.state = protected ? .on : .off
            // item.title = protected ? "Remove write protection" : "Write protect disk"
            return hasDisk
        }
        if item.action == #selector(MyController.drivePowerAction(_:)) {
            let connected = c64.iec.driveConnected()
            // item.state = connected ? .on : .off
            item.title = connected ? "Disconnect drive" : "Connect drive"
            return true
        }

        // Tape menu
        if item.action == #selector(MyController.ejectTapeAction(_:)) {
            return c64.datasette.hasTape()
        }
        if item.action == #selector(MyController.playOrStopAction(_:)) {
            item.title = c64.datasette.playKey() ? "Press Stop" : "Press Play"
            return c64.datasette.hasTape()
        }
        if item.action == #selector(MyController.rewindAction(_:)) {
            return c64.datasette.hasTape()
        }
        
        // Cartridge menu
        if item.action == #selector(MyController.detachCartridgeAction(_:)) {
            return c64.expansionport.cartridgeAttached()
        }
        if item.action == #selector(MyController.finalCartridgeIIIaction(_:)) {
            return c64.expansionport.cartridgeType() == CRT_FINAL_III
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
            item.state = c64.vc1541.cpu.tracing() ? .on : .off
        }
        if item.action == #selector(MyController.traceViaAction(_:)) {
            item.state = c64.vc1541.via1.tracing() ? .on : .off
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
        
        if (c64.takeUserSnapshot()) {
            metalScreen.snapToFront()
        } else {
            userSnapshotStorageFull()
        }
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
            
            greenLED.isHidden = false
            redLED.isHidden = false
            progress.isHidden = false
            tapeProgress.isHidden = false
            driveIcon.isHidden = !c64.vc1541.hasDisk()
            tapeIcon.isHidden = !c64.datasette.hasTape()
            cartridgeIcon.isHidden = !c64.expansionport.cartridgeAttached()
            clockSpeed.isHidden = false
            clockSpeedBar.isHidden = false
            warpIcon.isHidden = false
            
            metalScreen.shrink()
            window?.setContentBorderThickness(24, for: .minY)
            adjustWindowSize()
            statusBar = true
        }
 
        if statusBar && !value {
            
            greenLED.isHidden = true
            redLED.isHidden = true
            progress.isHidden = true
            tapeProgress.isHidden = true
            driveIcon.isHidden = true
            tapeIcon.isHidden = true
            cartridgeIcon.isHidden = true
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
    @IBAction func shiftLockAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.shiftLockAction(sender)
        }
        if c64.keyboard.shiftLockIsPressed() {
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

    @IBAction func newDiskAction(_ sender: Any!) {
        
        if proceedWithUnsafedDisk() {
            c64.vc1541.ejectDisk()
            c64.insertDisk(ArchiveProxy.make())
        }
    }
    
    @IBAction func insertDiskAction(_ sender: Any!) {
        
        if !proceedWithUnsafedDisk() { return }
        
        let openPanel = NSOpenPanel()
        openPanel.allowsMultipleSelection = false
        openPanel.canChooseDirectories = false
        openPanel.canCreateDirectories = false
        openPanel.canChooseFiles = true
        openPanel.prompt = "Insert"
        openPanel.allowedFileTypes = ["t64", "prg", "p00", "d64", "g64", "nib"]
        
        // Run panel as sheet
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                let url = openPanel.url
                do {
                    try self.processFile(url: url,
                                         warnAboutUnsafedDisk: false,
                                         showMountDialog: false)
                } catch {
                    NSApp.presentError(error)
                }
            }
        })
    }
    
    @IBAction func insertRecentDiskAction(_ sender: Any!) {
        
        track()
        let sender = sender as! NSMenuItem
        let tag = sender.tag
        
        if tag < recentDiskURLs.count {
            do {
                try processFile(url: recentDiskURLs[tag],
                                warnAboutUnsafedDisk: true,
                                showMountDialog: false)
            } catch {
                NSApp.presentError(error)
            }
        }
    }
    
    @IBAction func clearRecentDisksAction(_ sender: Any!) {
        
        track()
        recentDiskURLs = []
    }
    
    @IBAction func ejectDiskAction(_ sender: Any!) {
        
        if proceedWithUnsafedDisk() {
            c64.vc1541.ejectDisk()
        }
    }
    
    @IBAction func exportDiskAction(_ sender: Any!) {

        let nibName = NSNib.Name(rawValue: "ExportDiskDialog")
        let exportPanel = ExportDiskController.init(windowNibName: nibName)
        exportPanel.showSheet(withParent: self)
    }
 
    @IBAction func writeProtectAction(_ sender: Any!) {
        
        let protected = c64.vc1541.disk.writeProtected()
        c64.vc1541.disk.setWriteProtection(!protected)
    }
    
    @IBAction func drivePowerAction(_ sender: Any!) {
        
        track()
        if c64.iec.driveConnected() {
            c64.iec.disconnectDrive()
        } else {
            c64.iec.connectDrive()
        }
    }
    
    
    //
    // Action methods (Datasette menu)
    //
    
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

    @IBAction func detachCartridgeAction(_ sender: Any!) {
        track()
        c64.detachCartridgeAndReset()
    }
    
    @IBAction func finalCartridgeIIIaction(_ sender: Any!) {
        // Dummy action method to enable menu item validation
    }
    
    @IBAction func finalCartridgeIIIfreezeAction(_ sender: Any!) {
        c64.expansionport.pressFirstButton()
    }
    
    @IBAction func finalCartridgeIIIresetAction(_ sender: Any!) {
        c64.expansionport.pressSecondButton()
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
        
        c64.vc1541.cpu.setTracing(!c64.vc1541.cpu.tracing())
    }
  
    @IBAction func traceViaAction(_ sender: Any!) {
        
        undoManager?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceViaAction(sender)
        }
        
        c64.vc1541.via1.setTracing(!c64.vc1541.via1.tracing())
        c64.vc1541.via2.setTracing(!c64.vc1541.via2.tracing())
    }
    
    @IBAction func dumpC64(_ sender: Any!) { c64.dump() }
    @IBAction func dumpC64CPU(_ sender: Any!) { c64.cpu.dump() }
    @IBAction func dumpC64CIA1(_ sender: Any!) {c64.cia1.dump() }
    @IBAction func dumpC64CIA2(_ sender: Any!) { c64.cia2.dump() }
    @IBAction func dumpC64VIC(_ sender: Any!) { c64.vic.dump() }
    @IBAction func dumpC64SID(_ sender: Any!) { c64.sid.dump() }
    @IBAction func dumpC64Memory(_ sender: Any!) { c64.mem.dump() }
    @IBAction func dumpVC1541(_ sender: Any!) { c64.vc1541.dump() }
    @IBAction func dumpVC1541CPU(_ sender: Any!) { c64.vc1541.dump() }
    @IBAction func dumpVC1541VIA1(_ sender: Any!) { c64.vc1541.dump() }
    @IBAction func dumpVC1541VIA2(_ sender: Any!) { c64.vc1541.via2.dump() }
    @IBAction func dumpDisk(_ sender: Any!) { c64.vc1541.disk.dump() }
    @IBAction func dumpKeyboard(_ sender: Any!) { c64.keyboard.dump() }
    @IBAction func dumpC64JoystickA(_ sender: Any!) { c64.port1.dump() }
    @IBAction func dumpC64JoystickB(_ sender: Any!) { c64.port2.dump(); gamePadManager.listDevices()}
    @IBAction func dumpIEC(_ sender: Any!) { c64.iec.dump() }
    @IBAction func dumpC64ExpansionPort(_ sender: Any!) { c64.expansionport.dump() }
    
    //
    // Action methods (Toolbar)
    //
    
    @IBAction func resetAction(_ sender: Any!) {
        
        let document = self.document as! MyDocument
        document.updateChangeCount(.changeDone)
        
        metalScreen.rotateBack()
        c64.powerUp()
        refresh()
    }
    
}
