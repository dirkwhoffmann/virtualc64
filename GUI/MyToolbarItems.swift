// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

/* This file provides wrapper classes for all toolbar elements.
 *
 * - MyToolbarButton
 *     A custom button used in all other elements.
 *
 * - MyToolbarItem
 *     A toolbar item containing a single button.
 *
 * - MyToolbarItemGroup
 *     A toolbar item with multiple buttons, mimicking the behavior of a
 *     segmented control.
 *
 * - MyToolbarMenuItem
 *     A toolbar item with an attached pull-down menu.
 */

class MyToolbarButton: NSButton {
    
    init(image: SFSymbol?, target: Any?, action: Selector?) {

        super.init(frame: .zero)
        
        if image != nil { self.image = SFSymbol.get(image!) }
        self.target = target as AnyObject
        self.action = action

        self.title = ""
        self.isBordered = false
        self.bezelStyle = .smallSquare
        self.imagePosition = .imageOnly
        self.imageScaling = .scaleProportionallyDown
        self.translatesAutoresizingMaskIntoConstraints = false
    }
    
    required init?(coder: NSCoder) {
        
        super.init(coder: coder)
    }
}

//
// Toolbar button or button group
//

class MyToolbarItem: MyToolbarItemGroup {

    var button: NSButton { buttons[0] }

    convenience init(identifier: NSToolbarItem.Identifier,
                     image: SFSymbol?, action: Selector?, target: AnyObject? = nil,
                     size: CGSize = CGSize(width: 26, height: 26),
                     label: String, paletteLabel: String? = nil) {

        self.init(identifier: identifier, images: [image], actions: [action],
                  target: target, size: size, label: label, paletteLabel: paletteLabel)
    }
}

class MyToolbarItemGroup: NSToolbarItem {
    
    let debug = false
    
    var buttons: [NSButton] = []
    
    override var isEnabled: Bool {
        
        get { super.isEnabled }
        set { super.isEnabled = newValue; for button in buttons { button.isEnabled = newValue } }
    }
    
    // Container view holding the buttons
    private let container: NSView = {
        
        let view = NSView()
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    
    init(identifier: NSToolbarItem.Identifier,
         images: [SFSymbol?], actions: [Selector?], target: AnyObject? = nil,
         size: CGSize = CGSize(width: 26, height: 26),
         label: String, paletteLabel: String? = nil) {
        
        assert(images.count == actions.count, "Mismatch in images and actions")
        
        super.init(itemIdentifier: identifier)
        
        self.view = container
        self.label = label
        self.paletteLabel = paletteLabel ?? label
        
        for i in 0..<images.count {
            
            let button = MyToolbarButton(image: images[i], target: target, action: actions[i])
            
            container.addSubview(button)
            buttons.append(button)
        }
        
        if debug {
            
            container.wantsLayer = true
            container.layer?.backgroundColor = NSColor.systemYellow.withAlphaComponent(0.3).cgColor
            
            for button in buttons {
                
                button.layer?.backgroundColor = NSColor.systemGreen.withAlphaComponent(0.3).cgColor
            }
        }
        
        setupConstraints(size: size)
    }
    
    private func setupConstraints(size: CGSize) {

        let padding: CGFloat = 8
        let spacing: CGFloat = 16
        
        // Container contraints
        container.heightAnchor.constraint(equalToConstant: size.height).isActive = true

        // Button constraints
        for (index, button) in buttons.enumerated() {
            
            // Height
            button.centerYAnchor.constraint(equalTo: container.centerYAnchor).isActive = true
            
            // Width
            button.widthAnchor.constraint(equalToConstant: size.width).isActive = true

            if index == 0 {
                button.leadingAnchor.constraint(equalTo: container.leadingAnchor,
                                                constant: padding).isActive = true
            } else {
                button.leadingAnchor.constraint(equalTo: buttons[index - 1].trailingAnchor,
                                                constant: spacing).isActive = true
            }
            if index == buttons.count - 1 {
                button.trailingAnchor.constraint(equalTo: container.trailingAnchor,
                                                 constant: -padding).isActive = true
            }
        }
    }
    
    func setEnabled(_ enabled: Bool, forSegment segment: Int) {
        
        buttons[segment].isEnabled = enabled
    }
    
    func setToolTip(_ toolTip: String, forSegment segment: Int) {
        
        buttons[segment].toolTip = toolTip
    }
    
    func button(at index: Int) -> NSButton? {
        
        guard index >= 0 && index < buttons.count else { return nil }
        return buttons[index]
    }
    
    func setImage(_ image: NSImage?, forSegment segment: Int) {
        
        buttons[segment].image = image
    }
}

//
// MyToolbarMenuItem
//

class MyToolbarMenuItem: MyToolbarItem {
    
    var items: [(SFSymbol, String, Int)] = []
    var finalAction: Selector?
    var finalTarget: AnyObject?
    
    var menu = NSMenu()
    
    convenience init(identifier: NSToolbarItem.Identifier,
                     menuItems: [(SFSymbol, String, Int)],
                     image: SFSymbol, action: Selector, target: AnyObject?,
                     label: String, paletteLabel: String? = nil) {
        
        self.init(identifier: identifier,
                  image: image, action: #selector(showMenu),
                  label: label, paletteLabel: paletteLabel)
        
        self.items = menuItems
        self.finalAction = action
        self.finalTarget = target
        self.buttons[0].target = self
        
        for (image, title, tag) in items {
            
            let item = menu.addItem(withTitle: title, action: #selector(menuAction), keyEquivalent: "")
            item.tag = tag
            item.image = SFSymbol.get(image)
            item.target = self
        }
    }
    
    func selectItem(withTag tag: Int) {
        
        for item in menu.items {
            item.state = item.tag == tag ? .on : .off
        }
        buttons[0].image = menu.item(withTag: tag)?.image
    }
    
    @objc func menuAction(_ sender: NSMenuItem) {
        
        NSApp.sendAction(finalAction!, to: finalTarget!, from: sender)
    }
    
    @objc func showMenu() {
        
        let point = NSPoint(x: 0, y: self.buttons[0].bounds.height)
        menu.popUp(positioning: nil, at: point, in: self.buttons[0])
    }
    
    override func validateMenuItem(_ menuItem: NSMenuItem) -> Bool {
        
        return menuItem.isEnabled
    }
}
