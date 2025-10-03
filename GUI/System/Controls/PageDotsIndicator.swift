// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

@MainActor
protocol PageDotsIndicatorDelegate: AnyObject {

    func pageDotsIndicator(_ indicator: PageDotsIndicator, didSelectPage page: Int)
}

@MainActor
class PageDotsIndicator: NSView {

    var numberOfPages: Int = 0 { didSet { rebuildDots() } }
    var currentPage: Int = 0 { didSet { updateDots() } }

    var delegate: PageDotsIndicatorDelegate?

    private let stack = NSStackView()
    private var dots: [DotView] = []

    override init(frame frameRect: NSRect) {

        super.init(frame: frameRect)
        commonInit()
    }

    required init?(coder: NSCoder) {

        super.init(coder: coder)
        commonInit()
    }

    private func commonInit() {

        stack.orientation = .horizontal
        stack.alignment = .centerY
        stack.spacing = 8
        stack.translatesAutoresizingMaskIntoConstraints = false
        addSubview(stack)

        NSLayoutConstraint.activate([
            stack.centerXAnchor.constraint(equalTo: centerXAnchor),
            stack.centerYAnchor.constraint(equalTo: centerYAnchor)
        ])
    }

    private func rebuildDots() {

        stack.arrangedSubviews.forEach { $0.removeFromSuperview() }
        dots.removeAll()

        for i in 0..<numberOfPages {

            let dot = DotView()
            dot.translatesAutoresizingMaskIntoConstraints = false
            dot.widthAnchor.constraint(equalToConstant: 12).isActive = true
            dot.heightAnchor.constraint(equalToConstant: 12).isActive = true
            dot.onClick = { [weak self] in

                self?.currentPage = i
                self?.delegate?.pageDotsIndicator(self!, didSelectPage: i)
            }
            stack.addArrangedSubview(dot)
            dots.append(dot)
        }
        updateDots()
    }

    private func updateDots() {

        for (i, dot) in dots.enumerated() {
            dot.isActive = (i == currentPage)
        }
    }
}

private class DotView: NSView {

    var isActive = false { didSet { needsDisplay = true } }

    var onClick: (() -> Void)?

    override var intrinsicContentSize: NSSize {

        return NSSize(width: 12, height: 12)
    }

    override func draw(_ dirtyRect: NSRect) {

        let rect = bounds.insetBy(dx: 2, dy: 2)
        let path = NSBezierPath(ovalIn: rect)
        let color = isActive ? NSColor.labelColor : NSColor.secondaryLabelColor.withAlphaComponent(0.3)
        color.setFill()
        path.fill()
    }

    override func mouseDown(with event: NSEvent) {

        onClick?()
    }
}
