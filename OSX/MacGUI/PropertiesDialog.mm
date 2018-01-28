/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
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

#import "C64GUI.h"
#import "VirtualC64-Swift.h"

@implementation PropertiesDialog

// System
NSString *VC64BasicRomFileKey  = @"VC64BasicRomFileKey";
NSString *VC64CharRomFileKey   = @"VC64CharRomFileKey";
NSString *VC64KernelRomFileKey = @"VC64KernelRomFileKey";
NSString *VC64VC1541RomFileKey = @"VC64VC1541RomFileKey";

// Keyboard
NSString *VC64DisconnectEmulationKeys = @"VC64DisconnectKeys";

// Video
NSString *VC64EyeX             = @"VC64EyeX";
NSString *VC64EyeY             = @"VC64EyeY";
NSString *VC64EyeZ             = @"VC64EyeZ";
NSString *VC64ColorSchemeKey   = @"VC64ColorSchemeKey";
NSString *VC64VideoUpscalerKey = @"VC64VideoUpscalerKey";
NSString *VC64VideoFilterKey   = @"VC64VideoFilterKey";
NSString *VC64FullscreenKeepAspectRatioKey = @"VC64FullscreenKeepAspectRatioKey";

- (void)initialize:(MyController *)mycontroller
{
    controller = mycontroller;
    c64 = [controller c64];
    recordKey1 = -1;
    recordKey2 = -1;
	[self update];
}

- (IBAction)useAsDefaultAction:(id)sender
{
    NSLog(@"Saving emulator user defaults");
    [controller saveUserDefaults];
}

- (IBAction)factorySettingsAction:(id)sender
{
    NSLog(@"Restoring emulator factoring settings");
    
    // Joystick and keyboard
    [controller restoreFactorySettingsKeyboard];
    
    // Video
    [controller setEyeX:(float)0.0];
    [controller setEyeY:(float)0.0];
    [controller setEyeZ:(float)0.0];
    [[c64 vic] setColorScheme:VICE];
    [controller setVideoUpscaler:1];
    [controller setVideoFilter:2];
    [controller setFullscreenKeepAspectRatio:NO];
    
    
    [self update];
    [self useAsDefaultAction:self];
}

- (IBAction)recordKeyAction:(id)sender
{
    if ([sender tag] >= 0 && [sender tag] <= 4) {
    
        recordKey1 = [sender tag];
        recordKey2 = -1;
        
    } else if ([sender tag] >= 10 && [sender tag] <= 14) {
        
        recordKey1 = -1;
        recordKey2 = [sender tag] - 10;
        
    } else { assert(0); }

    [self update];
 }

- (IBAction)disconnectKeysAction:(id)sender
{
    NSLog(@"disconnectKeysAction");
    [controller setDisconnectEmulationKeys:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)changeColorScheme:(id)sender
{
	long scheme = [[sender selectedItem] tag];
    [[c64 vic] setColorScheme:scheme];
	[self update];    
}

- (IBAction)setUpscalerAction:(id)sender
{
    long upscaler = [[sender selectedItem] tag];
    [controller setVideoUpscaler:upscaler];
    [self update];
}

- (IBAction)setFilterAction:(id)sender
{
    long filter = [[sender selectedItem] tag];
    [controller setVideoFilter:filter];
    [self update];
}

- (IBAction)setEyeXAction:(id)sender
{
    [controller setEyeX:[sender floatValue]];
    [self update];
}

- (IBAction)setEyeYAction:(id)sender
{
    [controller setEyeY:[sender floatValue]];
    [self update];
}

- (IBAction)setEyeZAction:(id)sender
{
    NSLog(@"setEyeZAction");
    
    [controller setEyeZ:[sender floatValue]];
    [self update];
}

- (IBAction)setFullscreenAspectRatio:(id)sender
{
    NSLog(@"setFullscreenAspectRatio");
    
    [controller setFullscreenKeepAspectRatio:[(NSButton *)sender state]];
    NSLog(@"ar: %d", [controller fullscreenKeepAspectRatio]);
    [self update];
}


- (NSString *)keycodeInPlainText:(int)code character:(char)c
{
    // Check for special keys
    switch (code) {
        case 123: return @"\u2190"; // Cursor left
        case 124: return @"\u2192"; // Cursor right
        case 125: return @"\u2193"; // Cursor down
        case 126: return @"\u2191"; // Cursor up
        case 49:  return @"\u2423"; // Space
    }
    
    // Return character a a string
    return [NSString stringWithFormat:@"%c" , c];
}

- (void)updateKeymap:(int)nr direction:(JoystickDirection)dir button:(NSButton *)b text:(NSTextField *)t
{
    assert(dir >= 0 && dir <= 4);
    assert(nr == 1 || nr == 2);
    
    KeyMap *map = [[controller gamePadManager] keysetOfDevice:(nr - 1)];
    MacKeyFingerprint fp = [map fingerprintFor:dir];
    NSString *s = [map getCharacterFor:dir];
    
    // Change button text and image
    if (nr == 1) {
        [b setTitle:(recordKey1 == dir ? @"" : [@(fp) stringValue])];
        [b setImage:[NSImage imageNamed:(recordKey1 == dir ? @"LEDnewRed" : @"")]];
    } else if (nr == 2) {
        [b setTitle:(recordKey2 == dir ? @"" : [@(fp) stringValue])];
        [b setImage:[NSImage imageNamed:(recordKey2 == dir ? @"LEDnewRed" : @"")]];
    }
    
    // Change appearance for some special keys
    switch (fp) {
        case NSAlternateKeyMask: s = @"\u2325"; break;
        case NSShiftKeyMask: s = @"\u21E7"; break;
        case NSCommandKeyMask: s = @"\u2318"; break;
        case NSControlKeyMask: s = @"\u2303"; break;
        case 123: s = @"\u2190"; break; // Cursor left
        case 124: s = @"\u2192"; break; // Cursor right
        case 125: s = @"\u2193"; break; // Cursor down
        case 126: s = @"\u2191"; break; // Cursor up
        case 49:  s = @"\u2423"; break; // Space
    }
    
    [t setStringValue:s];
}

- (void)update
{
    /* Joystick */
    
    // First key set
    [self updateKeymap:1 direction:JoystickDirection(UP) button:up1button text:up1];
    [self updateKeymap:1 direction:JoystickDirection(DOWN) button:down1button text:down1];
    [self updateKeymap:1 direction:JoystickDirection(LEFT) button:left1button text:left1];
    [self updateKeymap:1 direction:JoystickDirection(RIGHT) button:right1button text:right1];
    [self updateKeymap:1 direction:JoystickDirection(FIRE) button:fire1button text:fire1];
    
    // Second key set
    [self updateKeymap:2 direction:JoystickDirection(UP) button:up2button text:up2];
    [self updateKeymap:2 direction:JoystickDirection(DOWN) button:down2button text:down2];
    [self updateKeymap:2 direction:JoystickDirection(LEFT) button:left2button text:left2];
    [self updateKeymap:2 direction:JoystickDirection(RIGHT) button:right2button text:right2];
    [self updateKeymap:2 direction:JoystickDirection(FIRE) button:fire2button text:fire2];
    
    /* Keyboard */
    [disconnectKeys setState:[controller getDisconnectEmulationKeys]];
    
	/* Video */
    [upscaler selectItemWithTag:[controller videoUpscaler]];
    [filter selectItemWithTag:[controller videoFilter]];
    [colorScheme selectItemWithTag:[[c64 vic] colorScheme]];

    [eyeXSlider setFloatValue:[controller eyeX]];
    [eyeYSlider setFloatValue:[controller eyeY]];
    [eyeZSlider setFloatValue:[controller eyeZ]];
    [aspectRatioButton setState:[controller fullscreenKeepAspectRatio]];
    
    [colorWell0 setColor:[[c64 vic] color:0]];
	[colorWell1 setColor:[[c64 vic] color:1]];
	[colorWell2 setColor:[[c64 vic] color:2]];
	[colorWell3 setColor:[[c64 vic] color:3]];
	[colorWell4 setColor:[[c64 vic] color:4]];
	[colorWell5 setColor:[[c64 vic] color:5]];
	[colorWell6 setColor:[[c64 vic] color:6]];
	[colorWell7 setColor:[[c64 vic] color:7]];
	[colorWell8 setColor:[[c64 vic] color:8]];
	[colorWell9 setColor:[[c64 vic] color:9]];
	[colorWell10 setColor:[[c64 vic] color:10]];
	[colorWell11 setColor:[[c64 vic] color:11]];
	[colorWell12 setColor:[[c64 vic] color:12]];
	[colorWell13 setColor:[[c64 vic] color:13]];
	[colorWell14 setColor:[[c64 vic] color:14]];
	[colorWell15 setColor:[[c64 vic] color:15]];
}

- (void)keyDown:(NSEvent *)event
{
    unsigned short keycode = [event keyCode];
    // unsigned char  c = [[event characters] UTF8String][0];
    NSEventModifierFlags flags = [event modifierFlags];
    
    MacKeyFingerprint f = [KeyboardController fingerprintForKey:keycode withModifierFlags:flags];

    if (recordKey1 != -1) {
        
        KeyMap *map = [[controller gamePadManager] keysetOfDevice:0];
        [map setFingerprint:f for:(JoystickDirection)recordKey1];
        [map setCharacter:[event characters] for:(JoystickDirection)recordKey1];
     }

    if (recordKey2 != -1) {
        
        KeyMap *map = [[controller gamePadManager] keysetOfDevice:1];
        [map setFingerprint:f for:(JoystickDirection)recordKey2];
        [map setCharacter:[event characters] for:(JoystickDirection)recordKey2];
    }

    recordKey1 = -1;
    recordKey2 = -1;
    [self update];
    return;
}

- (void)flagsChanged:(NSEvent *)event
{
    NSEventModifierFlags flags = [event modifierFlags];
    // int keycode;
    
    NSLog(@"PREFPANEL flagsChanged: %ld", (long)flags);
    
    NSLog(@"%ld %ld %ld", (long)NSAlternateKeyMask, (long)NSShiftKeyMask, (long)NSCommandKeyMask);
    
    
    // Check if one of the supported special keys has been pressed or released
    if (flags & NSAlternateKeyMask)
        // keycode = NSAlternateKeyMask;
        flags = NSAlternateKeyMask;
    else if (flags & NSShiftKeyMask)
        // keycode = NSShiftKeyMask;
        flags = NSShiftKeyMask;
    else if (flags & NSCommandKeyMask)
        // keycode = NSCommandKeyMask;
        flags = NSCommandKeyMask;
    else if (flags & NSControlKeyMask)
        // keycode = NSControlKeyMask;
        flags = NSControlKeyMask;
    else {
        // Special key released
        return;
    }
    
    // First keyset
    if (recordKey1 != -1) {
        
        KeyMap *map = [[controller gamePadManager] keysetOfDevice:0];
        [map setFingerprint:flags for:(JoystickDirection)recordKey1];
        [map setCharacter:@" " for:(JoystickDirection)recordKey1];
    }
    
    // Second keyset
    if (recordKey2 != -1) {
        
        KeyMap *map = [[controller gamePadManager] keysetOfDevice:1];
        [map setFingerprint:flags for:(JoystickDirection)recordKey2];
        [map setCharacter:@" " for:(JoystickDirection)recordKey2];
    }
    
    recordKey1 = -1;
    recordKey2 = -1;
    [self update];
}

@end
