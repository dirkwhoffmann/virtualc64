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

@implementation PropertiesDialog

// System
NSString *VC64BasicRomFileKey = @"VC64BasicRomFileKey";
NSString *VC64CharRomFileKey  = @"VC64CharRomFileKey";
NSString *VC64KernelRomFileKey= @"VC64KernelRomFileKey";
NSString *VC64VC1541RomFileKey= @"VC64VC1541RomFileKey";

// Joystick
NSString *VC64Left1keycodeKey = @"VC64Left1keycodeKey";
NSString *VC64Left1charKey = @"VC64Left1charKey";
NSString *VC64Right1keycodeKey = @"VC64Right1keycodeKey";
NSString *VC64Right1charKey = @"VC64Right1charKey";
NSString *VC64Up1keycodeKey = @"VC64Up1keycodeKey";
NSString *VC64Up1charKey = @"VC64Up1charKey";
NSString *VC64Down1keycodeKey = @"VC64Down1keycodeKey";
NSString *VC64Down1charKey = @"VC64Down1charKey";
NSString *VC64Fire1keycodeKey = @"VC64Fire1keycodeKey";
NSString *VC64Fire1charKey = @"VC64Fire1charKey";
NSString *VC64Left2keycodeKey = @"VC64Left2keycodeKey";
NSString *VC64Left2charKey = @"VC64Left2charKey";
NSString *VC64Right2keycodeKey = @"VC64Right2keycodeKey";
NSString *VC64Right2charKey = @"VC64Right2charKey";
NSString *VC64Up2keycodeKey = @"VC64Up2keycodeKey";
NSString *VC64Up2charKey = @"VC64Up2charKey";
NSString *VC64Down2keycodeKey = @"VC64Down2keycodeKey";
NSString *VC64Down2charKey = @"VC64Down2charKey";
NSString *VC64Fire2keycodeKey = @"VC64Fire2keycodeKey";
NSString *VC64Fire2charKey = @"VC64Fire2charKey";

// Video
NSString *VC64EyeX            = @"VC64EyeX";
NSString *VC64EyeY            = @"VC64EyeY";
NSString *VC64EyeZ            = @"VC64EyeZ";
NSString *VC64ColorSchemeKey  = @"VC64ColorSchemeKey";
NSString *VC64VideoFilterKey  = @"VC64VideoFilterKey";

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
    
    // Joystick
    [[controller screen] setJoyKeycode:126 keymap:1 direction:JOYSTICK_UP];
    [[controller screen] setJoyChar:' ' keymap:1 direction:JOYSTICK_UP];
    [[controller screen] setJoyKeycode:125 keymap:1 direction:JOYSTICK_DOWN];
    [[controller screen] setJoyChar:' ' keymap:1 direction:JOYSTICK_DOWN];
    [[controller screen] setJoyKeycode:123 keymap:1 direction:JOYSTICK_LEFT];
    [[controller screen] setJoyChar:' ' keymap:1 direction:JOYSTICK_LEFT];
    [[controller screen] setJoyKeycode:124 keymap:1 direction:JOYSTICK_RIGHT];
    [[controller screen] setJoyChar:' ' keymap:1 direction:JOYSTICK_RIGHT];
    [[controller screen] setJoyKeycode:49 keymap:1 direction:JOYSTICK_FIRE];
    [[controller screen] setJoyChar:' ' keymap:1 direction:JOYSTICK_FIRE];

    [[controller screen] setJoyKeycode:13 keymap:2 direction:JOYSTICK_UP];
    [[controller screen] setJoyChar:'w' keymap:2 direction:JOYSTICK_UP];
    [[controller screen] setJoyKeycode:6 keymap:2 direction:JOYSTICK_DOWN];
    [[controller screen] setJoyChar:'y' keymap:2 direction:JOYSTICK_DOWN];
    [[controller screen] setJoyKeycode:0 keymap:2 direction:JOYSTICK_LEFT];
    [[controller screen] setJoyChar:'a' keymap:2 direction:JOYSTICK_LEFT];
    [[controller screen] setJoyKeycode:1 keymap:2 direction:JOYSTICK_RIGHT];
    [[controller screen] setJoyChar:'s' keymap:2 direction:JOYSTICK_RIGHT];
    [[controller screen] setJoyKeycode:7 keymap:2 direction:JOYSTICK_FIRE];
    [[controller screen] setJoyChar:'x' keymap:2 direction:JOYSTICK_FIRE];

    // Video
    [[controller screen] setEyeX:(float)0.0];
    [[controller screen] setEyeY:(float)0.0];
    [[controller screen] setEyeZ:(float)0.0];
    [c64 setVideoFilter:GLFILTER_ANTI_ALIASING];
    [c64 setColorScheme:CCS64];
    
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

- (IBAction)changeColorScheme:(id)sender
{
	int scheme = [[sender selectedItem] tag];
    [c64 setColorScheme:scheme];
	[self update];    
}

- (IBAction)setVideoFilterAction:(id)sender
{
    int filter = [[sender selectedItem] tag];
    [c64 setVideoFilter:filter];
    [self update];
}

- (IBAction)setEyeXAction:(id)sender
{
	[[controller screen] setEyeX:[sender floatValue]];
    [self update];
}

- (IBAction)setEyeYAction:(id)sender
{
	[[controller screen] setEyeY:[sender floatValue]];
    [self update];
}

- (IBAction)setEyeZAction:(id)sender
{
 	[[controller screen] setEyeZ:[sender floatValue]];
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

- (void)updateKeymap:(int)map direction:(JoystickDirection)dir button:(NSButton *)b text:(NSTextField *)t
{
    int code = [[controller screen] joyKeycode:map direction:dir];
    char c = [[controller screen] joyChar:map direction:dir];
    
    // Change button text and image
    if (map == 1) {
        [b setTitle:(recordKey1 == dir ? @"" : [@(code) stringValue])];
        [b setImage:[NSImage imageNamed:(recordKey1 == dir ? @"LEDnewRed" : @"")]];
    } else if (map == 2) {
        [b setTitle:(recordKey2 == dir ? @"" : [@(code) stringValue])];
        [b setImage:[NSImage imageNamed:(recordKey2 == dir ? @"LEDnewRed" : @"")]];
    }
    
    // Convert keycode to plain text
    NSString *str = [NSString stringWithFormat:@"%c" , c];
    
    // Change appearance for some special keys
    switch (code) {
        case NSAlternateKeyMask: str = @"\u2325"; break;
        case NSShiftKeyMask: str = @"\u21E7"; break;
        case NSCommandKeyMask: str = @"\u2318"; break;
        case NSControlKeyMask: str = @"\u2303"; break;
        case 123: str = @"\u2190"; break; // Cursor left
        case 124: str = @"\u2192"; break; // Cursor right
        case 125: str = @"\u2193"; break; // Cursor down
        case 126: str = @"\u2191"; break; // Cursor up
        case 49:  str = @"\u2423"; break; // Space
    }
    
    [t setStringValue:str];
}

- (void)update
{	    
    /* Joystick */
    
    // First key set
    [self updateKeymap:1 direction:JOYSTICK_UP button:up1button text:up1];
    [self updateKeymap:1 direction:JOYSTICK_DOWN button:down1button text:down1];
    [self updateKeymap:1 direction:JOYSTICK_LEFT button:left1button text:left1];
    [self updateKeymap:1 direction:JOYSTICK_RIGHT button:right1button text:right1];
    [self updateKeymap:1 direction:JOYSTICK_FIRE button:fire1button text:fire1];
    
    // Second key set
    [self updateKeymap:2 direction:JOYSTICK_UP button:up2button text:up2];
    [self updateKeymap:2 direction:JOYSTICK_DOWN button:down2button text:down2];
    [self updateKeymap:2 direction:JOYSTICK_LEFT button:left2button text:left2];
    [self updateKeymap:2 direction:JOYSTICK_RIGHT button:right2button text:right2];
    [self updateKeymap:2 direction:JOYSTICK_FIRE button:fire2button text:fire2];
    
	/* Video */
    [videoFilter selectItemWithTag:[c64 videoFilter]];
    [colorScheme selectItemWithTag:[c64 colorScheme]];

    [eyeXSlider setFloatValue:[[controller screen] eyeX]]; 
    [eyeYSlider setFloatValue:[[controller screen] eyeY]]; 
    [eyeZSlider setFloatValue:[[controller screen] eyeZ]]; 
      
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
    // NSLog(@"PREFPANEL keyDown: %ld", (long)[event keyCode]);
    // NSLog(@"PREFPANEL record1: %ld record2: %ld", (long)recordKey1, (long)recordKey2);

    unsigned short keycode = [event keyCode];
    unsigned char  c       = [[event characters] UTF8String][0];
    int            flags   = [event modifierFlags];
    
    /*
    NSLog(@"Recording keycode: %04X (modifierFlags: %08X)\n", keycode, flags);
    if (flags & NSAlphaShiftKeyMask) NSLog(@"NSAlphaShiftKeyMask ");
    if (flags & NSShiftKeyMask) NSLog(@"NSShiftKeyMask ");
    if (flags & NSControlKeyMask) NSLog(@"NSControlKeyMask ");
    if (flags & NSAlternateKeyMask) NSLog(@"NSAlternateKeyMask ");
    if (flags & NSCommandKeyMask) NSLog(@"NSCommandKeyMask ");
    if (flags & NSNumericPadKeyMask) NSLog(@"NSNumericPadKeyMask ");
    if (flags & NSHelpKeyMask) NSLog(@"NSHelpKeyMask ");
    if (flags & NSFunctionKeyMask) NSLog(@"NSFunctionKeyMask ");
    */
    
    int fingerprint = [[controller screen] fingerprintForKey:keycode withModifierFlags:flags];
    if (recordKey1 != -1) {
        [[controller screen] setJoyKeycode:fingerprint keymap:1 direction:(JoystickDirection)recordKey1];
        [[controller screen] setJoyChar:c keymap:1 direction:(JoystickDirection)recordKey1];
    }

    if (recordKey2 != -1) {
        [[controller screen] setJoyKeycode:fingerprint keymap:2 direction:(JoystickDirection)recordKey2];
        [[controller screen] setJoyChar:c keymap:2 direction:(JoystickDirection)recordKey2];
    }

    recordKey1 = -1;
    recordKey2 = -1;
    [self update];
    return;
}

- (void)flagsChanged:(NSEvent *)event
{
    unsigned int flags = [event modifierFlags];
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
        [[controller screen] setJoyKeycode:flags keymap:1 direction:(JoystickDirection)recordKey1];
        [[controller screen] setJoyChar:' ' keymap:1 direction:(JoystickDirection)recordKey1];
    }
    
    // Second keyset
    if (recordKey2 != -1) {
        [[controller screen] setJoyKeycode:flags keymap:2 direction:(JoystickDirection)recordKey2];
        [[controller screen] setJoyChar:' ' keymap:2 direction:(JoystickDirection)recordKey2];
    }
    
    recordKey1 = -1;
    recordKey2 = -1;
    [self update];
}

@end
