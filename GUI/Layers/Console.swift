// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation
import Carbon.HIToolbox

class Console: Layer {
    
    var window: NSWindow { return controller.window! }
    var contentView: NSView { return window.contentView! }
    let scrollView = NSTextView.scrollableTextView()
    
    var textView: NSTextView
    var textColor = NSColor.white
    var backgroundColor = NSColor(r: 0x80, g: 0x80, b: 0x80, a: 0x80)
    var isDirty = false
    
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {
        
        textView = (scrollView.documentView as? NSTextView)!
        textView.isEditable = false
        textView.backgroundColor = backgroundColor
        
        super.init(renderer: renderer)
        
        resize()
        isDirty = true
    }
    
    override func open() {
        
        super.open()
        resize()
    }
    
    override func update(frames: Int64) {
        
        super.update(frames: frames)
        
        let colorTable: [Int: (NSColor, NSColor)] = [
            
            // Commander console
            0: (NSColor(r: 0xCF, g: 0xCF, b: 0xFF, a: 0xFF),
                // NSColor(r: 0xEF, g: 0xEF, b: 0xEF, a: 0xFF),
                NSColor(r: 0x60, g: 0x60, b: 0x60, a: 0xD0)),
            /*
             0: (NSColor(r: 0xCF, g: 0xFF, b: 0xCF, a: 0xFF),
             NSColor(r: 0x30, g: 0x90, b: 0x30, a: 0xD0)),
             */
            
            // Debugger console
            1: (NSColor(r: 0xFF, g: 0xDF, b: 0xDF, a: 0xFF),
                // NSColor(r: 0xEF, g: 0xEF, b: 0xEF, a: 0xFF),
                NSColor(r: 0x60, g: 0x60, b: 0x60, a: 0xD0)),
            /*
             1: (NSColor(r: 0xFF, g: 0xCF, b: 0xCF, a: 0xFF),
             NSColor(r: 0x90, g: 0x30, b: 0x30, a: 0xD0)),
             */
            // Navigator console
            2: (NSColor(r: 0xDF, g: 0xFF, b: 0xDF, a: 0xFF),
                // NSColor(r: 0xEF, g: 0xEF, b: 0xEF, a: 0xFF),
                NSColor(r: 0x60, g: 0x60, b: 0x60, a: 0xD0)),
            /*
             2: (NSColor(r: 0xCF, g: 0xCF, b: 0xFF, a: 0xFF),
             NSColor(r: 0x30, g: 0x30, b: 0x90, a: 0xD0))
             */
        ]
        
        guard let emu = emu else { return }
        
        if isDirty {
            
            let info = emu.retroShell.info
            
            (textColor, backgroundColor) = colorTable[info.console]!
            textView.backgroundColor = backgroundColor
            
            if let text = emu.retroShell.getText() {
                
                let cursorColor = NSColor(r: 255, g: 255, b: 255, a: 128)
                // let cursorColor = textColor.withAlphaComponent(0.5)
                let monoFont = NSFont.monospaced(ofSize: 14, weight: .medium)
                let cpos = info.cursorRel - 1 // emu.retroShell.cursorRel - 1
                
                let attr = [
                    NSAttributedString.Key.foregroundColor: textColor,
                    NSAttributedString.Key.font: monoFont
                ]
                let string = NSMutableAttributedString(string: text, attributes: attr)
                string.addAttribute(.backgroundColor,
                                    value: cursorColor,
                                    range: NSRange(location: string.length + cpos, length: 1))
                textView.textStorage?.setAttributedString(string)
                
            } else {
                fatalError()
            }
            
            // Scroll to the end and update the text view
            textView.scrollToEndOfDocument(self)
            textView.layoutManager!.invalidateGlyphs(
                forCharacterRange: NSRange(location: 0, length: textView.string.utf16.count),
                changeInLength: 0,
                actualCharacterRange: nil)
            textView.layoutManager!.invalidateLayout(
                forCharacterRange: NSRange(location: 0, length: textView.string.utf16.count),
                actualCharacterRange: nil)
            
            isDirty = false
        }
    }
    
    override func alphaDidChange() {
        
        textView.textColor = textColor.withAlphaComponent(CGFloat(alpha.current))
        textView.backgroundColor = backgroundColor.withAlphaComponent(CGFloat(alpha.current * 0.8))
        
        if alpha.current > 0 && scrollView.superview == nil {
            contentView.addSubview(scrollView)
        }
        
        if alpha.current == 0 && scrollView.superview != nil {
            scrollView.removeFromSuperview()
        }
    }
    
    func resize() {
        
        let size = controller.metal.frame.size
        let origin = controller.metal.frame.origin
        let newSize = NSSize(width: size.width, height: size.height)
        
        scrollView.setFrameSize(newSize)
        scrollView.frame.origin = CGPoint(x: origin.x, y: origin.y)
        
        let drawableSize = controller.metal.drawableSize
        emu?.set(.HOST_FRAMEBUF_WIDTH, value: Int(drawableSize.width))
        emu?.set(.HOST_FRAMEBUF_HEIGHT, value: Int(drawableSize.height))
    }
    
    func keyDown(with event: NSEvent) {
        
        guard let emu = emu else { return }
        let macKey = MacKey(event: event)
        let shift  = macKey.modifierFlags.contains(.shift)
        let ctrl   = macKey.modifierFlags.contains(.control)
        
        switch macKey.keyCode {
            
        case kVK_ANSI_A where ctrl: emu.retroShell.pressSpecialKey(.HOME, shift: shift)
        case kVK_ANSI_E where ctrl: emu.retroShell.pressSpecialKey(.END, shift: shift)
        case kVK_ANSI_K where ctrl: emu.retroShell.pressSpecialKey(.CUT, shift: shift)
        case kVK_UpArrow: emu.retroShell.pressSpecialKey(.UP, shift: shift)
        case kVK_DownArrow: emu.retroShell.pressSpecialKey(.DOWN, shift: shift)
        case kVK_LeftArrow: emu.retroShell.pressSpecialKey(.LEFT, shift: shift)
        case kVK_RightArrow: emu.retroShell.pressSpecialKey(.RIGHT, shift: shift)
        case kVK_PageUp: emu.retroShell.pressSpecialKey(.PAGE_UP, shift: shift)
        case kVK_PageDown: emu.retroShell.pressSpecialKey(.PAGE_DOWN, shift: shift)
        case kVK_Home: emu.retroShell.pressSpecialKey(.HOME, shift: shift)
        case kVK_End: emu.retroShell.pressSpecialKey(.END, shift: shift)
        case kVK_Delete: emu.retroShell.pressSpecialKey(.BACKSPACE, shift: shift)
        case kVK_ForwardDelete: emu.retroShell.pressSpecialKey(.DEL, shift: shift)
        case kVK_Return: emu.retroShell.pressSpecialKey(.RETURN, shift: shift)
        case kVK_Tab: emu.retroShell.pressSpecialKey(.TAB, shift: shift)
        case kVK_Escape: close()
            
        default:
            
            if let c = event.characters?.utf8CString.first {
                emu.retroShell.pressKey(c)
            }
        }
        
        isDirty = true
    }
    
    func keyUp(with event: NSEvent) {
        
    }
    
    func runScript(url: URL) throws {

        open()
        try emu?.retroShell.executeScript(url: url)
        isDirty = true
    }
}
