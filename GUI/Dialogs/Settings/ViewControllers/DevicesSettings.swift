// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DevicesSettingsViewController: SettingsViewController {

    @IBOutlet weak var selector: NSSegmentedControl!

    // Property Box
    @IBOutlet weak var infoBox: NSBox!
    @IBOutlet weak var infoBoxTitle: NSTextField!
    @IBOutlet weak var manufacturer: NSTextField!
    @IBOutlet weak var product: NSTextField!
    @IBOutlet weak var version: NSTextField!
    @IBOutlet weak var vendorID: NSTextField!
    @IBOutlet weak var productID: NSTextField!
    @IBOutlet weak var transport: NSTextField!
    @IBOutlet weak var usage: NSTextField!
    @IBOutlet weak var usagePage: NSTextField!
    @IBOutlet weak var locationID: NSTextField!
    @IBOutlet weak var uniqueID: NSTextField!

    // Controller
    @IBOutlet weak var image: NSImageView!
    @IBOutlet weak var action: NSTextField!
    @IBOutlet weak var action2: NSTextField!
    @IBOutlet weak var hidEvent: NSTextField!
    @IBOutlet weak var hidMapping: NSTextView!
    @IBOutlet weak var hidMappingScrollView: NSScrollView!

    /*

    var pad: GamePad? { return gamePadManager?.gamePads[selector.selectedTag()] }
    var db: DeviceDatabase { return myAppDelegate.database }
    var guid: GUID {return pad?.guid ?? GUID() }

    func property(_ key: String) -> String {
        return pad?.property(key: key) ?? "-"
    }

    var usageDescription: String {
        return pad?.device?.usageDescription ?? property(kIOHIDPrimaryUsageKey)
    }

    //
    // Methods from SettingsViewController
    //

    override func activate() {

        action.stringValue = ""
        action2.stringValue = ""
        hidEvent.stringValue = ""

        refresh()
    }

    override func refresh() {

        super.refresh()

        // Device properties
        manufacturer.stringValue = property(kIOHIDManufacturerKey)
        product.stringValue = property(kIOHIDProductKey)
        version.stringValue = property(kIOHIDVersionNumberKey)
        vendorID.stringValue = property(kIOHIDVendorIDKey)
        productID.stringValue = property(kIOHIDProductIDKey)
        transport.stringValue = property(kIOHIDTransportKey)
        usagePage.stringValue = property(kIOHIDPrimaryUsagePageKey)
        locationID.stringValue = property(kIOHIDLocationIDKey)
        uniqueID.stringValue = property(kIOHIDUniqueIDKey)
        usage.stringValue = usageDescription

        // Controller mapping
        hidMapping.focusRingType = .none
        hidMapping.string = pad?.db.seek(guid: guid, withDelimiter: ",\n") ?? ""

        // Information messages
        if pad?.isKnown == true {
            infoBoxTitle.stringValue = ""
            infoBoxTitle.textColor = .secondaryLabelColor
        } else if pad?.isKnown == false {
            infoBoxTitle.stringValue = "This device is not known to the emulator. It may or may not work."
            infoBoxTitle.textColor = .warning
        } else {
            infoBoxTitle.stringValue = "Not connected"
            infoBoxTitle.textColor = .secondaryLabelColor
        }

        // Hide some controls
        let hide = pad == nil || pad?.isMouse == true
        image.isHidden = hide
        hidMappingScrollView.isHidden = hide
        hidEvent.isHidden = hide
        action.isHidden = hide
        action2.isHidden = hide
    }

    func refreshDeviceEvent(event: HIDEvent, nr: Int, value: Int) {

        var text = ""

        switch event {

        case .BUTTON: text = "b\(nr) = \(value)"
        case .AXIS: text = "a\(nr) = \(value)"
        case .DPAD_UP: text = "DPad = Up"
        case .DPAD_DOWN: text = "DPad = Down"
        case .DPAD_RIGHT: text = "DPad = Right"
        case .DPAD_LEFT: text = "DPad = Left"
        case .HATSWITCH: text = "h\(nr).\(value)"
        default: text = ""
        }

        hidEvent?.stringValue = text
    }

    func refreshDeviceActions(actions: [GamePadAction]) {

        var activity = "", activity2 = ""

        func add(_ str: String) {
            if activity == "" { activity += str } else { activity2 += str }
        }

        if actions.contains(.PULL_UP) { add(" Pull Up ") }
        if actions.contains(.PULL_DOWN) { add(" Pull Down ") }
        if actions.contains(.PULL_RIGHT) { add(" Pull Right ") }
        if actions.contains(.PULL_LEFT) { add(" Pull Left ") }
        if actions.contains(.PRESS_FIRE) { add(" Press Fire ") }
        if actions.contains(.RELEASE_X) { add(" Release X Axis ") }
        if actions.contains(.RELEASE_Y) { add(" Release Y Axis ") }
        if actions.contains(.RELEASE_XY) { add(" Release Axis ") }
        if actions.contains(.RELEASE_FIRE) { add(" Release Fire ") }

        action?.stringValue = activity
        action2?.stringValue = activity2
    }

    //
    // Action methods (Misc)
    //

    @IBAction func selectDeviceAction(_ sender: Any!) {

        hidEvent.stringValue = ""
        action.stringValue = ""
        action2.stringValue = ""
        refresh()
    }

    override func preset(tag: Int) {

        // Reset the database
        myAppDelegate.database.reset()

        // Make the change effective
        gamePadManager?.updateHidMapping()

        refresh()
    }

    override func save() {

        pref.saveDevicesUserDefaults()
        myAppDelegate.database.save()
    }
     */

}

/*
extension DevicesSettingsViewController : NSTextViewDelegate {

    func textDidChange(_ notification: Notification) {

        if let textView = notification.object as? NSTextView {

            // Add the update device description to the 'custom' database
            db.update(line: textView.string)

            // Make the change effective
            gamePadManager?.updateHidMapping()
        }
    }
}
*/
