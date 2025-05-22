// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSDraggingInfo {

    var url: URL? {
        let pasteBoard = draggingPasteboard
        let types = [NSPasteboard.PasteboardType.fileURL]
        if pasteBoard.availableType(from: types) != nil {
            return NSURL(from: pasteBoard) as URL?
        }
        return nil
    }
}

class DropView: NSImageView {

    @IBOutlet var parent: ConfigurationController!
    var emu: EmulatorProxy? { return parent.emu }

    override func awakeFromNib() {

        registerForDraggedTypes([NSPasteboard.PasteboardType.fileURL])
    }
    
    func acceptDragSource(url: URL) -> Bool { return false }

    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {

        if let url = sender.url {
            if acceptDragSource(url: url) {
                image = NSImage(named: "rom_medium")
                return .copy
            }
        }
        return NSDragOperation()
    }

    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {

        return false
    }

    override func draggingExited(_ sender: NSDraggingInfo?) {

        parent.refresh()
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool {

        return true
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?) {

        parent.refresh()
    }
}

class RomDropView: DropView {

    func performDrag(type: vc64.RomType, url: URL?) -> Bool {

        guard let url = url?.unpacked else { return false }

        do {

            let rom = try MediaFileProxy.make(with: url)
            emu?.loadRom(rom)

            return true

        } catch {

            parent.parent.showAlert(.cantOpen(url: url),
                                    error: error,
                                    async: true,
                                    window: parent.window)
            return false
        }
    }
}

class BasicRomDropView: RomDropView {

    override func acceptDragSource(url: URL) -> Bool {
        return emu != nil && emu!.poweredOff && emu!.isRom(.BASIC, url: url)
    }
    
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        return performDrag(type: .BASIC, url: sender.url)
    }
}

class CharRomDropView: RomDropView {
    override func acceptDragSource(url: URL) -> Bool {
        return emu != nil && emu!.poweredOff && emu!.isRom(.CHAR, url: url)
    }
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        return performDrag(type: .CHAR, url: sender.url)
    }
}

class KernalRomDropView: RomDropView {

    override func acceptDragSource(url: URL) -> Bool {
        return emu != nil && emu!.poweredOff && emu!.isRom(.KERNAL, url: url)
    }
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        return performDrag(type: .KERNAL, url: sender.url)
    }
}

class Vc1541RomDropView: RomDropView {
    
    override func acceptDragSource(url: URL) -> Bool {
        return emu != nil && emu!.poweredOff && emu!.isRom(.VC1541, url: url)
    }
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        return performDrag(type: .VC1541, url: sender.url)
    }
}
