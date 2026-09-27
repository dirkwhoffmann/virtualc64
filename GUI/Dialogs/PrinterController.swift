// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

/* Displays the output of the emulated MPS-803 printer as it prints, and lets
 * the user print or export it as a PDF.
 *
 * This window is built entirely in code rather than loaded from a nib. Its
 * base class is plain NSWindowController (not DialogController) because
 * DialogController's showAsWindow()/showAsSheet() assume a nib-backed
 * window (they call loadWindow(), which requires a nib name); a window that
 * is constructed programmatically has no nib to load.
 */
@MainActor
final class PrinterController: NSWindowController {

    weak var parent: MyController?

    var emu: EmulatorProxy? { parent?.emu }
    var printer: PrinterProxy? { emu?.printer }

    var paperView: PaperView!
    var scrollView: NSScrollView!

    // Coalescing: PRT_ROWS is posted at bus speed (far faster than 60Hz).
    // A single pending Timer, armed by the first refresh() in a batch and
    // ignored by every refresh() that arrives before it fires, guarantees
    // the paper view is redrawn at most once per frame regardless of how
    // many messages arrive in between.
    private var redrawTimer: Timer?

    convenience init(parent: MyController) {

        self.init(window: nil)
        self.parent = parent
        buildWindow()
    }

    private func buildWindow() {

        let contentSize = NSSize(width: 680, height: 900)
        let window = NSWindow(contentRect: NSRect(origin: .zero, size: contentSize),
                               styleMask: [.titled, .closable, .miniaturizable, .resizable],
                               backing: .buffered,
                               defer: false)
        window.title = "Printer"
        window.minSize = NSSize(width: 360, height: 400)
        shouldCascadeWindows = false
        window.center()
        window.isReleasedWhenClosed = false

        let paper = PaperView()
        paper.controller = self
        paper.frame = NSRect(origin: .zero, size: paper.intrinsicContentSize)

        let scroll = NSScrollView(frame: NSRect(origin: .zero, size: contentSize))
        scroll.hasVerticalScroller = true
        scroll.hasHorizontalScroller = true
        scroll.autohidesScrollers = true
        scroll.borderType = .noBorder
        scroll.documentView = paper
        scroll.autoresizingMask = [.width, .height]

        let printButton = NSButton(title: "Print…", target: self, action: #selector(printAction(_:)))
        printButton.bezelStyle = .rounded
        printButton.keyEquivalent = "p"
        printButton.keyEquivalentModifierMask = [.command]

        let exportButton = NSButton(title: "Export as PDF…", target: self, action: #selector(exportAction(_:)))
        exportButton.bezelStyle = .rounded

        let clearButton = NSButton(title: "Clear", target: self, action: #selector(clearAction(_:)))
        clearButton.bezelStyle = .rounded

        let bar = NSStackView(views: [clearButton, NSView(), exportButton, printButton])
        bar.orientation = .horizontal
        bar.distribution = .fill
        bar.spacing = 8
        bar.edgeInsets = NSEdgeInsets(top: 8, left: 12, bottom: 8, right: 12)
        bar.translatesAutoresizingMaskIntoConstraints = false

        let content = NSView(frame: NSRect(origin: .zero, size: contentSize))
        content.addSubview(scroll)
        content.addSubview(bar)

        scroll.translatesAutoresizingMaskIntoConstraints = false

        NSLayoutConstraint.activate([

            bar.leadingAnchor.constraint(equalTo: content.leadingAnchor),
            bar.trailingAnchor.constraint(equalTo: content.trailingAnchor),
            bar.bottomAnchor.constraint(equalTo: content.bottomAnchor),

            scroll.leadingAnchor.constraint(equalTo: content.leadingAnchor),
            scroll.trailingAnchor.constraint(equalTo: content.trailingAnchor),
            scroll.topAnchor.constraint(equalTo: content.topAnchor),
            scroll.bottomAnchor.constraint(equalTo: bar.topAnchor)
        ])

        window.contentView = content
        window.delegate = self

        self.window = window
        self.paperView = paper
        self.scrollView = scroll
    }

    // Invalidates the currently-open page and schedules a coalesced redraw.
    // Called from MyController's message handler on PRT_ROWS, PRT_PAGE and
    // PRT_CONNECT.
    func refresh() {

        guard let paperView else { return }

        let touched = paperView.pageCount
        if touched != paperView.lastKnownPageCount {
            paperView.lastKnownPageCount = touched
            paperView.frame = NSRect(origin: .zero, size: paperView.intrinsicContentSize)
        }
        if touched > 0 {
            paperView.invalidateCache(page: touched - 1)
        }

        scheduleRedraw()
    }

    private func scheduleRedraw() {

        guard redrawTimer == nil else { return }

        redrawTimer = Timer.scheduledTimer(withTimeInterval: 1.0 / 60.0, repeats: false) { [weak self] _ in

            self?.redrawTimer = nil
            self?.paperView.needsDisplay = true
        }
    }

    //
    // Printing and PDF export
    //

    @IBAction func printAction(_ sender: Any!) {

        let info = NSPrintInfo.shared
        info.topMargin = 0
        info.bottomMargin = 0
        info.leftMargin = 0
        info.rightMargin = 0

        NSPrintOperation(view: paperView, printInfo: info).run()
    }

    @IBAction func exportAction(_ sender: Any!) {

        let panel = NSSavePanel()
        panel.allowedContentTypes = [.pdf]
        panel.nameFieldStringValue = "Printout.pdf"

        panel.beginSheetModal(for: window!) { response in

            guard response == .OK, let url = panel.url else { return }
            self.writePDF(to: url)
        }
    }

    @IBAction func clearAction(_ sender: Any!) {

        printer?.clear()
        paperView.invalidateAllCache()
        paperView.lastKnownPageCount = 0
        paperView.frame = NSRect(origin: .zero, size: paperView.intrinsicContentSize)
        paperView.needsDisplay = true
    }

    // Writes every printed page as a true-size PDF page, through the same
    // PaperView.drawImage(forPage:in:rect:) path used for on-screen and
    // NSPrintOperation-based printing, so the exported file always matches
    // what is on screen.
    //
    // Page sizing (from the MPS-803's real geometry):
    //   Horizontal: 480 dots @ 60 dpi  = 8.00in = 576pt
    //   Vertical:   792 dots @ 72 dpi  = 11.00in = 792pt
    // That maps 1:1 onto US Letter's printable area, so each page is drawn
    // at its true physical size with no resampling.
    private func writePDF(to url: URL) {

        guard let paperView, paperView.pageCount > 0 else { NSSound.beep(); return }
        guard let consumer = CGDataConsumer(url: url as CFURL) else { return }

        var mediaBox = CGRect(x: 0, y: 0, width: PaperView.pageWidthPts, height: PaperView.pageHeightPts)
        guard let ctx = CGContext(consumer: consumer, mediaBox: &mediaBox, nil) else { return }

        for page in 0 ..< paperView.pageCount {

            var box = mediaBox
            ctx.beginPDFPage(nil)
            ctx.interpolationQuality = .none
            paperView.drawImage(forPage: page, in: ctx, rect: CGRect(origin: .zero, size: box.size))
            ctx.endPDFPage()
        }

        ctx.closePDF()
    }
}

extension PrinterController: NSWindowDelegate {

    func windowWillClose(_ notification: Notification) {

        redrawTimer?.invalidate()
        redrawTimer = nil
    }
}

/* Draws the printed pages stacked vertically inside the enclosing scroll
 * view, on a paper-coloured background. Each page is fetched as a CGImage
 * via PrinterProxy.createImageForPage: (CF_RETURNS_RETAINED -- see the
 * caching note below) and cached; only the page currently receiving new
 * dots is ever invalidated, so redrawing after a PRT_ROWS message costs at
 * most one fresh page image, never a full rebuild of the document.
 */
@MainActor
final class PaperView: NSView {

    weak var controller: PrinterController?

    // Physical page size in points -- see writePDF's comment for the dpi
    // arithmetic. Kept fixed regardless of window size or zoom: dots must
    // stay hard squares (well, 1.2:1 rectangles, matching the printer's
    // non-square dot pitch), never resampled to fit.
    static let pageWidthPts: CGFloat = 576
    static let pageHeightPts: CGFloat = 792
    static let pageGap: CGFloat = 24
    static let margin: CGFloat = 24

    // Height of one page on the paper roll, in dots. Mirrors
    // VCCore/Peripherals/Printer/Paper.h's `height` constant. Every page
    // PrinterProxy hands back is always exactly this tall (untouched rows
    // come back blank), so page geometry never needs to be discovered from
    // the image itself.
    private static let dotsPerPage = 792

    // Per-page image cache. A CGImage fetched via createImageForPage: is a
    // Core Foundation object; because the Objective-C method is annotated
    // CF_RETURNS_RETAINED, the Swift importer treats the call as handing
    // back an already-owned (+1) reference and imports the return type as a
    // plain `CGImage?` (not `Unmanaged<CGImage>`). ARC then manages it like
    // any other Swift-owned object -- no manual CGImageRelease, no
    // takeRetainedValue()/takeUnretainedValue() dance needed. Storing it in
    // this dictionary keeps it alive across redraws (avoiding the ~380KB
    // per-page rebuild on every frame); when a page is evicted from the
    // dictionary (or the view itself deallocates) ARC releases it normally.
    private var cache: [Int: CGImage] = [:]

    // Last page count this view laid out for. Used by the controller to
    // detect when the document has grown (a new page started) so it knows
    // when to invalidate the intrinsic content size.
    var lastKnownPageCount = 0

    override var isFlipped: Bool { false }

    // Number of pages touched so far (complete or in progress), derived
    // from the absolute row count the same way Paper::pages() would if it
    // additionally counted a partial trailing page.
    var pageCount: Int {

        guard let rows = controller?.printer?.rows, rows > 0 else { return 0 }
        return (rows + Self.dotsPerPage - 1) / Self.dotsPerPage
    }

    override var intrinsicContentSize: NSSize {

        let n = max(pageCount, 1)
        let height = CGFloat(n) * Self.pageHeightPts + CGFloat(n - 1) * Self.pageGap + 2 * Self.margin
        let width = Self.pageWidthPts + 2 * Self.margin
        return NSSize(width: width, height: height)
    }

    func invalidateCache(page: Int) {

        cache[page] = nil
    }

    func invalidateAllCache() {

        cache.removeAll()
    }

    private func image(forPage page: Int) -> CGImage? {

        if let img = cache[page] { return img }
        guard let cg = controller?.printer?.createImage(forPage: page) else { return nil }
        cache[page] = cg
        return cg
    }

    // Page rect in the view's own (bottom-up, non-flipped) coordinate
    // system, with page 0 at the top of the document.
    private func pageRect(_ page: Int) -> CGRect {

        let totalHeight = intrinsicContentSize.height
        let top = totalHeight - Self.margin - CGFloat(page) * (Self.pageHeightPts + Self.pageGap)
        return CGRect(x: Self.margin, y: top - Self.pageHeightPts, width: Self.pageWidthPts, height: Self.pageHeightPts)
    }

    // Shared drawing path used by both on-screen drawing and PDF export, so
    // the exported file always matches what is on screen.
    func drawImage(forPage page: Int, in ctx: CGContext, rect: CGRect) {

        ctx.interpolationQuality = .none
        ctx.setFillColor(NSColor.white.cgColor)
        ctx.fill(rect)

        if let img = image(forPage: page) {
            ctx.draw(img, in: rect)
        }
    }

    override func draw(_ dirtyRect: NSRect) {

        NSColor(white: 0.55, alpha: 1).setFill()
        dirtyRect.fill()

        guard let ctx = NSGraphicsContext.current?.cgContext else { return }

        let n = max(pageCount, 1)
        for page in 0 ..< n {

            let rect = pageRect(page)
            guard rect.intersects(dirtyRect) else { continue }
            drawImage(forPage: page, in: ctx, rect: rect)
        }
    }

    //
    // Printing
    //

    override func knowsPageRange(_ range: NSRangePointer) -> Bool {

        range.pointee = NSRange(location: 1, length: max(pageCount, 1))
        return true
    }

    override func rectForPage(_ page: Int) -> NSRect {

        // rectForPage is 1-based
        return pageRect(page - 1)
    }
}
