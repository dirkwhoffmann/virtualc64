// -----------------------------------------------------------------------------
// This file is part of VirtuaC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class CartridgeInspector: DialogController {

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var type: NSTextField!
    @IBOutlet weak var supported: NSTextField!
    @IBOutlet weak var game: NSTextField!
    @IBOutlet weak var exrom: NSTextField!
    @IBOutlet weak var packets: NSTableView!

    var info: CartridgeInfo?
    var packetInfos: [CartridgeRomInfo] = []

    //
    // Starting up
    //

    func show(expansionPort: ExpansionPortProxy) {

        info = expansionPort.getInfo()

        if let info = info, info.numPackets > 0 {

            for i in 0 ..< info.numPackets {
                packetInfos.append(expansionPort.getRomInfo(i))
            }
        }

        showWindow()
    }

    //
    // Updating the displayed information
    //

    override func sheetWillShow() {

        update()
    }

    func update() {

        title.stringValue = "Commodore Expansion Port Module"
        type.stringValue = info!.type.description
        supported.stringValue = info!.supported ? "Yes" : "No"
        game.stringValue = info!.gameLineInCrtFile ? "High" : "Low"
        exrom.stringValue = info!.exromLineInCrtFile ? "High" : "Low"
    }
}

extension CartridgeInspector: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {

        return packetInfos.count
    }

    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {

        switch tableColumn?.identifier.rawValue {

        case "packet":

            return row

        case "size":

            let kb = Int(packetInfos[row].size) / 1024
            return String(format: "%d KB", kb)

        case "addr":

            let addr = Int(packetInfos[row].loadAddress)
            let size = Int(packetInfos[row].size)
            return String(format: "$%04X - $%04X", addr, addr + size - 1)

        default:

            debug("\(tableColumn!.identifier.rawValue)")
            fatalError()
        }
    }
}

extension CartridgeInspector: NSTableViewDelegate {

    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {

    }

    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {

        return false
    }
}
