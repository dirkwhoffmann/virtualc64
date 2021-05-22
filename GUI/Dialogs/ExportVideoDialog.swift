// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ExportVideoDialog: DialogController {

    @IBOutlet weak var text: NSTextField!
    @IBOutlet weak var subtext: NSTextField!
    @IBOutlet weak var duration: NSTextField!
    @IBOutlet weak var size: NSTextField!
    @IBOutlet weak var progress: NSProgressIndicator!
    @IBOutlet weak var cancelButton: NSButton!
    @IBOutlet weak var exportButton: NSButton!
    @IBOutlet weak var icon: NSImageView!

    var panel: NSSavePanel!

    let path = "/tmp/virtualc64.mp4"

    override func showSheet(completionHandler handler: (() -> Void)? = nil) {
            
        track()
        super.showSheet()

        duration.stringValue = ""
        size.stringValue = ""
        progress.startAnimation(self)
            
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.25) {
            self.text.isHidden = false
            self.subtext.isHidden = false
        }

        if c64.recorder.export(as: path) {
            
            text.stringValue = "The video has been ecoded successfully"
            subtext.stringValue = "Select Save or copy via drag & drop"
            icon.isHidden = false
            exportButton.isHidden = false
            size.stringValue = URL.init(fileURLWithPath: path).fileSizeString
            duration.stringValue = String(format: "%.1f sec", c64.recorder.duration)
            
        } else {
            
            subtext.stringValue = "The video could not be ecoded"
            subtext.stringValue = "No output generated"
        }
        
        cancelButton.isHidden = false
        progress.stopAnimation(self)
        progress.isHidden = true
    }
    
    @IBAction func exportAction(_ sender: NSButton!) {

        track()
        
        // Create save panel
        panel = NSSavePanel()
        panel.allowedFileTypes = ["mp4"]
        
        // Run panel as sheet
        if let win = window {
            track()
            panel.beginSheetModal(for: win, completionHandler: { result in
                if result == .OK {
                    track()
                    if let url = self.panel.url {
                        track("url = \(url)")
                        let source = URL.init(fileURLWithPath: self.path)
                        FileManager.copy(from: source, to: url)
                    }
                }
            })
        }
    }
}
