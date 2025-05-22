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
        
    var isDirty = false
        
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {

        textView = (scrollView.documentView as? NSTextView)!
        textView.isEditable = false
        textView.backgroundColor = NSColor(r: 0x80, g: 0x80, b: 0x80, a: 0x80)
        
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

		if isDirty && emu != nil {

			if let text = emu!.retroShell.getText() {

				let cursorColor = NSColor(r: 255, g: 255, b: 255, a: 128)
				let monoFont = NSFont.monospaced(ofSize: 14, weight: .medium)
				let cpos = emu!.retroShell.cursorRel - 1

				let attr = [
					NSAttributedString.Key.foregroundColor: NSColor.white,
					NSAttributedString.Key.font: monoFont
				]
				let string = NSMutableAttributedString(string: text, attributes: attr)
				string.addAttribute(.backgroundColor,
									value: cursorColor,
									range: NSRange(location: string.length + cpos, length: 1))
				textView.textStorage?.setAttributedString(string)
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
                
        let a1 = Int(alpha.current * 0xFF)
        let a2 = Int(alpha.current * 0.8 * 0xFF)
        textView.textColor = NSColor(r: 0xFF, g: 0xFF, b: 0xFF, a: a1)
        textView.backgroundColor = NSColor(r: 0x80, g: 0x80, b: 0x80, a: a2)
        
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
    }
    
    func keyDown(with event: NSEvent) {
        
        if let emu = emu {

            let macKey = MacKey(event: event)
            let shift  = macKey.modifierFlags.contains(.shift)
            let ctrl   = macKey.modifierFlags.contains(.control)

            switch macKey.keyCode {

            case kVK_ANSI_A where ctrl: emu.retroShell.pressSpecialKey(.HOME)
            case kVK_ANSI_E where ctrl: emu.retroShell.pressSpecialKey(.END)
            case kVK_ANSI_K where ctrl: emu.retroShell.pressSpecialKey(.CUT)
            case kVK_UpArrow: emu.retroShell.pressSpecialKey(.UP)
            case kVK_DownArrow: emu.retroShell.pressSpecialKey(.DOWN)
            case kVK_LeftArrow: emu.retroShell.pressSpecialKey(.LEFT)
            case kVK_RightArrow: emu.retroShell.pressSpecialKey(.RIGHT)
            case kVK_Home: emu.retroShell.pressSpecialKey(.HOME)
            case kVK_End: emu.retroShell.pressSpecialKey(.END)
            case kVK_Delete: emu.retroShell.pressSpecialKey(.BACKSPACE)
            case kVK_ForwardDelete: emu.retroShell.pressSpecialKey(.DEL)
            case kVK_Return: emu.retroShell.pressSpecialKey(.RETURN, shift: shift)
            case kVK_Tab: emu.retroShell.pressSpecialKey(.TAB)
            case kVK_Escape: close()

            default:

                if let c = event.characters?.utf8CString.first {
                    emu.retroShell.pressKey(c)
                }
            }

            isDirty = true
        }
    }
    
    func keyUp(with event: NSEvent) {
        
    }

    func runScript(script: MediaFileProxy) {

        emu?.retroShell.executeScript(script)
        isDirty = true
    }
}
