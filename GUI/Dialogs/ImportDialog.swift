// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ImportDialog: DialogController {
            
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

    // Proxy objects (created from the attachment when the sheet opens)
    var g64: G64FileProxy?
    var tap: TAPFileProxy?
    var crt: CRTFileProxy?
    var disk: DiskProxy?
    var volume: FileSystemProxy?
    
    var type: FileType!
    var writeProtect: Bool { return checkbox.state == .on }
    var autoRun: Bool { return checkbox.state == .on }

    // Custom font
	var monofont: NSFont { return NSFont.monospaced(ofSize: 13.0, weight: .semibold) }
    
    var titleString = "???"
    var subtitle1String = "???"
    var subtitle2String = ""
    var subtitle3String = ""
    var supportedCrt = true
    
    // Shortcuts
    var myDocument: MyDocument { return parent.mydocument! }

    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
                    
        track()

        switch myDocument.attachment {
             
        case is CRTFileProxy:
            
            crt = myDocument.attachment as? CRTFileProxy
            
            titleString = "Commodore Expansion Port Module"
            subtitle1String = crt!.packageInfo
            subtitle2String = crt!.lineInfo
            subtitle3String = ""
            
            if !crt!.isSupported {
                subtitle3String = "This cartridge is not supported by the emulator yet"
                supportedCrt = false
            }
            
        case is TAPFileProxy:
            
            tap = myDocument.attachment as? TAPFileProxy
            
            titleString = "Commodore Cassette Tape"
            subtitle1String = tap!.version.description
            subtitle2String = ""
            subtitle3String = ""

        case is D64FileProxy:

            if let d64 = myDocument.attachment as? D64FileProxy {
                volume = try? FileSystemProxy.make(with: d64)
            }
                                                                            
            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""
            
        case is G64FileProxy:

            g64 = myDocument.attachment as? G64FileProxy
            
            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = "A bit-accurate image of a C64 diskette"
            subtitle2String = ""
            subtitle3String = ""

        case is T64FileProxy:
            
            if let collection = myDocument.attachment as? AnyCollectionProxy {
                volume = try? FileSystemProxy.make(with: collection)
            }

            titleString = "Commodore 64 Floppy Disk (from T64 file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is PRGFileProxy:
            
            if let collection = myDocument.attachment as? AnyCollectionProxy {
                volume = try? FileSystemProxy.make(with: collection)
            }

            titleString = "Commodore 64 Floppy Disk (from PRG file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is P00FileProxy:
            
            if let collection = myDocument.attachment as? AnyCollectionProxy {
                volume = try? FileSystemProxy.make(with: collection)
            }

            titleString = "Commodore 64 Floppy Disk (from P00 file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is FolderProxy:
            
            if let folder = myDocument.attachment as? FolderProxy {
                volume = folder.fileSystem
            }
            titleString = "Disk from a file system folder"
            subtitle1String = "Comprises all PRG files found in this directory"
            subtitle2String = ""
            subtitle3String = ""
            
        default:
            fatalError()
        }

        if volume != nil {
            track("Volume created successfully")
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
        
        // Configure controls
        if tap != nil {
            
            checkbox.title = "Auto load"
            drive8.isHidden = true
            drive9.title = "Insert"
            drive9.isHidden = false
            drive9.keyEquivalent = "\r"
            
        } else if crt != nil {
            
            checkbox.isHidden = true
            drive8.isHidden = true
            drive9.title = "Attach"
            drive9.isHidden = false
            drive9.keyEquivalent = "\r"
            drive9.isEnabled = supportedCrt
            if !supportedCrt { subtitle3.textColor = .warningColor }
            
        } else {
            
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
        flash.font = monofont
        
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
        
        icon.image = myDocument.attachment?.icon(protected: writeProtect)
    }

    //
    // Action methods
    //
    
    @IBAction func checkboxAction(_ sender: NSButton!) {
        
        refresh()
    }

    @IBAction func insertAction(_ sender: NSButton!) {
        
        let drive = sender.tag == 0 ? c64.drive8! : c64.drive9!
        
        track("insertAction: \(sender.tag)")

        if volume != nil {
            
            track("Inserting Volume (wp: \(writeProtect))")
            drive.insertFileSystem(volume, protected: writeProtect)
        
        } else if tap != nil {
            
            track("Inserting Tape")
            c64.datasette.insertTape(tap)
            
            if autoRun {
                parent.keyboard.type("LOAD\n")
                c64.datasette.pressPlay()
            }
        
        } else if crt != nil {
            
            track("Inserting Cartridge")
            try? c64.expansionport.attachCartridge(crt!, reset: true)
            
        } else if g64 != nil {
            
            track("Inserting G64")
            drive.insertG64(g64, protected: writeProtect)

        } else {
            
            fatalError()
        }

        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {
        
        track("flashAction: \(sender.selectedTag())")
        
        try? parent.c64.flash(volume!, item: sender.selectedTag())
        parent.keyboard.type("RUN\n")
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction override func cancelAction(_ sender: Any!) {
                                  
         hideSheet()
     }
}

extension ImportDialog {
    
    func windowDidResize(_ notification: Notification) {
        
        track()
    }
    
    func windowWillStartLiveResize(_ notification: Notification) {
         
         track()
     }
     
     func windowDidEndLiveResize(_ notification: Notification) {
        
        track()
     }
}
