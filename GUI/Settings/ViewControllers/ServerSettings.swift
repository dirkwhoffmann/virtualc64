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
    @IBOutlet weak var rshPort: RangeField!
    @IBOutlet weak var rpcEnable: NSButton!
    @IBOutlet weak var rpcStatus: NSTextField!
    @IBOutlet weak var rpcPort: RangeField!
    @IBOutlet weak var dapEnable: NSButton!
    @IBOutlet weak var dapStatus: NSTextField!
    @IBOutlet weak var dapPort: RangeField!
    @IBOutlet weak var promEnable: NSButton!
    @IBOutlet weak var promStatus: NSTextField!
    @IBOutlet weak var promPort: RangeField!

    override func viewDidLoad() {

        log(.lifetime)
        [rshPort, rpcPort, dapPort, promPort].forEach { $0.setRange(0...65535) }
    }

    override func refresh() {

        func prettyPrint(state: SrvState) -> (String, NSColor) {

            switch state {
            case .OFF:          return ("Disconnected", .systemRed)
            case .WAITING:      return ("Waiting",      .systemYellow)
            case .STARTING:     return ("Starting",     .systemYellow)
            case .LISTENING:    return ("Listening",    .systemYellow)
            case .CONNECTED:    return ("Connected",    .systemGreen)
            case .STOPPING:     return ("Stopping",     .systemYellow)
            case .INVALID:      return ("Invalid",      .systemRed)
            default:            return ("???",          .textColor)
            }
        }

        func update(_ component: NSTextField, state: SrvState) {

            let format = prettyPrint(state: state)

            component.stringValue = format.0
            component.textColor = format.1
            component.font = NSFont.boldSystemFont(ofSize: component.font?.pointSize ?? NSFont.systemFontSize)

        }
        func update(_ component: NSControl, enable: Bool) {
            component.isEnabled = enable
        }

        super.refresh()

        guard let emu = emu, let config = config else { return }

        let serverInfo = emu.remoteManager.info

        // RSH server
        rshEnable.state = config.rshSeverEnable ? .on : .off
        rshPort.integerValue = Int(config.rshSeverPort)
        update(rshStatus, state: serverInfo.rshInfo.state)

        // RPC server
        rpcEnable.state = config.rpcSeverEnable ? .on : .off
        rpcPort.integerValue = Int(config.rpcSeverPort)
        update(rpcStatus, state: serverInfo.rpcInfo.state)

        // DAP server
        dapEnable.state = config.dapSeverEnable ? .on : .off
        dapPort.integerValue = Int(config.dapSeverPort)
        update(dapStatus, state: serverInfo.dapInfo.state)

        // Prom server
        promEnable.state = config.promSeverEnable ? .on : .off
        promPort.integerValue = Int(config.promSeverPort)
        update(promStatus, state: serverInfo.promInfo.state)
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

        print("portAction \(sender.integerValue)")

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
