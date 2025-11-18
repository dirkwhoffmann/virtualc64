/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ServerSettingsViewController: SettingsViewController {

    // Remote Shell server
    @IBOutlet weak var rshEnable: NSButton!
    @IBOutlet weak var rshTransport: NSPopUpButton!
    @IBOutlet weak var rshPort: RangeField!
    @IBOutlet weak var rshPath: NSTextField!
    @IBOutlet weak var rshStatusIcon: NSImageView!
    @IBOutlet weak var rshStatusText: NSTextField!
    @IBOutlet weak var rshInfo: NSTextField!

    // RPC server
    @IBOutlet weak var rpcEnable: NSButton!
    @IBOutlet weak var rpcTransport: NSPopUpButton!
    @IBOutlet weak var rpcPort: RangeField!
    @IBOutlet weak var rpcPath: NSTextField!
    @IBOutlet weak var rpcStatusIcon: NSImageView!
    @IBOutlet weak var rpcStatusText: NSTextField!
    @IBOutlet weak var rpcInfo: NSTextField!

    // DAP server
    @IBOutlet weak var dapEnable: NSButton!
    @IBOutlet weak var dapTransport: NSPopUpButton!
    @IBOutlet weak var dapPort: RangeField!
    @IBOutlet weak var dapPath: NSTextField!
    @IBOutlet weak var dapStatusIcon: NSImageView!
    @IBOutlet weak var dapStatusText: NSTextField!
    @IBOutlet weak var dapInfo: NSTextField!

    // PROM server
    @IBOutlet weak var promEnable: NSButton!
    @IBOutlet weak var promTransport: NSPopUpButton!
    @IBOutlet weak var promPort: RangeField!
    @IBOutlet weak var promPath: NSTextField!
    @IBOutlet weak var promStatusIcon: NSImageView!
    @IBOutlet weak var promStatusText: NSTextField!
    @IBOutlet weak var promInfo: NSTextField!

    override func viewDidLoad() {

        log(.lifetime)
        [rshPort, rpcPort, dapPort, promPort].forEach { $0.setRange(0...65535) }
    }

    override func refresh() {

        func prettyPrint(state: SrvState) -> (String, NSColor) {

            switch state {
            case .OFF:          return ("Disconnected", .systemRed)
            case .STARTING:     return ("Starting",     .systemYellow)
            case .LISTENING:    return ("Listening",    .systemYellow)
            case .CONNECTED:    return ("Connected",    .systemGreen)
            case .STOPPING:     return ("Stopping",     .systemYellow)
            case .INVALID:      return ("Invalid",      .systemRed)
            default:            return ("???",          .textColor)
            }
        }

        func update(_ icon: NSImageView, _ text: NSTextField, state: SrvState) {

            let format = prettyPrint(state: state)
            icon.contentTintColor = format.1
            text.stringValue = format.0
            // text.font = NSFont.boldSystemFont(ofSize: text.font?.pointSize ?? NSFont.systemFontSize)
        }

        super.refresh()

        guard let emu = emu, let config = config else { return }
        let serverInfo = emu.remoteManager.info

        // Enable
        rshEnable.state = config.rshServerEnable ? .on : .off
        rpcEnable.state = config.rpcServerEnable ? .on : .off
        dapEnable.state = config.dapServerEnable ? .on : .off
        promEnable.state = config.promServerEnable ? .on : .off

        // Transport
        rshTransport.selectItem(withTag: config.rshServerTransport)
        rpcTransport.selectItem(withTag: config.rpcServerTransport)
        dapTransport.selectItem(withTag: config.dapServerTransport)
        promTransport.selectItem(withTag: config.promServerTransport)

        // Port
        rshPort.integerValue = Int(config.rshServerPort)
        rpcPort.integerValue = Int(config.rpcServerPort)
        dapPort.integerValue = Int(config.dapServerPort)
        promPort.integerValue = Int(config.promServerPort)
        rshPort.isEnabled = config.rshServerEnable
        rpcPort.isEnabled = config.rpcServerEnable
        dapPort.isEnabled = config.dapServerEnable
        promPort.isEnabled = config.promServerEnable
        rshPort.isHidden = config.rshServerTransport == 0
        rpcPort.isHidden = config.rpcServerTransport == 0
        dapPort.isHidden = config.dapServerTransport == 0
        promPort.isHidden = config.promServerTransport == 0

        // Path
        rpcPath.stringValue = "/  rpc"
        promPath.stringValue = "/  metrics"
        rshPath.isHidden = config.rshServerTransport != 2
        rpcPath.isHidden = config.rpcServerTransport != 2
        dapPath.isHidden = config.dapServerTransport != 2
        promPath.isHidden = config.promServerTransport != 2
        rshPath.textColor = config.rshServerEnable ? .textColor : .secondaryLabelColor
        rpcPath.textColor = config.rpcServerEnable ? .textColor : .secondaryLabelColor
        dapPath.textColor = config.dapServerEnable ? .textColor : .secondaryLabelColor
        promPath.textColor = config.promServerEnable ? .textColor : .secondaryLabelColor

        // Connection status
        update(rshStatusIcon, rshStatusText, state: serverInfo.rshInfo.state)
        update(rpcStatusIcon, rpcStatusText, state: serverInfo.rpcInfo.state)
        update(dapStatusIcon, dapStatusText, state: serverInfo.dapInfo.state)
        update(promStatusIcon, promStatusText, state: serverInfo.promInfo.state)

        // Server description
        rshInfo.textColor = config.rshServerEnable ? .textColor : .secondaryLabelColor
        rpcInfo.textColor = config.rpcServerEnable ? .textColor : .secondaryLabelColor
        dapInfo.textColor = config.dapServerEnable ? .textColor : .secondaryLabelColor
        promInfo.textColor = config.promServerEnable ? .textColor : .secondaryLabelColor
    }

    @IBAction func enableAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0: config?.rshServerEnable = sender.state == .on
        case 1: config?.rpcServerEnable = sender.state == .on
        case 2: config?.dapServerEnable = sender.state == .on
        case 3: config?.promServerEnable = sender.state == .on
        default: fatalError()
        }
    }

    @IBAction func transportAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0: config?.rshServerTransport = sender.selectedTag()
        case 1: config?.rpcServerTransport = sender.selectedTag()
        case 2: config?.dapServerTransport = sender.selectedTag()
        case 3: config?.promServerTransport = sender.selectedTag()
        default: fatalError()
        }
    }

    @IBAction func portAction(_ sender: NSTextField!) {

        switch sender.tag {
        case 0: config?.rshServerPort = sender.integerValue
        case 1: config?.rpcServerPort = sender.integerValue
        case 2: config?.dapServerPort = sender.integerValue
        case 3: config?.promServerPort = sender.integerValue
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
