// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
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
    @IBOutlet weak var carousel: iCarousel!

    // Proxy objects (created from the attachment when the sheet opens)
    var g64: G64FileProxy?
    var tap: TAPFileProxy?
    var crt: CRTFileProxy?
    var disk: DiskProxy?
    var volume: FSDeviceProxy?
    
    var type: FileType!
    var writeProtect: Bool { return checkbox.state == .on }
    var autoRun: Bool { return checkbox.state == .on }
    
    var screenshots: [Screenshot] = []
    
    // Custom font
    var monofont: NSFont {
        if #available(OSX 10.15, *) {
            return NSFont.monospacedSystemFont(ofSize: 13.0, weight: .semibold)
        } else {
            return NSFont.monospacedDigitSystemFont(ofSize: 13.0, weight: .semibold)
        }
    }
    
    var titleString = "???"
    var subtitle1String = "???"
    var subtitle2String = ""
    var subtitle3String = ""
    var supportedCrt = true
    
    // Shortcuts
    var myDocument: MyDocument { return parent.mydocument! }
    var numItems: Int { return carousel.numberOfItems }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return numItems / 2 }
    var lastItem: Int { return numItems - 1 }
    var empty: Bool { return numItems == 0 }
    
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
                volume = try? Proxy.create(d64: d64) as FSDeviceProxy
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
            
            if let coll = myDocument.attachment as? AnyCollectionProxy {
                volume = try? Proxy.create(collection: coll) as FSDeviceProxy
            }

            titleString = "Commodore 64 Floppy Disk (from T64 file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is PRGFileProxy:
            
            if let coll = myDocument.attachment as? AnyCollectionProxy {
                volume = try? Proxy.create(collection: coll) as FSDeviceProxy
            }

            titleString = "Commodore 64 Floppy Disk (from PRG file)"
            subtitle1String = volume?.layoutInfo ?? ""
            subtitle2String = volume?.dos.description ?? ""
            subtitle3String = volume?.filesInfo ?? ""

        case is P00FileProxy:
            
            if let coll = myDocument.attachment as? AnyCollectionProxy {
                volume = try? Proxy.create(collection: coll) as FSDeviceProxy
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
        
        // Load screenshots (if any)
        let fnv = myDocument.attachment!.fnv
        if fnv != 0 {
            for url in Screenshot.collectFiles(forDisk: fnv) {
                if let screenshot = Screenshot.init(fromUrl: url) {
                    screenshots.append(screenshot)
                }
            }
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
        window?.makeFirstResponder(carousel)
        refresh()
        updateCarousel(goto: centerItem, animated: false)
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

        if empty {

            setHeight(222)
            
        } else {
            
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                self.carousel.type = iCarouselType.coverFlow
                self.carousel.isHidden = false
                self.carousel.scrollToItem(at: self.centerItem, animated: false)
            }
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
    
    func setHeight(_ newHeight: CGFloat) {
        
        var rect = window!.frame
        rect.origin.y += rect.size.height - newHeight
        rect.size.height = newHeight
        
        window?.setFrame(rect, display: true)
        refresh()
    }

    func refresh() {
        
        icon.image = myDocument.attachment?.icon(protected: writeProtect)
    }

    func updateCarousel(goto item: Int = -1, animated: Bool = false) {
        
        carousel.reloadData()
        if item != -1 {
            self.carousel.scrollToItem(at: item, animated: animated)
        }
        carousel.layOutItemViews()
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
            
            track("Inserting Volume")
            drive.insertFileSystem(volume)
            drive.setWriteProtection(writeProtect)
        
        } else if tap != nil {
            
            track("Inserting Tape")
            c64.datasette.insertTape(tap)
            
            if autoRun {
                parent.keyboard.type("LOAD\n")
                c64.datasette.pressPlay()
            }
        
        } else if crt != nil {
            
            track("Inserting Cartridge")
            c64.expansionport.attachCartridge(crt, reset: true)
            
        } else if g64 != nil {
            
            track("Inserting G64")
            drive.insertG64(g64)

        } else {
            
            fatalError()
        }

        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {
        
        track("flashAction: \(sender.selectedTag())")
        
        let collection = myDocument.attachment as! AnyCollectionProxy
        parent.c64.flash(collection, item: sender.selectedTag())

        parent.keyboard.type("RUN\n")
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction override func cancelAction(_ sender: Any!) {
                                  
         hideSheet()
     }
}

extension ImportDialog: NSWindowDelegate {
    
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

//
// iCarousel data source and delegate
//

extension ImportDialog: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
        
        return screenshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        itemView.image = screenshots[index].screen?.roundCorners()
        return itemView
    }
}
