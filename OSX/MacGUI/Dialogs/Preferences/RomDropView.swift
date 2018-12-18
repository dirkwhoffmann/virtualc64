//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension NSDraggingInfo
{
    var url: URL? {
        let pasteBoard = draggingPasteboard
        let types = [NSPasteboard.PasteboardType.compatibleFileURL]
        if let _ = pasteBoard.availableType(from: types) {
            return NSURL.init(from: pasteBoard) as URL?
        }
        return nil
    }
}

class RomDropView : NSImageView
{
    @IBOutlet var dialogController: UserDialogController!

    func acceptDragSource(url: URL) -> Bool {
        return proxy?.isRom(url) ?? false
    }
    
    override func awakeFromNib()
    {
        registerForDraggedTypes([NSPasteboard.PasteboardType.compatibleFileURL])
    }
    
    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation
    {
        if let url = sender.url {
            if acceptDragSource(url: url) {
                image = NSImage.init(named: "rom_medium")
                return .copy
            }
        }
        return NSDragOperation()
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?)
    {
        dialogController.refresh()
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        return true
    }

    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        guard let url = sender.url else { return false }
        guard let controller = myController else { return false }
        guard let c64 = proxy else { return false }
        
        if !controller.loadRom(url) {
            return false
        }
        if c64.isRunnable() {
            dialogController.okAction(self)
        }
        return true
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?)
    {
        dialogController.refresh()
    }
}

class BasicRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return proxy?.isBasicRom(url) ?? false
    }
}

class CharRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return proxy?.isCharRom(url) ?? false
    }
}

class KernalRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return proxy?.isKernalRom(url) ?? false
    }
}

class Vc1541RomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return proxy?.isVC1541Rom(url) ?? false
    }
}
