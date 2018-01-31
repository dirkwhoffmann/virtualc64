//
//  MyControllerMenu.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 25.01.18.
//

import Foundation

extension MyController {
    
    //
    // Menu item validation (NSMenuValidation informal protocol)
    //
    
    @objc override open func validateMenuItem(_ menuItem: NSMenuItem) -> Bool {
        
        // View menu
        if menuItem.action == #selector(MyController.toggleStatusBarAction(_:)) {
            menuItem.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
        }
        
        // Keyboard menu
        if menuItem.action == #selector(MyController.toggleShiftKey(_:)) {
            menuItem.state = c64.keyboard.shiftKeyIsPressed() ? .on : .off
        }
        if menuItem.action == #selector(MyController.toggleCommodoreKey(_:)) {
            menuItem.state = c64.keyboard.commodoreKeyIsPressed() ? .on : .off
        }
        if menuItem.action == #selector(MyController.toggleCtrlKey(_:)) {
            menuItem.state = c64.keyboard.ctrlKeyIsPressed() ? .on : .off
        }
        if menuItem.action == #selector(MyController.toggleRunstopKey(_:)) {
            menuItem.state = c64.keyboard.runstopKeyIsPressed() ? .on : .off
        }
        
        // Disk menu
        if menuItem.action == #selector(MyController.driveAction(_:)) {
            menuItem.title = c64.iec.isDriveConnected() ? "Power off" : "Power on"
            return true
        }
        if menuItem.action == #selector(MyController.insertBlankDisk(_:)) {
            return c64.iec.isDriveConnected()
        }
        if menuItem.action == #selector(MyController.driveEjectAction(_:)) {
            return c64.iec.isDriveConnected() && c64.vc1541.hasDisk()
        }
        if menuItem.action == #selector(MyController.exportDisk(_:)) {
            return c64.vc1541.hasDisk()
        }
        
        // Cartridge menu
        if menuItem.action == #selector(MyController.finalCartridgeIIIaction(_:)) {
            return c64.expansionport.cartridgeType() == CRT_FINAL_III
        }
        
        // Debug menu
        if menuItem.action == #selector(MyController.pauseAction(_:)) {
            return c64.isRunning();
        }
        if menuItem.action == #selector(MyController.continueAction(_:)) ||
            menuItem.action == #selector(MyController.stepIntoAction(_:)) ||
            menuItem.action == #selector(MyController.stepOutAction(_:)) ||
            menuItem.action == #selector(MyController.stepOverAction(_:)) ||
            menuItem.action == #selector(MyController.stopAndGoAction(_:)) {
            return c64.isHalted();
        }
        if menuItem.action == #selector(MyController.markIRQLinesAction(_:)) {
            menuItem.state = c64.vic.showIrqLines() ? .on : .off
        }
        if menuItem.action == #selector(MyController.markDMALinesAction(_:)) {
            menuItem.state = c64.vic.showDmaLines() ? .on : .off
        }
        if menuItem.action == #selector(MyController.hideSpritesAction(_:)) {
            menuItem.state = c64.vic.hideSprites() ? .on : .off
        }
        if menuItem.action == #selector(MyController.traceC64CpuAction(_:)) {
            menuItem.state = c64.cpu.tracingEnabled() ? .on : .off
        }
        if menuItem.action == #selector(MyController.traceIecAction(_:)) {
            menuItem.state = c64.iec.tracingEnabled() ? .on : .off
        }
        if menuItem.action == #selector(MyController.traceVC1541CpuAction(_:)) {
            menuItem.state = c64.vc1541.cpu.tracingEnabled() ? .on : .off
        }
        if menuItem.action == #selector(MyController.traceViaAction(_:)) {
            menuItem.state = c64.vc1541.via1.tracingEnabled() ? .on : .off
        }
        
        return true
    }
   
    // -----------------------------------------------------------------
    // Action methods (File menu)
    // -----------------------------------------------------------------
    
    @IBAction func insertBlankDisk(_ sender: Any!) {
        
        if !c64.vc1541.diskModified() || showUnsafedDiskAlert() == .alertFirstButtonReturn {
        
            // Create empty Archive
            let archive = ArchiveProxy()
            // Convert to D64Archive
            let d64archive = D64ArchiveProxy.archive(fromArchive: archive)
            // Insert as disk
            c64.mountArchive(d64archive)
        }
    }
    
    // -----------------------------------------------------------------
    // Action methods (View menu)
    // -----------------------------------------------------------------

    @IBAction func toggleStatusBarAction(_ sender: Any!) {
        
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.toggleStatusBarAction(sender)
        }
        
        showStatusBar(!statusBar)
    }
    
    
    @objc public func showStatusBar(_ value: Bool) {
        
        if !statusBar && value {
            
            greenLED.isHidden = false
            redLED.isHidden = false
            progress.isHidden = false
            tapeProgress.isHidden = false
            driveIcon.isHidden = !c64.vc1541.hasDisk()
            driveEject.isHidden = !c64.vc1541.hasDisk()
            tapeIcon.isHidden = !c64.datasette.hasTape()
            cartridgeIcon.isHidden = !c64.expansionport.cartridgeAttached()
            cartridgeEject.isHidden = !c64.expansionport.cartridgeAttached()
            info.isHidden = false
            clockSpeed.isHidden = false
            clockSpeedBar.isHidden = false
            warpIcon.isHidden = false
            
            shrink()
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
            driveEject.isHidden = true
            tapeIcon.isHidden = true
            cartridgeIcon.isHidden = true
            cartridgeEject.isHidden = true
            info.isHidden = true
            clockSpeed.isHidden = true
            clockSpeedBar.isHidden = true
            warpIcon.isHidden = true
            
            expand()
            window?.setContentBorderThickness(0, for: .minY)
            adjustWindowSize()
            statusBar = false
        }
    }
        
    // -----------------------------------------------------------------
    // Action methods (Keyboard menu)
    // -----------------------------------------------------------------


    @IBAction func shiftAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_SHIFT))
    }
    
    // >>>>
    @IBAction func shiftCommodoreKeyAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_COMMODORE))
    }
    @IBAction func shiftCtrlKeyAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_CTRL))
    }
    @IBAction func shiftRunstopKeyAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_RUNSTOP))
    }
    @IBAction func shiftRestoreAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_RESTORE))
    }
    @IBAction func shiftLeftarrowAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_LEFTARROW))
    }
    @IBAction func shiftUparrowAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_UPARROW))
    }
    @IBAction func shiftPowndAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_POUND))
    }
    // <<<<
    
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_COMMODORE))
    }
    @IBAction func ctrlKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_CTRL))
    }
    @IBAction func runstopAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_RUNSTOP))
    }
    @IBAction func restoreAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_RESTORE))
    }
    @IBAction func runstopRestoreAction(_ sender: Any!) {
        simulateUserPressingKey(withRunstop: C64KeyFingerprint(C64KEY_RESTORE))
    }
    @IBAction func leftarrowAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_LEFTARROW))
    }
    @IBAction func uparrowAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_UPARROW))
    }
    @IBAction func powndAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_POUND))
    }
    @IBAction func clearKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_CLR))
    }
    @IBAction func homeKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_HOME))
    }
    @IBAction func insertKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_INST))
    }
    @IBAction func deleteKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_DEL))
    }
    
    // -----
    
    @IBAction func toggleShiftKey(_ sender: Any!) {
        c64.keyboard.toggleShiftKey()
        c64.keyboard.dump()
    }
    @IBAction func toggleCommodoreKey(_ sender: Any!) {
        c64.keyboard.toggleCommodoreKey()
        c64.keyboard.dump()
    }
    @IBAction func toggleCtrlKey(_ sender: Any!) {
        c64.keyboard.toggleCtrlKey()
        c64.keyboard.dump()
    }
    @IBAction func toggleRunstopKey(_ sender: Any!) {
        c64.keyboard.toggleRunstopKey()
        c64.keyboard.dump()
    }
    
    // ----
    
    @IBAction func loadDirectoryAction(_ sender: Any!) {
        simulateUserTypingText("LOAD \"$\",8")
    }
    @IBAction func listAction(_ sender: Any!) {
        simulateUserTypingText("LIST")
    }
    @IBAction func loadFirstFileAction(_ sender: Any!) {
        simulateUserTypingText("LOAD \"*\",8,1")
    }
    @IBAction func runProgramAction(_ sender: Any!) {
        simulateUserTypingText("RUN")
    }
    @IBAction func formatDiskAction(_ sender: Any!) {
        simulateUserTypingText("OPEN 1,8,15,\"N:TEST, ID\": CLOSE 1")
    }

 
    // -----------------------------------------------------------------
    // Action methods (Disk menu)
    // -----------------------------------------------------------------

    @IBAction func exportDisk(_ sender: Any!) {
        // Dummy target to make menu item validatable
    }
    @IBAction func exportDiskD64(_ sender: Any!) {
        exportDiskDialogWorker(type: D64_CONTAINER)
    }
    @IBAction func exportDiskT64(_ sender: Any!) {
        exportDiskDialogWorker(type: T64_CONTAINER)
    }
    @IBAction func exportDiskPRG(_ sender: Any!) {
        exportDiskDialogWorker(type: PRG_CONTAINER)
    }
    @IBAction func exportDiskP00(_ sender: Any!) {
        exportDiskDialogWorker(type: P00_CONTAINER)
    }
    
    /*! @brief   Main functionality of exportDiskDialog
     *  @result  true if disk contents has been exportet, false if operation was canceled.
     */
    @discardableResult
    func exportDiskDialogWorker(type: ContainerType) -> Bool {
    
        func numberOfItems(archive: ArchiveProxy, format: String) -> Int {
            
            let items = archive.getNumberOfItems()
            
            if items == 0 { showEmptyDiskAlert(format: format) }
            if items > 1  { showMoreThanOneFileAlert(format: format) }
            return items
        }
        
        var archive: ArchiveProxy?
        var fileTypes: [String]?
    
        // Create D64 archive from drive
        guard let d64archive = D64ArchiveProxy.archive(fromVC1541: c64.vc1541) else {
            NSLog("Failed to create D64 archive from disk in drive")
            return false
        }
        
        // Convert D64 archive to target format
        switch (type) {

        case D64_CONTAINER:
            fileTypes = ["D64"]
            archive = d64archive
            break
    
        case T64_CONTAINER:
            fileTypes = ["T64"]
            archive = T64ArchiveProxy.archive(fromArchive: d64archive)
            break
    
        case PRG_CONTAINER:
            
            // PRG files store exactly one file. Abort if disk is empty
            if numberOfItems(archive: d64archive, format: "PRG") == 0 {
                return false
            }
            NSLog("Exporting to PRG format")
            fileTypes = ["PRG"]
            archive = PRGArchiveProxy.archive(fromArchive: d64archive)
            break
    
        case P00_CONTAINER:
            
            // P00 files store exactly one file. Abort if disk is empty
            if numberOfItems(archive: d64archive, format: "P00") == 0 {
                return false
            }
            fileTypes = ["P00"]
            archive = P00ArchiveProxy.archive(fromArchive: d64archive)
            break
    
        default:
            assert(false)
            return false
        }
    
        if archive == nil {
            NSLog("Unable to create archive from disk data")
            return false
        }
        
        // Open save panel
        let sPanel = NSSavePanel()
        sPanel.canSelectHiddenExtension = true
        sPanel.allowedFileTypes = fileTypes
        if sPanel.runModal() != .OK {
            return false
        }
        
        // Write to file
        let selectedURL = sPanel.url
        let selectedFileURL = selectedURL?.absoluteString
        guard let selectedFile = selectedFileURL?.replacingOccurrences(of: "file://", with: "") else {
            NSLog("Unable to extract filename")
            return false
        }
        
        NSLog("Exporting to file \(selectedFile)")

        archive!.write(toFile: selectedFile)
        c64.vc1541.disk.setModified(false)
        
        return true
    }
    
    // -----------------------------------------------------------------
    // Action methods (Cartridge menu)
    // -----------------------------------------------------------------

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
    // Action methods (Debug)
    //
    
    @IBAction func hideSpritesAction(_ sender: Any!) {

        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.hideSpritesAction(sender)
        }
        
        c64.vic.setHideSprites(!c64.vic.hideSprites())
    }
  
    @IBAction func markIRQLinesAction(_ sender: Any!) {
    
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.markIRQLinesAction(sender)
        }
        
        c64.vic.setShowIrqLines(!c64.vic.showIrqLines())
    }
    
    @IBAction func markDMALinesAction(_ sender: Any!) {
    
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.markDMALinesAction(sender)
        }
        
        c64.vic.setShowDmaLines(!c64.vic.showDmaLines())
    }
    
    @IBAction func traceC64CpuAction(_ sender: Any!) {
        
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceC64CpuAction(sender)
        }
        
        c64.cpu.setTraceMode(!c64.cpu.tracingEnabled())
    }
  
    @IBAction func traceIecAction(_ sender: Any!) {
        
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceIecAction(sender)
        }
        
        c64.iec.setTraceMode(!c64.iec.tracingEnabled())
    }
 
    @IBAction func traceVC1541CpuAction(_ sender: Any!) {
        
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceVC1541CpuAction(sender)
        }
        
        c64.vc1541.cpu.setTraceMode(!c64.vc1541.cpu.tracingEnabled())
    }
  
    @IBAction func traceViaAction(_ sender: Any!) {
        
        let undo = undoManager()
        undo?.registerUndo(withTarget: self) {
            targetSelf in targetSelf.traceViaAction(sender)
        }
        
        c64.vc1541.via1.setTraceMode(!c64.vc1541.via1.tracingEnabled())
        c64.vc1541.via2.setTraceMode(!c64.vc1541.via2.tracingEnabled())
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
    @IBAction func dumpVC1541Memory(_ sender: Any!) { c64.vc1541.mem.dump() }
    @IBAction func dumpKeyboard(_ sender: Any!) { c64.keyboard.dump() }
    @IBAction func dumpC64JoystickA(_ sender: Any!) { c64.joystickA.dump() }
    @IBAction func dumpC64JoystickB(_ sender: Any!) { c64.joystickB.dump(); gamePadManager.listDevices()}
    @IBAction func dumpIEC(_ sender: Any!) { c64.iec.dump() }
    @IBAction func dumpC64ExpansionPort(_ sender: Any!) { c64.expansionport.dump() }
    
    // -----------------------------------------------------------------
    // Action methods (Toolbar)
    // -----------------------------------------------------------------
    
    @IBAction @objc func resetAction(_ sender: Any!) {
        
        let document = self.document as! MyDocument
        document.updateChangeCount(.changeDone)
        
        rotateBack()
        c64.powerUp()
        refresh()
    }
    
}
