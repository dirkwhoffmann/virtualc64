// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

class DiskImporter: DialogController {

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var checkbox: NSButton!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var subtitle1: NSTextField!
    @IBOutlet weak var subtitle2: NSTextField!
    @IBOutlet weak var subtitle3: NSTextField!
    @IBOutlet weak var drive8: NSButton!
    @IBOutlet weak var drive9: NSButton!
    @IBOutlet weak var flash: NSPopUpButton!
    @IBOutlet weak var flashLabel: NSTextField!

    // Dragged in URL
    var url: URL!

    // Media file to process
    var file: AnyFileProxy?

    // Extracted file system
    var volume: FileSystemProxy?

    var type: FileType!
    var writeProtect: Bool { return checkbox.state == .on }
    var autoRun: Bool { return checkbox.state == .on }

    var titleString = "???"
    var subtitle1String = "???"
    var subtitle2String = ""
    var subtitle3String = ""
    var supportedCrt = true

    // Shortcuts
    var myDocument: MyDocument { return parent.mydocument! }

    func show(url: URL) throws {

        debug(.lifetime)

        // Load media file
        file = try mm.createFileProxy(from: url, allowedTypes: FileType.draggable)

        // Remember the URL
        self.url = url

        showSheet()
    }

    override func showSheet(completionHandler handler:(() -> Void)? = nil) {

        switch file {

        case let file as CRTFileProxy:

            titleString = "Commodore Expansion Port Module"
            subtitle1String = file.packageInfo
            subtitle2String = file.lineInfo
            subtitle3String = ""

            if !file.isSupported {
                subtitle3String = "This cartridge is not supported by the emulator yet"
                supportedCrt = false
            }

        case let file as TAPFileProxy:

            titleString = "Commodore Cassette Tape"
            subtitle1String = file.version.description
            subtitle2String = ""
            subtitle3String = ""

        case let file as D64FileProxy:

            volume = try? FileSystemProxy.make(with: file)

            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is G64FileProxy:

            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = "A bit-accurate image of a C64 diskette"
            subtitle2String = ""
            subtitle3String = ""

        case let file as T64FileProxy:

            volume = try? FileSystemProxy.make(with: file)

            titleString = "Commodore 64 Floppy Disk (from T64 file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case let file as PRGFileProxy:

            volume = try? FileSystemProxy.make(with: file)

            titleString = "Commodore 64 Floppy Disk (from PRG file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case let file as P00FileProxy:

            volume = try? FileSystemProxy.make(with: file)

            titleString = "Commodore 64 Floppy Disk (from P00 file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case let file as FolderProxy:

            volume = try? FileSystemProxy.make(with: file)

            titleString = "Disk from a file system folder"
            subtitle1String = "Comprises all PRG files found in this directory"
            subtitle2String = ""
            subtitle3String = ""

        default:
            fatalError()
        }

        if volume != nil {

            volume?.info()
            volume?.printDirectory()
        }

        super.showSheet(completionHandler: handler)
    }

    override public func awakeFromNib() {

        super.awakeFromNib()
        refresh()
    }

    override func windowDidLoad() {

        let connected8 = parent.config.drive8Connected
        let connected9 = parent.config.drive9Connected

        title.stringValue = titleString
        subtitle1.stringValue = subtitle1String
        subtitle2.stringValue = subtitle2String
        subtitle3.stringValue = subtitle3String

        let numberOfItems = setUpFlashItems()
        flash.isHidden = numberOfItems == 0
        flashLabel.isHidden = numberOfItems == 0

        switch file {

        case is TAPFileProxy:

            checkbox.title = "Auto load"
            drive8.isHidden = true
            drive9.title = "Insert"
            drive9.isHidden = false
            drive9.keyEquivalent = "\r"

        case is CRTFileProxy:

            checkbox.isHidden = true
            drive8.isHidden = true
            drive9.title = "Attach"
            drive9.isHidden = false
            drive9.keyEquivalent = "\r"
            drive9.isEnabled = supportedCrt
            if !supportedCrt { subtitle3.textColor = .warningColor }

        default:

            checkbox.title = "Write protect"
            drive8.isEnabled = connected8
            drive8.keyEquivalent = "\r"
            drive8.isHidden = false
            drive9.isEnabled = connected9
            drive9.keyEquivalent = ""
            drive9.isHidden = false
        }
    }

    func setUpFlashItems() -> Int {

        guard let v = volume else { return 0 }

        flash.removeAllItems()
        flash.font = .monospaced(ofSize: 13.0, weight: .semibold)

        var seen: [String] = []
        var item = 0
        for i in  0 ..< v.numFiles {

            let size = v.fileBlocks(i) // TODO:.itemBlocks(i)
            let name = v.fileName(i) ?? "<null>"
            let type = v.fileType(i)

            var title = "\(size)"
            title = title.padding(toLength: 5, withPad: " ", startingAt: 0)
            title += "\"\(name)\""
            title = title.padding(toLength: 24, withPad: " ", startingAt: 0)
            title += "\(type.description)"
            title = title.padding(toLength: 28, withPad: " ", startingAt: 0)

            if !seen.contains(title) {
                seen.append(title)
                flash.addItem(withTitle: title)
                flash.item(at: item)!.tag = i
                flash.item(at: item)!.isEnabled = type == .PRG
                item += 1
            }
        }
        flash.autoenablesItems = false
        return flash.numberOfItems
    }

    func refresh() {

        icon.image = file?.icon(protected: writeProtect)
    }

    //
    // Action methods
    //

    @IBAction func checkboxAction(_ sender: NSButton!) {

        refresh()
    }

    @IBAction func insertAction(_ sender: NSButton!) {

        do {

            switch file {

            case is FolderProxy:

                debug(.media, "Inserting file system (wp: \(writeProtect))")
                let drive = sender.tag == 0 ? c64.drive8! : c64.drive9!
                drive.insertFileSystem(volume!, protected: writeProtect)

            default:

                debug(.media, "Inserting media file \(url!) (wp: \(writeProtect))")

                var options: [MediaManager.Option] = []

                if autoRun { options.append(.autostart) }
                if writeProtect { options.append(.protect) }
                options.append(.reset)
                options.append(.remember)

                try mm.addMedia(url: url,
                            allowedTypes: FileType.draggable,
                            drive: sender.tag,
                            options: options)
            }
                /*
            } else if tap != nil {

                debug(.media, "Inserting Tape")
                let options: [MediaManager.Option] = autoRun ? [ .autostart ] : []
                try mm.addMedia(proxy: tap!, options: options)

            } else if crt != nil {

                debug(.media, "Attaching Cartridge")
                let options: [MediaManager.Option] = [ .reset ]
                try mm.addMedia(proxy: crt!, options: options)

            } else if g64 != nil {

                debug(.media, "Inserting G64")
                let options: [MediaManager.Option] = writeProtect ? [ .protect ] : []
                try mm.addMedia(proxy: g64!, drive: drive, options: options)

            } else {

                fatalError()
            }
                 */

        } catch {

            NSSound.beep()
        }

        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {

        debug(.media, "flashAction: \(sender.selectedTag())")

        try? parent.c64.flash(volume!, item: sender.selectedTag())
        parent.keyboard.type("RUN\n")
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction override func cancelAction(_ sender: Any!) {

        hideSheet()
    }
}

extension DiskImporter {

    func windowDidResize(_ notification: Notification) {

    }

    func windowWillStartLiveResize(_ notification: Notification) {

    }

    func windowDidEndLiveResize(_ notification: Notification) {

    }
}
