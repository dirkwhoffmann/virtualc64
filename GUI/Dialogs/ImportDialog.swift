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
    var d64: D64FileProxy?
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
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
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
    
    var layoutInfo: String {
        
        guard let v = volume else { return "" }
        return "Single sided, single density disk with \(v.numTracks) tracks"
    }

    var dosInfo: String {

        guard let v = volume else { return "" }
        return v.dos.description
    }

    var filesInfo: String {
 
        guard let v = volume else { return "" }
        
        let num = v.numFiles
        let free = v.numUsedBlocks
        let files = num == 1 ? "file" : "files"
        
        return "\(num) \(files), \(free) blocks used"
    }
    
    var crtInfo: String {
        
        guard let c = crt else { return "" }

        let cnt = c.chipCount
        let type = c.cartridgeType
        let packages = cnt == 1 ? "package" : "packages"

        if type == .CRT_NORMAL {
            return "Standard cartridge with \(cnt) chip \(packages)"
        } else {
            return "\(type.description)"
        }
    }
    
    var crtLineInfo: String {
        
        guard let c = crt else { return "" }
        
        let exrom = c.initialExromLine
        let game = c.initialGameLine
        
        return "Exrom line: \(exrom), " + "Game line: \(game)"
    }
    
    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
    
        let type = myDocument.attachment!.typeString() ?? "?"
        track("showSheet(fileType: \(type))")

        switch myDocument.attachment {
             
        case is CRTFileProxy:
            
            crt = myDocument.attachment as? CRTFileProxy
            
            titleString = "Commodore Expansion Port Module"
            subtitle1String = crtInfo
            subtitle2String = crtLineInfo
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

            d64 = myDocument.attachment as? D64FileProxy
            volume = FSDeviceProxy.make(withD64: d64!)
            
            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = layoutInfo
            subtitle2String = dosInfo
            subtitle3String = filesInfo
            
        case is G64FileProxy:

            g64 = myDocument.attachment as? G64FileProxy
            
            titleString = "Commodore 64 Floppy Disk"
            subtitle1String = "A bit-accurate image of a C64 diskette"
            subtitle2String = ""
            subtitle3String = ""

        case is T64FileProxy:
            
            if let c = myDocument.attachment as? AnyCollectionProxy {
                volume = FSDeviceProxy.make(withCollection: c)
            }

            titleString = "Commodore 64 Floppy Disk (from T64 file)"
            subtitle1String = layoutInfo
            subtitle2String = dosInfo
            subtitle3String = filesInfo

        case is PRGFileProxy:
            
            if let c = myDocument.attachment as? AnyCollectionProxy {
                volume = FSDeviceProxy.make(withCollection: c)
            }

            titleString = "Commodore 64 Floppy Disk (from PRG file)"
            subtitle1String = layoutInfo
            subtitle2String = dosInfo
            subtitle3String = filesInfo

        case is P00FileProxy:
            
            if let c = myDocument.attachment as? AnyCollectionProxy {
                volume = FSDeviceProxy.make(withCollection: c)
            }

            titleString = "Commodore 64 Floppy Disk (from P00 file)"
            subtitle1String = layoutInfo
            subtitle2String = dosInfo
            subtitle3String = filesInfo

        case is PRGFolderProxy:
            
            titleString = "Disk from a file system folder"
            subtitle1String = "Comprises all PRG files found in this directory"
            subtitle2String = ""
            subtitle3String = ""
            
        default:
            fatalError()
        }
        
        // Load screenshots (if any)
        let fnv = myDocument.attachment!.fnv()
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
        
        if crt != nil {
            icon.image = NSImage.init(named: "cartridge")
        } else if tap != nil {
            icon.image = NSImage.init(named: "tape")
        } else if volume != nil {
            icon.image = NSImage.init(named: writeProtect ? "disk2_protected" : "disk2")
        } else {
            icon.image = NSImage.init(named: "NSFolder")
        }
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
        
        let id = sender.tag == 0 ? DriveID.DRIVE8 : DriveID.DRIVE9
        
        track("insertAction: \(sender.tag)")

        if d64 != nil {
            track("Inserting D64")
            c64.drive(id)?.insertD64(d64)
            c64.drive(id).setWriteProtection(writeProtect)
        }
        /*
        switch media {
        case .collection, .d64, .g64, .directory:
            
            let disk = myDocument.attachment
            let id = sender.tag == 0 ? DriveID.DRIVE8 : DriveID.DRIVE9
            c64.drive(id)?.insertDisk(disk as? AnyArchiveProxy)
            c64.drive(id).setWriteProtection(writeProtect)
            
        case .cartridge:

            let cartridge = myDocument.attachment as! CRTFileProxy
            c64.expansionport.attachCartridgeAndReset(cartridge)
                        
        case .tape:
            
            let tape = myDocument.attachment as! TAPFileProxy
            c64.datasette.insertTape(tape)
            
            if autoRun {
                /*
                parent.keyboard.type(string: "LOAD\n",
                                     completion: c64.datasette.pressPlay)
                */
                parent.keyboard.type("LOAD\n")
                c64.datasette.pressPlay()
            }
            
        case .none:
            fatalError()
        }
        */
        
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {
        
        track("flashAction: \(sender.selectedTag())")
        
        let archive = myDocument.attachment as! AnyArchiveProxy
        parent.c64.flash(archive, item: sender.selectedTag())

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
