// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

@MainActor
class OnboardingButton: NSControl {

    @IBOutlet weak var icon: NSImageView?
    @IBOutlet weak var label: NSTextField?

    let box = NSBox()
    private let clickButton = NSButton()

    var state: NSControl.StateValue = .off { didSet { update() } }

    @IBInspectable override var tag: Int {

        get { super.tag }
        set { super.tag = newValue }
    }

    override init(frame frameRect: NSRect) {

        super.init(frame: frameRect)
        setup()
    }

    required init?(coder: NSCoder) {

        super.init(coder: coder)
        setup()
    }

    var myFont: NSFont?

    private func setup() {

        wantsLayer = true

        // Box
        box.boxType = .primary
        box.titlePosition = .noTitle
        addSubview(box)

        // Clickable button (fills whole box, invisible)
        clickButton.bezelStyle = .regularSquare
        clickButton.isBordered = false
        clickButton.title = ""
        clickButton.target = self
        clickButton.action = #selector(buttonClicked)
        clickButton.imagePosition = .imageAbove
        addSubview(clickButton)
    }

    override func layout() {

        super.layout()
        box.frame = bounds
        clickButton.frame = bounds
    }

    @objc private func buttonClicked() {

        sendAction(action, to: target)
    }

    private func update() {

        switch state {

        case .on:
            box.boxType = .custom
            box.isTransparent = false
            box.borderWidth = 2
            box.cornerRadius = 8
            box.borderColor = .tertiaryLabelColor.withAlphaComponent(0.05)
            box.fillColor = .tertiaryLabelColor.withAlphaComponent(0.05)
            label?.textColor = .labelColor
            icon?.isEnabled = true

        default:
            box.isTransparent = true
            label?.textColor = .secondaryLabelColor
            icon?.isEnabled = false
        }
    }
}
