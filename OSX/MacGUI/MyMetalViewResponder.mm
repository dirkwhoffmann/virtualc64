/*
 * Author: Dirk W. Hoffmann, 2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if 0

#import "C64GUI.h"

@implementation MyMetalView(Responder)

// --------------------------------------------------------------------------------
//                                  Keyboard events
// --------------------------------------------------------------------------------

- (MacKeyFingerprint)fingerprintForKey:(int)keycode withModifierFlags:(unsigned long)flags
{
    return keycode;
    
    
#if 0
    // The recorded fingerprint consists of the keycode. If the key is a number key (0 - 9), the
    // fingerprint also contains the NSNumericPadKeyMask flag to distinguish keys from the
    // numeric keypad from "normal" keys.
    switch (keycode) {
        case kVK_ANSI_1: case kVK_ANSI_2: case kVK_ANSI_3: case kVK_ANSI_4: case kVK_ANSI_5:
        case kVK_ANSI_6: case kVK_ANSI_7: case kVK_ANSI_8: case kVK_ANSI_9: case kVK_ANSI_0:
            flags &= NSNumericPadKeyMask; // keep NSNumericPadKeyMask flag
            break;
            
        default:
            flags = 0; // standard case: we only keep the keycode
    }
    
    return keycode | flags;
#endif
}

- (MacKeyFingerprint)joyKeyFingerprint:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: return joyFingerprint[0][dir];
        case 2: return joyFingerprint[1][dir];
    }
    
    assert(0);
    return 0;
}

- (char)joyChar:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: return joyChar[0][dir];
        case 2: return joyChar[1][dir];
    }
    
    assert(0);
    return 0;
}

- (void)setJoyKeyFingerprint:(MacKeyFingerprint)key keymap:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: joyFingerprint[0][dir] = key; return;
        case 2: joyFingerprint[1][dir] = key; return;
    }
    
    assert(0);
}

- (void)setJoyChar:(char)c keymap:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: joyChar[0][dir] = c; return;
        case 2: joyChar[1][dir] = c; return;
    }
    
    assert(0);
}

- (BOOL)pullJoystick:(int)nr withKey:(MacKeyFingerprint)k device:(int)d
{
    assert (nr == 1 || nr == 2);
    
    if (d != IPD_KEYSET_1 && d != IPD_KEYSET_2)
        return NO;
    
    unsigned keyset = (d == IPD_KEYSET_1) ? 0 : 1;
    // Joystick *joy = (nr == 1) ? &c64->joystick1 : &c64->joystick2;
    JoystickProxy *j = (nr == 1) ? [c64proxy joystickA] : [c64proxy joystickB];
    
    if (k == joyFingerprint[keyset][JOYSTICK_UP]) { [j setAxisY:JOYSTICK_UP]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_DOWN]) { [j setAxisY:JOYSTICK_DOWN]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_LEFT]) { [j setAxisX:JOYSTICK_LEFT]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_RIGHT]) { [j setAxisX:JOYSTICK_RIGHT]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_FIRE]) { [j setButtonPressed:YES]; return YES; }
    
    return NO;
}

- (BOOL)releaseJoystick:(int)nr withKey:(MacKeyFingerprint)k device:(int)d
{
    assert (nr == 1 || nr == 2);
    
    if (d != IPD_KEYSET_1 && d != IPD_KEYSET_2)
        return NO;
    
    unsigned keyset = (d == IPD_KEYSET_1) ? 0 : 1;
    JoystickProxy *j = (nr == 1) ? [c64proxy joystickA] : [c64proxy joystickB];

    if (k == joyFingerprint[keyset][JOYSTICK_UP]) { [j setAxisY:JOYSTICK_RELEASED]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_DOWN]) { [j setAxisY:JOYSTICK_RELEASED]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_LEFT]) { [j setAxisX:JOYSTICK_RELEASED]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_RIGHT]) { [j setAxisX:JOYSTICK_RELEASED]; return YES; }
    if (k == joyFingerprint[keyset][JOYSTICK_FIRE]) { [j setButtonPressed:NO]; return YES; }

    return NO;
}

- (C64KeyFingerprint)translateKey:(char)key plainkey:(char)plainkey keycode:(short)keycode flags:(unsigned long)flags
{
    switch (keycode) {
        case MAC_F1: return C64KEY_F1;
        case MAC_F2: return C64KEY_F2;
        case MAC_F3: return C64KEY_F3;
        case MAC_F4: return C64KEY_F4;
        case MAC_F5: return C64KEY_F5;
        case MAC_F6: return C64KEY_F6;
        case MAC_F7: return C64KEY_F7;
        case MAC_F8: return C64KEY_F8;
        case MAC_DEL: return (flags & NSShiftKeyMask) ? C64KEY_INS : C64KEY_DEL;
        case MAC_RET: return C64KEY_RET;
        case MAC_CL: return C64KEY_CL;
        case MAC_CR: return C64KEY_CR;
        case MAC_CU: return C64KEY_CU;
        case MAC_CD: return C64KEY_CD;
        case MAC_ESC: return C64KEY_RUNSTOP;
        case MAC_TAB: return C64KEY_RESTORE;
        case MAC_HAT: return '^';
        case MAC_TILDE_US: if (plainkey != '<' && plainkey != '>') return C64KEY_ARROW; else break;
    }
    
    if (flags & NSAlternateKeyMask) {
        // Commodore key (ALT) is pressed
        return (int)plainkey | C64KEY_COMMODORE;
    } else if (flags & NSControlKeyMask) {
        // CTRL key is pressed
        return (int)plainkey | C64KEY_CTRL;
    } else {
        // No special translation needed here
        return (int)key;
    }
}

- (void)keyDown:(NSEvent *)event
{
    unsigned char  c       = [[event characters] UTF8String][0];
    unsigned char  c_unmod = [[event charactersIgnoringModifiers] UTF8String][0];
    unsigned short keycode = [event keyCode];
    unsigned long  flags   = [event modifierFlags];
    int c64key;
    
    // NSLog(@"keyDown: '%c' keycode: %02X flags: %08lX", (char)c, keycode, flags);
        
    // Ignore keys that are already pressed
    /*
    if ([alreadPressed containsObject:[NSNumber numberWithInt: keycode]]) {
        NSLog(@"Key already pressed");
        return;
    }
    */
    
    if (pressedKeys[(unsigned char)keycode])
        return;
    
    // Ignore command key
    if (flags & NSCommandKeyMask)
        return;
    
    // Simulate joysticks
    MacKeyFingerprint fingerprint = [self fingerprintForKey:keycode withModifierFlags:flags];
    if ([self pullJoystick:1 withKey:fingerprint device:[controller inputDeviceA]])
        return;
    if ([self pullJoystick:2 withKey:fingerprint device:[controller inputDeviceB]])
        return;
    
    // Remove alternate key modifier if present
    if (flags & NSAlternateKeyMask)
        c = [[event charactersIgnoringModifiers] UTF8String][0];

    // Translate key
    if (!(c64key = [self translateKey:c plainkey:c_unmod keycode:keycode flags:flags]))
        return;
    
    // Press key
    pressedKeys[(unsigned char)keycode] = c64key;
    [[c64proxy keyboard] pressKey:c64key];
}

- (void)keyUp:(NSEvent *)event
{
    unsigned short keycode = [event keyCode];
    unsigned long  flags   = [event modifierFlags];
    
    // NSLog(@"keyUp: keycode: %02X flags: %08lX", keycode, flags);
    
    // Simulate joysticks
    MacKeyFingerprint fingerprint = [self fingerprintForKey:keycode withModifierFlags:flags];
    if ([self releaseJoystick:1 withKey:fingerprint device:[controller inputDeviceA]])
        return;
    if ([self releaseJoystick:2 withKey:fingerprint device:[controller inputDeviceB]])
        return;
    
    // Only proceed if the released key is on the records
    if (!pressedKeys[(unsigned char)keycode])
        return;
    
    // Release key
    // NSLog(@"Releasing stored key %c for keycode %ld",pressedKeys[keycode], (long)keycode);
    [[c64proxy keyboard] releaseKey:pressedKeys[keycode]];
    pressedKeys[(unsigned char)keycode] = 0;
}

- (void)flagsChanged:(NSEvent *)event
{
    NSEventModifierFlags flags = [event modifierFlags];
    int keycode;

    // Store modifier flags
    [controller setModifierFlags:flags];
    
    // Check if special keys are used for joystick emulation
    if (flags & NSAlternateKeyMask)
        keycode = NSAlternateKeyMask;
        else if (flags & NSShiftKeyMask)
            keycode = NSShiftKeyMask;
            else if (flags & NSCommandKeyMask)
                keycode = NSCommandKeyMask;
                else if (flags & NSControlKeyMask)
                    keycode = NSControlKeyMask;
                    else {
                        (void)([self releaseJoystick:1 withKey:NSAlternateKeyMask device:[controller inputDeviceA]]);
                        (void)([self releaseJoystick:1 withKey:NSShiftKeyMask device:[controller inputDeviceA]]);
                        (void)([self releaseJoystick:1 withKey:NSCommandKeyMask device:[controller inputDeviceA]]);
                        (void)([self releaseJoystick:1 withKey:NSControlKeyMask device:[controller inputDeviceA]]);
                        (void)([self releaseJoystick:2 withKey:NSAlternateKeyMask device:[controller inputDeviceB]]);
                        (void)([self releaseJoystick:2 withKey:NSShiftKeyMask device:[controller inputDeviceB]]);
                        (void)([self releaseJoystick:2 withKey:NSCommandKeyMask device:[controller inputDeviceB]]);
                        (void)([self releaseJoystick:2 withKey:NSControlKeyMask device:[controller inputDeviceB]]);
                        return;
                    }
    
    // Pull joysticks
    if ([self pullJoystick:1 withKey:keycode device:[controller inputDeviceA]])
        return;
    if ([self pullJoystick:2 withKey:keycode device:[controller inputDeviceB]])
        return;
}



// --------------------------------------------------------------------------------
//                                  Drag and Drop
// --------------------------------------------------------------------------------

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    // NSLog(@"draggingEntered");
    if ([sender draggingSource] == self)
        return NSDragOperationNone;
    
    NSPasteboard *pb = [sender draggingPasteboard];
    NSString *besttype = [pb availableTypeFromArray:[NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];
    
    if (besttype == NSFilenamesPboardType) {
        // NSLog(@"Dragged in filename");
        return NSDragOperationCopy;
    }
    
    if (besttype == NSPasteboardTypeString) {
        // NSLog(@"Dragged in string");
        return NSDragOperationCopy;
    }
    
    if (besttype == NSFileContentsPboardType) {
        // NSLog(@"Dragged in file contents");
        return NSDragOperationCopy;
    }
    
    return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pb = [sender draggingPasteboard];
    
    if ([[pb types] containsObject:NSFileContentsPboardType]) {
        
        NSFileWrapper *fileWrapper = [pb readFileWrapper];
        NSData *fileData = [fileWrapper regularFileContents];
        SnapshotProxy *snapshot = [SnapshotProxy snapshotFromBuffer:[fileData bytes] length:(unsigned)[fileData length]];
        [[controller c64] loadFromSnapshot:snapshot];
        return YES;
    }
    
    if ([[pb types] containsObject:NSFilenamesPboardType]) {
        
        NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];
        NSLog(@"Processing file %@", path);
        
        // Is it a snapshot?
        if ([SnapshotProxy isSnapshotFile:path]) {
            
            NSLog(@"  Snapshot found");
            
            // Do the version numbers match?
            if ([SnapshotProxy isSnapshotFile:path
                                        major:V_MAJOR
                                        minor:V_MINOR
                                     subminor:V_SUBMINOR]) {
                
                SnapshotProxy *snapshot = [SnapshotProxy snapshotFromFile:path];
                if (snapshot) {
                    [[controller c64] loadFromSnapshot:snapshot];
                    return YES;
                }
                
            } else {
                
                NSLog(@"  ERROR: Version number in snapshot must be %d.%d", V_MAJOR, V_MINOR);
                [[controller document] showVersionNumberAlert];
                return NO;
            }
        }
        
        // Is it a ROM file?
        if ([[controller document] loadRom:path]) {
            return YES;
        }
        
        // Is it a cartridge?
        if ([[controller document] setCartridgeWithName:path]) {
            [controller mountCartridge];
            return YES;
        }

        // Is it a NIB archive?
        if ([[controller document] setNIBArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }

        // Is it a G64 archive?
        if ([[controller document] setG64ArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }
        
        // Is it a TAP container?
        if ([[controller document] setTAPContainerWithName:path]) {
            [controller showTapeDialog];
            return YES;
        }
        
        // Is it an archive other than G64?
        if ([[controller document] setArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }					
    }
    
    return NO;	
}

- (void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
}

@end

#endif

