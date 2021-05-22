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
    @IBOutlet weak var progress: NSProgressIndicator!
    @IBOutlet weak var button: NSButton!

    var panel: NSSavePanel!

    override func showSheet(completionHandler handler: (() -> Void)? = nil) {
            
        track()
                
        // Create save panel
        panel = NSSavePanel()
        panel.prompt = "Export"
        panel.title = "Export"
        panel.allowedFileTypes = ["mp4"]
        
        /*
        // Run panel as sheet
        if let win = parent.window {
            panel.beginSheetModal(for: win, completionHandler: { result in
                if result == .OK {
                    if let url = self.panel.url {
                        track("url = \(url)")
                        self.export(to: url)
                    }
                }
            })
        }
        */
        export(to: URL.init(string: "/tmp/movie.mp4")!)
    }
    
    func export(to url: URL) {

        track("url = \(url)")

        super.showSheet()

        text.stringValue = "Exporting video to"
        subtext.stringValue = url.path
        button.isHidden = true
        progress.startAnimation(self)
                
        c64.recorder.export(as: url.path)

        text.stringValue = "Video exported to"
        progress.stopAnimation(self)
        progress.isHidden = true
        button.isHidden = false
    }
}
