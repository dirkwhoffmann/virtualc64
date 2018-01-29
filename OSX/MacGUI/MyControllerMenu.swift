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
        
        // File menu
        if menuItem.action == #selector(MyController.exportDiskDialog(_:)) {
            return c64.vc1541.hasDisk()
        }
        if menuItem.action == #selector(MyController.exportFileFromDiskDialog(_:)) {
            // TODO: Check how many files are present.
            //       Only enable items when a single file is present
            return c64.vc1541.hasDisk()
        }
        
        // View menu
        if menuItem.action == #selector(MyController.toggleStatusBarAction(_:)) {
            menuItem.title = statusBar ? "Hide Status Bar" : "Show Status Bar"
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
        
        // Cartridge menu
        if menuItem.action == #selector(MyController.finalCartridgeIIIaction(_:)) {
            return c64.expansionport.cartridgeType() == CRT_FINAL_CARTRIDGE_III
        }
        
        return true
    }
   
    //
    // Action methods (File)
    //
    
    @IBAction func insertBlankDisk(_ sender: Any!) {
        
        print("\(#function)")

        // Create empty Archive
        let archive = ArchiveProxy()
        
        // Convert to D64Archive
        let d64archive = D64ArchiveProxy.archive(fromArchive: archive)
        
        // Insert as disk
        c64.mountArchive(d64archive)
    }
    
    //
    // Action methods (Keyboard)
    //
    
    @IBAction func runstopAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_RUNSTOP))
    }
    
    @IBAction func shiftRunstopAction(_ sender: Any!) {
        simulateUserPressingKey(withShift: C64KeyFingerprint(C64KEY_RUNSTOP))
    }
    
    @IBAction func restoreAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_RESTORE))
    }
    
    @IBAction func runstopRestoreAction(_ sender: Any!) {
        simulateUserPressingKey(withRunstop:C64KeyFingerprint(C64KEY_RESTORE))
    }
    
    @IBAction func commodoreKeyAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_COMMODORE))
    }

    @IBAction func commodoreRunstopKeyAction(_ sender: Any!) {
        simulateUserPressingKey(withRunstop:C64KeyFingerprint(C64KEY_COMMODORE))
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
    
    @IBAction func leftarrowAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_LEFTARROW))
    }
    
    @IBAction func uparrowAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_UPARROW))
    }

    @IBAction func powndAction(_ sender: Any!) {
        simulateUserPressingKey(C64KeyFingerprint(C64KEY_POUND))
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

    
    //
    // Action methods (Cartridge)
    //

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
    
}
