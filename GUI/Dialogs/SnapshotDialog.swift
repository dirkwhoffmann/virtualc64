// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class SnapshotDialog: DialogController {
    
    var now: Date!
    
    @IBOutlet weak var carousel: iCarousel!
    @IBOutlet weak var moveUp: NSButton!
    @IBOutlet weak var moveDown: NSButton!
    @IBOutlet weak var restore: NSButton!
    @IBOutlet weak var nr: NSTextField!
    @IBOutlet weak var text1: NSTextField!
    @IBOutlet weak var text2: NSTextField!
    
    // Computed variables
    var myDocument: MyDocument { return parent.mydocument! }
    var numItems: Int { return carousel.numberOfItems }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return numItems / 2 }
    var lastItem: Int { return numItems - 1 }
    var empty: Bool { return numItems == 0 }
    
    override func windowWillLoad() {
   
        track()
    }
    
    override func sheetDidShow() {
  
        track()
        
        now = Date()
        
        parent.stopSnapshotTimer()
        updateLabels()
        
        self.carousel.type = iCarouselType.timeMachine
        self.carousel.isHidden = false
        self.updateCarousel(goto: self.lastItem, animated: false)
    }
    
    func updateLabels() {
        
        moveUp.isEnabled = currentItem >= 0 && currentItem < lastItem
        moveDown.isEnabled = currentItem > 0
        nr.stringValue = "\(currentItem + 1) / \(numItems)"
    
        moveUp.isHidden = empty
        moveDown.isHidden = empty
        nr.isHidden = empty
        restore.isHidden = empty
        
        if let snapshot = myDocument.snapshots.element(at: currentItem) {
            let takenAt = snapshot.timeStamp
            text1.stringValue = "Taken at " + timeInfo(time: takenAt)
            text2.stringValue = timeDiffInfo(time: takenAt)
        } else {
            text1.stringValue = "No snapshots available"
            text2.stringValue = ""
        }
        text1.isHidden = false
        text2.isHidden = false
    }
      
    func updateCarousel(goto item: Int, animated: Bool) {
        
        carousel.reloadData()
        
        let index = min(item, carousel.numberOfItems - 1)
        if index >= 0 { carousel.scrollToItem(at: index, animated: animated) }
        
        carousel.layOutItemViews()
        updateLabels()
    }

    func updateCarousel(animated: Bool = false) {
        
        updateCarousel(goto: -1, animated: animated)
    }
    
    func timeInfo(date: Date?) -> String {
         
         if date == nil { return "" }
         
         let formatter = DateFormatter()
         formatter.timeZone = TimeZone.current
         formatter.dateFormat = "HH:mm:ss" // "yyyy-MM-dd HH:mm"
         
         return formatter.string(from: date!)
    }
    
    func timeInfo(time: time_t) -> String {
        
        return timeInfo(date: Date(timeIntervalSince1970: TimeInterval(time)))
    }
    
    func timeDiffInfo(seconds: Int) -> String {
        
        let secPerMin = 60
        let secPerHour = secPerMin * 60
        let secPerDay = secPerHour * 24
        let secPerWeek = secPerDay * 7
        let secPerMonth = secPerWeek * 4
        let secPerYear = secPerWeek * 52
        
        if seconds == 0 {
            return "Now"
        }
        if seconds < secPerMin {
            return "\(seconds) second" + (seconds == 1 ? "" : "s") + " ago"
        }
        if seconds < secPerHour {
            let m = seconds / secPerMin
            return "\(m) minute" + (m == 1 ? "" : "s") + " ago"
        }
        if seconds < secPerDay {
            let h = seconds / secPerHour
            return "\(h) hour" + (h == 1 ? "" : "s") + " ago"
        }
        if seconds < secPerWeek {
            let d = seconds / secPerDay
            return "\(d) day" + (d == 1 ? "" : "s") + " ago"
        }
        if seconds < secPerMonth {
            let w = seconds / secPerWeek
            return "\(w) week" + (w == 1 ? "" : "s") + " ago"
        }
        if seconds < secPerYear {
            let m = seconds / secPerMonth
            return "\(m) month" + (m == 1 ? "" : "s") + " ago"
        } else {
            let y = seconds / secPerYear
            return "\(y) year" + (y == 1 ? "" : "s") + " ago"
        }
    }
    
    func timeDiffInfo(interval: TimeInterval?) -> String {
        
        return interval == nil ? "" : timeDiffInfo(seconds: Int(interval!))
    }
    
    func timeDiffInfo(date: Date?) -> String {
        
        return date == nil ? "" : timeDiffInfo(interval: date!.diff(now))
    }
    
    func timeDiffInfo(time: time_t) -> String {
        
        let date = Date(timeIntervalSince1970: TimeInterval(time))
        return timeDiffInfo(date: date)
    }

    func timeDiffInfo(url: URL) -> String {
        
        return timeDiffInfo(date: url.modificationDate)
    }

    @IBAction func selectorAction(_ sender: NSSegmentedControl!) {
                
        updateCarousel(goto: Int.max, animated: false)
    }
    
    @IBAction func moveUpAction(_ sender: NSButton!) {
                
        if currentItem < lastItem {
            carousel.scrollToItem(at: currentItem + 1, animated: true)
        }
    }

    @IBAction func moveDownAction(_ sender: NSButton!) {
                
        if currentItem > 0 {
            carousel.scrollToItem(at: currentItem - 1, animated: true)
        }
    }
        
    @IBAction func revertAction(_ sender: NSButton!) {
        
        track()
        
        do {
            try parent.restoreSnapshot(item: currentItem)
        } catch {
            NSSound.beep()
        }
    }
        
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
                        
        hideSheet()

        parent.startSnapshotTimer()
        
        // Hide some controls
        let items: [NSView] = [
            
            nr,
            moveUp,
            moveDown,
            restore,
            text1,
            text2,
            carousel
        ]
        
        for item in items { item.isHidden = true }
    }
}

//
// iCarousel data source and delegate
//

extension SnapshotDialog: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
                
        return myDocument.snapshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        itemView.image = myDocument.snapshots.element(at: index)?.previewImage?.roundCorners()
        
        /*
        itemView.wantsLayer = true
        itemView.layer?.cornerRadius = 10.0
        itemView.layer?.masksToBounds = true
        */
        
        return itemView
    }
    
    func carouselCurrentItemIndexDidChange(_ carousel: iCarousel) {
        
        updateLabels()
    }
}
