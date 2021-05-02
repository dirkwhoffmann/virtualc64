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
 
    let controller: MyController
    
    var window: NSWindow { return controller.window! }
    var contentView: NSView { return window.contentView! }
    
    let scrollView = NSTextView.scrollableTextView()
    var textView: NSTextView
        
    var isDirty = false
        
    //
    // Initializing
    //
    
    override init(renderer: Renderer) {
        
        controller = renderer.parent
        
        textView = (scrollView.documentView as? NSTextView)!
        textView.isEditable = false
        textView.backgroundColor = NSColor.init(r: 0x80, g: 0x80, b: 0x80, a: 0x80)
        
        super.init(renderer: renderer)
        
        resize()
        isDirty = true
    }

    override func open() {
    
        track()
        
        super.open()
        resize()
    }

    override func update(frames: Int64) {
        
        super.update(frames: frames)

        if isDirty {
            
            if let text = c64.retroShell.getText() {
                
                let cursorColor = NSColor.init(r: 255, g: 255, b: 255, a: 128)
                let monoFont = NSFont.monospaced(ofSize: 14, weight: .medium)
                let cpos = c64.retroShell.cposRel
                
                let attr = [
                    NSAttributedString.Key.foregroundColor: NSColor.white,
                    NSAttributedString.Key.font: monoFont
                ]
                let string = NSMutableAttributedString(string: text, attributes: attr)
                string.addAttribute(.backgroundColor,
                                    value: cursorColor,
                                    range: NSRange(location: string.length - 1 - cpos, length: 1))
                textView.textStorage?.setAttributedString(string)

            } else {
                track("ERROR: text is NULL\n")
            }
            
            textView.scrollToEndOfDocument(self)
            isDirty = false
        }
    }
        
    override func alphaDidChange() {
                
        let a1 = Int(alpha.current * 0xFF)
        let a2 = Int(alpha.current * 0.8 * 0xFF)
        textView.textColor = NSColor.init(r: 0xFF, g: 0xFF, b: 0xFF, a: a1)
        textView.backgroundColor = NSColor.init(r: 0x80, g: 0x80, b: 0x80, a: a2)
        
        if alpha.current > 0 && scrollView.superview == nil {
            
            track("Adding console sub view")
            contentView.addSubview(scrollView)
        }
        
        if alpha.current == 0 && scrollView.superview != nil {
        
            track("Removing console sub view")
            scrollView.removeFromSuperview()
        }
    }
    
    func resize() {
                
        let size = controller.metal.frame.size
        let origin = controller.metal.frame.origin
        let newSize = NSSize.init(width: size.width, height: size.height)
        
        scrollView.setFrameSize(newSize)
        scrollView.frame.origin = CGPoint.init(x: origin.x, y: origin.y)
    }
    
    func keyDown(with event: NSEvent) {
        
        let macKey = MacKey.init(event: event)
        
        switch macKey.keyCode {
        
        case kVK_UpArrow: c64.retroShell.pressUp()
        case kVK_DownArrow: c64.retroShell.pressDown()
        case kVK_LeftArrow: c64.retroShell.pressLeft()
        case kVK_RightArrow: c64.retroShell.pressRight()
        case kVK_Home: c64.retroShell.pressHome()
        case kVK_End: c64.retroShell.pressEnd()
        case kVK_Delete: c64.retroShell.pressBackspace()
        case kVK_ForwardDelete: c64.retroShell.pressDelete()
        case kVK_Return: c64.retroShell.pressReturn()
        case kVK_Tab: c64.retroShell.pressTab()
        case kVK_Escape: close()
        
        default:
            
            if let c = event.characters?.utf8CString.first {
                c64.retroShell.pressKey(c)
            }
        }
        
        isDirty = true
    }
    
    func keyUp(with event: NSEvent) {
        
    }
    
    func runScript(script: ScriptProxy) {
        
        open()
        script.execute(c64)
        isDirty = true
    }
}
