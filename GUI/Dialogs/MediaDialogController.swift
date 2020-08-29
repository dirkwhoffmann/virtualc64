// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MediaDialogController: DialogController {
        
    enum MediaType {
        case archive
        case disk
        case tape
        case cartridge
    }
    
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var checkbox: NSButton!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var subtitle: NSTextField!
    @IBOutlet weak var subsubtitle: NSTextField!
    @IBOutlet weak var drive8: NSButton!
    @IBOutlet weak var drive9: NSButton!
    @IBOutlet weak var flash: NSPopUpButton!
    @IBOutlet weak var carousel: iCarousel!

    var type: C64FileType!
    var media: MediaType!
    var archive: AnyArchiveProxy?
    var tape: TAPFileProxy?
    var crt: CRTFileProxy?
    
    var writeProtect = false
    // var screenshots: [Screenshot] = []
    
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
    var subTitleString = "???"
    var subsubTitleString = ""
    
    // Shortcuts
    var myDocument: MyDocument { return parent.mydocument! }
    var numItems: Int { return carousel.numberOfItems }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return numItems / 2 }
    var lastItem: Int { return numItems - 1 }
    var empty: Bool { return numItems == 0 }
    
    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
    
        type = myDocument.attachment!.type()

        switch myDocument.attachment {
             
        case _ as CRTFileProxy:
            
            media = .cartridge
            let crt = myDocument.attachment as! CRTFileProxy
            let type = crt.cartridgeType().rawValue
            let typeName = crt.cartridgeTypeName()!
            
            track("CRTFileProxy")
            titleString = "CRT File"
            subTitleString = "A cartridge for the C64 expansion port"
            subsubTitleString = "CRT type \(type): \(typeName)"
            
        case _ as TAPFileProxy:
            
            media = .tape
            let tap = myDocument.attachment as! TAPFileProxy
            let version = tap.tapVersion()
            let versionName = version == 0 ? "Original" : "Extended"
            track("TAPFileProxy")
            titleString = "TAP File"
            subTitleString = "A representation of a C64 cassette"
            subsubTitleString = "TAP type \(version): \(versionName) layout"
            
        case _ as T64FileProxy:
            
            media = .archive
            track("T64FileProxy")
            titleString = "T64 File"
            subTitleString = "A collection of multiple C64 programs"

        case _ as PRGFileProxy:
            
            media = .archive
            track("PRGFileProxy")
            titleString = "PRG File"
            subTitleString = "Binary representation of a single C64 program"
            
        case _ as P00FileProxy:
            
            media = .archive
            track("P00FileProxy")
            titleString = "P00 File"
            subTitleString = "Binary representation of a single C64 program"
            
        case _ as D64FileProxy:
            
            media = .archive
            track("D64FileProxy")
            titleString = "D64 File"
            subTitleString = "A byte-accurate image of a C64 diskette"
            
        case _ as G64FileProxy:
            
            media = .disk
            track("G64FileProxy")
            titleString = "G64 File"
            subTitleString = "A bit-accurate image of a C64 diskette"
            
        default:
            fatalError()
        }
                  
        /*
        if disk != nil {
            
            // Load screenshots (if any)
            for url in Screenshot.collectFiles(forDisk: disk!.fnv) {
                if let screenshot = Screenshot.init(fromUrl: url) {
                    screenshots.append(screenshot)
                }
            }
            super.showSheet(completionHandler: handler)
        }
        */
        super.showSheet(completionHandler: handler)
    }
    
    override public func awakeFromNib() {
        
        super.awakeFromNib()
        window?.makeFirstResponder(carousel)
        refresh()
        updateCarousel(goto: centerItem, animated: false)
    }
    
    override func windowDidLoad() {
        
        track("windowDidLoad")
        
        let connected8 = parent.config.drive8Connected
        let connected9 = parent.config.drive8Connected

        title.stringValue = titleString
        subtitle.stringValue = subTitleString
        subsubtitle.stringValue = subsubTitleString
        
        let numberOfItems = setUpFlashItems()
        flash.isHidden = numberOfItems == 0
        
        // Configure controls
        switch media {
                        
        case .archive, .disk:
            checkbox.title = "Write protect"
            checkbox.isHidden = false
            drive8.isEnabled = connected8
            drive9.isEnabled = connected9

        case .tape:
            checkbox.title = "Auto load"
            checkbox.isHidden = false
            drive8.title = "Insert"
            drive9.isHidden = true

        case .cartridge:
            checkbox.isHidden = true
            drive8.title = "Attach"
            drive9.isHidden = true

        default:
            fatalError()
        }

        if empty {

            setHeight(196)
            
        } else {
            
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                self.carousel.type = iCarouselType.coverFlow
                self.carousel.isHidden = false
                self.carousel.scrollToItem(at: self.centerItem, animated: false)
            }
        }
        
        track("(3)")
    }
    
    func setUpFlashItems() -> Int {
        
        if media != .archive { return 0 }
        let d64 = myDocument.attachment as! AnyArchiveProxy
        
        flash.removeAllItems()
        flash.font = monofont
        
        let items = d64.numberOfItems()
        var seen: [String] = []
        var item = 0
        for i in  0 ..< items {
            
            d64.selectItem(i)
            let name = d64.nameOfItem()!
            // let name = d64.unicodeNameOfItem()!
            let size = d64.sizeOfItemInBlocks()
            let type = d64.typeOfItem()!
            
            track("\(name) \(size) \(type)")
            
            var title = "\(size)"
            title = title.padding(toLength: 5, withPad: " ", startingAt: 0)
            title += "\"\(name)\""
            title = title.padding(toLength: 24, withPad: " ", startingAt: 0)
            title += "\(type)"
            title = title.padding(toLength: 28, withPad: " ", startingAt: 0)

            if !seen.contains(title) {
                seen.append(title)
                flash.addItem(withTitle: title)
                flash.item(at: item)!.tag = i
                flash.item(at: item)!.isEnabled = type == "PRG"
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
        
        // Update disk icon
        switch media {
        case .archive, .disk:
            icon.image = NSImage.init(named: writeProtect ? "disk_protected" : "disk")
        case .cartridge:
            icon.image = NSImage.init(named: "cartridge")
        case .tape:
            icon.image = NSImage.init(named: "tape")
        case .none:
            fatalError()
        }
        /*
        let typeName = [
            DiskType.DISK_35_DD: "3.5\"DD Amiga",
            DiskType.DISK_35_DD_PC: "3.5\"DD PC",
            DiskType.DISK_35_HD: "3.5\"HD Amiga",
            DiskType.DISK_35_HD_PC: "3.5\"HD PC",
            DiskType.DISK_525_SD: "5.25\"SD PC"
        ]
        let str = typeName[disk!.diskType]!
        subtitle.stringValue = "A byte-accurate image of a \(str) diskette."
        let compatible = disk!.diskType == .DISK_35_DD
        warning.isHidden = compatible
        */
        // Check for available drives
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
    
    @IBAction func writeProtectAction(_ sender: NSButton!) {
        
        writeProtect = sender.state == .on
        refresh()
    }

    @IBAction func insertAction(_ sender: NSButton!) {
        
        track("insertAction: \(sender.tag)")

        /*
        amiga.diskController.insert(sender.tag, adf: disk)
        amiga.diskController.setWriteProtection(sender.tag, value: writeProtect)
        */
     
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {
        
        let tag = sender.selectedTag()
        track("flashAction: \(tag)")
    }

    @IBAction override func cancelAction(_ sender: Any!) {
                                  
         hideSheet()
         // try? myDocument.persistScreenshots()
     }
}

extension MediaDialogController: NSWindowDelegate {
    
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

extension MediaDialogController: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
        
        return 0; // screenshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        // itemView.image = screenshots[index].screen?.roundCorners()
        itemView.image = nil
        return itemView
    }
}
