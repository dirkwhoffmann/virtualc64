/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ServerSettingsViewController: SettingsViewController {

    // Drive
    @IBOutlet weak var rshEnable: NSButton!
    @IBOutlet weak var rshStatus: NSTextField!
    @IBOutlet weak var rshPort: NSTextField!
    @IBOutlet weak var rpcEnable: NSButton!
    @IBOutlet weak var rpcStatus: NSTextField!
    @IBOutlet weak var rpcPort: NSTextField!
    @IBOutlet weak var dapEnable: NSButton!
    @IBOutlet weak var dapStatus: NSTextField!
    @IBOutlet weak var dapPort: NSTextField!
    @IBOutlet weak var promEnable: NSButton!
    @IBOutlet weak var promStatus: NSTextField!
    @IBOutlet weak var promPort: NSTextField!

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        func prettyPrint(state: SrvState) -> String {

            switch state {
            case .OFF:          return "Disconnected"
            case .SURVEILLING:  return "Surveilling"
            case .STARTING:     return "Starting"
            case .LISTENING:    return "Listening"
            case .CONNECTED:    return "Connected"
            case .STOPPING:     return "Stopping"
            case .INVALID:      return "Invalid"
            default:            return "???"
            }
        }

        super.refresh()

        guard let emu = emu, let config = config else { return }

        func update(_ component: NSTextField, enable: Bool) {
            component.textColor = enable ? .controlTextColor : .disabledControlTextColor
            component.isEnabled = enable
        }
        func update(_ component: NSControl, enable: Bool) {
            component.isEnabled = enable
        }

        let serverInfo = emu.remoteManager.info

        // RSH server
        rshEnable.state = config.rshSeverEnable ? .on : .off
        rshPort.integerValue = Int(config.rshSeverPort)
        rshStatus.stringValue = prettyPrint(state: serverInfo.rshInfo.state)

        // RPC server
        rpcEnable.state = config.rpcSeverEnable ? .on : .off
        rpcPort.integerValue = Int(config.rpcSeverPort)
        rpcStatus.stringValue = prettyPrint(state: serverInfo.rpcInfo.state)
    }

    @IBAction func enableAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0: config?.rshSeverEnable = sender.state == .on
        case 1: config?.rpcSeverEnable = sender.state == .on
        case 2: config?.dapSeverEnable = sender.state == .on
        case 3: config?.promSeverEnable = sender.state == .on
        default: fatalError()
        }
    }

    @IBAction func portAction(_ sender: NSTextField!) {

        switch sender.tag {
        case 0: config?.rshSeverPort = sender.integerValue
        case 1: config?.rpcSeverPort = sender.integerValue
        case 2: config?.dapSeverPort = sender.integerValue
        case 3: config?.promSeverPort = sender.integerValue
        default: fatalError()
        }
    }


    //
    // Presets and Saving
    //
    
    override func preset(tag: Int) {

        guard let emu = emu else { return }

        emu.suspend()

        // Revert to standard settings
        EmulatorProxy.defaults.removeServerUserDefaults()

        // Update the configuration
        config?.applyServerUserDefaults()

        emu.resume()
    }

    override func save() {

        config?.saveServerUserDefaults()
    }
}
