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
    var dragImage: NSImageView?

    func acceptDragSource(url: URL) -> Bool {
        return dialogController.c64.isRom(url)
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
                dragImage?.isHidden = true
                return .copy
            }
        }
        return NSDragOperation()
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?)
    {
        dragImage?.isHidden = false
        dialogController.refresh()
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        return true
    }

    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        guard let url = sender.url else {
            return false
        }
        if !dialogController.parent.loadRom(url) {
            return false
        }
        if dialogController.parent.c64.isRunnable() {
            dialogController.okAction(self)
        }
        return true
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?)
    {
        track()
        dialogController.refresh()
    }
}

class BasicRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return dialogController.c64.isBasicRom(url)
    }
}

class CharRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return dialogController.c64.isCharRom(url)
    }
}

class KernalRomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return dialogController.c64.isKernalRom(url)
    }
}

class Vc1541RomDropView : RomDropView
{
    override func acceptDragSource(url: URL) -> Bool {
        return dialogController.c64.isVC1541Rom(url)
    }
}
