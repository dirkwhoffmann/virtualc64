
// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class OnboardingLayerView: NSView {

    override func mouseDown(with event: NSEvent) {

    }

    override func mouseUp(with event: NSEvent) {

    }
}

class OnboardingPageContainerView: NSView {

}

class OnboardingLayerViewController: NSViewController {

    @IBOutlet weak var pageContainerView: OnboardingPageContainerView!
    @IBOutlet weak var pageDotIndicator: PageDotsIndicator!
    @IBOutlet weak var nextButton: NSButton!
    @IBOutlet weak var prevButton: NSButton!
    @IBOutlet weak var skipButton: NSButton!

    var layer: Onboarding!
    var controller: MyController { layer.controller }
    var config: Configuration { controller.config }
    var mm: MediaManager {  controller.mm }
    var emu: EmulatorProxy! { layer.emu }

    // Onboarding settings
    var model = 1
    var pal = true
    var rom = 0

    // Array holding the individual view controllers for each panel
    private var pages: [NSViewController] = []
    private var currentPageIndex: Int = 0 {
        didSet { pageDotIndicator.currentPage = currentPageIndex }
    }

    override func viewDidLoad() {

        super.viewDidLoad()
        pageDotIndicator.delegate = self

        view.wantsLayer = true
        view.layer!.backgroundColor = NSColor.red.cgColor

        /*
         let r = Double(34.0 / 255.0)
         let g = Double(84.0 / 255.0)
         let b = Double(164.0 / 255.0)
         */
    }

    override func viewDidAppear() {

        func instantiate(_ id: String) -> OnboardingViewController {

            let storyboard = NSStoryboard(name: "Onboarding", bundle: nil)
            let result = storyboard.instantiateController(withIdentifier: id) as! OnboardingViewController
            result.layer = layer
            return result
        }

        // Start over by removing all child view controllers and their views
        for child in children {
            child.view.removeFromSuperview()
            child.removeFromParent()
        }

        super.viewDidAppear()
        pages = [instantiate("Step1"), instantiate("Step2")]
        pageDotIndicator.numberOfPages = pages.count

        showPage(at: 0)
    }

    private func showPage(at index: Int, animated: Bool = true) {

        guard pages.indices.contains(index) else { return }

        let newPage = pages[index]
        let oldPage = children.isEmpty ? nil : children[0]

        addChild(newPage)
        newPage.view.frame = pageContainerView.bounds
        newPage.view.autoresizingMask = [.width, .height]

        if let oldPage = oldPage {

            // Transition from the old to the new page
            if animated {

                let options: NSViewController.TransitionOptions =
                [index > currentPageIndex ? .slideLeft : .slideRight, .allowUserInteraction]
                NSAnimationContext.runAnimationGroup({ context in

                    context.duration = 0.8
                    context.timingFunction = CAMediaTimingFunction(name: .easeInEaseOut)
                    self.transition(from: oldPage,
                                    to: newPage,
                                    options: options,
                                    completionHandler: nil)

                }, completionHandler: {

                    Task { @MainActor in oldPage.removeFromParent() }
                })

            } else {

                transition(from: oldPage, to: newPage, options: [], completionHandler: nil)
                oldPage.removeFromParent()
            }

        } else {

            // First page, just add it
            pageContainerView.addSubview(newPage.view)
        }

        currentPageIndex = index
        prevButton.isEnabled = currentPageIndex > 0
        nextButton.title = currentPageIndex == pages.count - 1 ? "Launch" : "Continue"
    }

    @IBAction func nextPage(_ sender: Any?) {

        let nextIndex = currentPageIndex + 1

        if pages.indices.contains(nextIndex) {
            showPage(at: nextIndex)
        } else {
            apply()
            finish()
        }
    }

    func apply() {

        var scheme: ConfigScheme

        // C64 model
        switch model {
        case 0: scheme = pal ? .PAL_OLD : .NTSC_OLD
        case 1: scheme = pal ? .PAL : .NTSC
        case 2: scheme = pal ? .PAL_II : .NTSC_II
        default: fatalError()
        }
        emu.set(scheme)

        // Roms
        emu.installOpenRoms()
    }

    func finish() {

        layer.close(delay: 1.0)
        try? emu.run()
    }

    @IBAction func previousPage(_ sender: Any?) {

        let prevIndex = currentPageIndex - 1
        if pages.indices.contains(prevIndex) {
            showPage(at: prevIndex)
        }
    }

    @IBAction func skipAction(_ sender: Any?) {

        finish()
    }
}

extension OnboardingLayerViewController: PageDotsIndicatorDelegate {

    func pageDotsIndicator(_ pageDotsIndicator: PageDotsIndicator, didSelectPage index: Int) {

        showPage(at: index)
    }
}

class OnboardingViewController: NSViewController {

    var layer: Onboarding!
    var config: Configuration { layer.controller.config }

    func refresh() { }

    override func viewDidLoad() {

        refresh()
    }
}

class OnboardingViewController1: OnboardingViewController {

    @IBOutlet weak var model0Button: OnboardingButton!
    @IBOutlet weak var model1Button: OnboardingButton!
    @IBOutlet weak var model2Button: OnboardingButton!
    @IBOutlet weak var palButton: OnboardingButton!
    @IBOutlet weak var ntscButton: OnboardingButton!
    @IBOutlet weak var palIcon: NSImageView!
    @IBOutlet weak var ntscIcon: NSImageView!

    var model: Int {
        get { layer.onboardingVC.model }
        set { layer.onboardingVC.model = newValue; refresh() }
    }
    var pal: Bool {
        get { layer.onboardingVC.pal }
        set { layer.onboardingVC.pal = newValue; refresh() }
    }

    @IBAction func modelAction(_ sender: NSControl) {

        model = sender.tag

    }

    @IBAction func palAction(_ sender: NSControl) {

        pal = sender.tag == 0
    }
    
    override func refresh() {

        model0Button.state = model == 0 ? .on : .off
        model1Button.state = model == 1 ? .on : .off
        model2Button.state = model == 2 ? .on : .off
        palButton.state = pal ? .on : .off
        ntscButton.state = !pal ? .on : .off
    }
}

class OnboardingViewController2: OnboardingViewController {

    var rom: Int {
        get { layer.onboardingVC.rom }
        set { layer.onboardingVC.rom = newValue; refresh() }
    }
    var openRoms: Bool { rom == 0 }

    @IBOutlet weak var openRomButton: OnboardingButton!

    @IBAction func romAction(_ sender: NSControl) {

        rom = sender.tag
    }

    override func refresh() {

        openRomButton.state = openRoms ? .on : .off
    }
}

class Onboarding: Layer {

    var window: NSWindow { controller.window! }
    var contentView: NSView { window.contentView! }
    var backgroundView: NSView = NSView()
    var storyboard: NSStoryboard { NSStoryboard(name: "Onboarding", bundle: nil) }

    var onboardingVC: OnboardingLayerViewController!

    override init(renderer: Renderer) {

        super.init(renderer: renderer)

        backgroundView.wantsLayer = true
        backgroundView.layer?.backgroundColor = NSColor.windowBackgroundColor.cgColor
        backgroundView.translatesAutoresizingMaskIntoConstraints = false

        onboardingVC = storyboard.instantiateController(withIdentifier: "OnboardingLayerViewController") as? OnboardingLayerViewController
        onboardingVC.view.wantsLayer = true
        onboardingVC.view.layer?.backgroundColor = NSColor.windowBackgroundColor.cgColor
        onboardingVC.layer = self
    }

    override func open(delay: Double) {

        super.open(delay: delay)

        if let toolbar = window.toolbar as? MyToolbar {
            toolbar.isVisible = true
            toolbar.globalDisable = true
            toolbar.validateVisibleItems()
        }
    }

    override func layerDidOpen() {

    }

    override func layerDidClose() {

        if let toolbar = window.toolbar as? MyToolbar {
            toolbar.globalDisable = false
            toolbar.validateVisibleItems()
        }
    }

    override func alphaDidChange() {

        onboardingVC.view.alphaValue = CGFloat(alpha.current)

        if alpha.current > 0 && onboardingVC.view.superview == nil {

            contentView.addSubview(backgroundView)
            NSLayoutConstraint.activate([
                backgroundView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
                backgroundView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
                backgroundView.topAnchor.constraint(equalTo: contentView.topAnchor),
                backgroundView.bottomAnchor.constraint(equalTo: contentView.bottomAnchor)
            ])

            contentView.addSubview(onboardingVC.view)
            onboardingVC.view.translatesAutoresizingMaskIntoConstraints = false
            NSLayoutConstraint.activate([
                onboardingVC.view.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
                onboardingVC.view.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
                onboardingVC.view.topAnchor.constraint(equalTo: contentView.topAnchor),
                onboardingVC.view.bottomAnchor.constraint(equalTo: contentView.bottomAnchor)
            ])
        }

        if alpha.current == 0 && onboardingVC.view.superview != nil {

            backgroundView.removeFromSuperview()
            onboardingVC.view.removeFromSuperview()
        }
    }
}
