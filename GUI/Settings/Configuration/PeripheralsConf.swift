// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension ConfigurationController {
   
    func refreshPeripheralsTab() {
                        
        track()
                
        // Drive
        perDrive8Connect.state = config.drive8Connected ? .on : .off
        perDrive9Connect.state = config.drive9Connected ? .on : .off
        perDrive8Type.selectItem(withTag: config.drive8Type)
        perDrive9Type.selectItem(withTag: config.drive9Type)
        
        // Ports
        parent.gamePadManager.refresh(popup: perControlPort1, hide: true)
        parent.gamePadManager.refresh(popup: perControlPort2, hide: true)
        perControlPort1.selectItem(withTag: config.gameDevice1)
        perControlPort2.selectItem(withTag: config.gameDevice2)
                
        // USB devices
        let pad1 = parent.gamePadManager.gamePads[3]
        perUSBName1.stringValue = pad1?.property(key: kIOHIDProductKey) ?? "-"
        perUSBManu1.stringValue = pad1?.property(key: kIOHIDManufacturerKey) ?? "-"
        perUSBProductID1.stringValue = pad1?.property(key: kIOHIDProductIDKey) ?? "-"
        perUSBVendorID1.stringValue = pad1?.property(key: kIOHIDVendorIDKey) ?? "-"
        
        let pad2 = parent.gamePadManager.gamePads[4]
        perUSBName2.stringValue = pad2?.property(key: kIOHIDProductKey) ?? "-"
        perUSBManu2.stringValue = pad2?.property(key: kIOHIDManufacturerKey) ?? "-"
        perUSBProductID2.stringValue = pad2?.property(key: kIOHIDProductIDKey) ?? "-"
        perUSBVendorID2.stringValue = pad2?.property(key: kIOHIDVendorIDKey) ?? "-"

        // Power button
        hwPowerButton.isHidden = !bootable
    }
    
    @IBAction func perDriveTypeAction(_ sender: NSPopUpButton!) {
        
        switch sender.tag {
        case 8: config.drive8Type = sender.tag
        case 9: config.drive9Type = sender.tag
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func perDriveConnectAction(_ sender: NSButton!) {
        
        switch sender.tag {
        case 8: config.drive8Connected = sender.state == .on
        case 9: config.drive9Connected = sender.state == .on
        default: fatalError()
        }
        refresh()
    }
    
    @IBAction func perControlPortAction(_ sender: NSPopUpButton!) {
        
        track("port: \(sender.tag) device: \(sender.selectedTag())")
        
        switch sender.tag {
        case 1: config.gameDevice1 = sender.selectedTag()
        case 2: config.gameDevice2 = sender.selectedTag()
        default: fatalError()
        }
        refresh()
    }
}
