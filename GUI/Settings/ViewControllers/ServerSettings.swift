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
    @IBOutlet weak var rshConnect: NSButton!
    @IBOutlet weak var rshStatus: NSTextField!
    @IBOutlet weak var rshPort: NSTextField!
    @IBOutlet weak var rpcConnect: NSButton!
    @IBOutlet weak var rpcStatus: NSTextField!
    @IBOutlet weak var rpcPort: NSTextField!

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        func prettyPrint(state: SrvState) -> String {

            switch state {
            case .OFF:       return "Disconnected"
            case .STARTING:  return "Starting"
            case .LISTENING: return "Listening"
            case .CONNECTED: return "Connected"
            case .STOPPING:  return "Stopping"
            default:         return "Invalid"
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
        rshConnect.state = config.rshSeverAutorun ? .on : .off
        rshPort.integerValue = Int(config.rshSeverPort)
        rshStatus.stringValue = prettyPrint(state: serverInfo.rshInfo.state)

        // RPC server
        rpcConnect.state = config.rpcSeverAutorun ? .on : .off
        rpcPort.integerValue = Int(config.rpcSeverPort)
        rpcStatus.stringValue = prettyPrint(state: serverInfo.rpcInfo.state)
    }

    @IBAction func connectAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0: config?.rshSeverAutorun = sender.state == .on
        case 1: config?.rpcSeverAutorun = sender.state == .on
        case 2: config?.dapSeverAutorun = sender.state == .on
        case 3: config?.promSeverAutorun = sender.state == .on
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
