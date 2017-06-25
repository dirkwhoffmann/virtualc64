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

#import <Cocoa/Cocoa.h>

/* System */
extern NSString *VC64BasicRomFileKey;
extern NSString *VC64CharRomFileKey;
extern NSString *VC64KernelRomFileKey;
extern NSString *VC64VC1541RomFileKey;

/* Joystick */
extern NSString *VC64Left1keycodeKey;
extern NSString *VC64Left1charKey;
extern NSString *VC64Right1keycodeKey;
extern NSString *VC64Right1charKey;
extern NSString *VC64Up1keycodeKey;
extern NSString *VC64Up1charKey;
extern NSString *VC64Down1keycodeKey;
extern NSString *VC64Down1charKey;
extern NSString *VC64Fire1keycodeKey;
extern NSString *VC64Fire1charKey;
extern NSString *VC64Left2keycodeKey;
extern NSString *VC64Left2charKey;
extern NSString *VC64Right2keycodeKey;
extern NSString *VC64Right2charKey;
extern NSString *VC64Up2keycodeKey;
extern NSString *VC64Up2charKey;
extern NSString *VC64Down2keycodeKey;
extern NSString *VC64Down2charKey;
extern NSString *VC64Fire2keycodeKey;
extern NSString *VC64Fire2charKey;

/* Video */
extern NSString *VC64EyeX;
extern NSString *VC64EyeY;
extern NSString *VC64EyeZ;
extern NSString *VC64ColorSchemeKey;
extern NSString *VC64VideoFilterKey;
extern NSString *VC64FullscreenKeepAspectRatioKey;

@interface PropertiesDialog : NSWindow
{
    C64Proxy *c64;
    MyController *controller;
    
    //! Indicates if a keycode should be recorded for keyset 1
    /*! Per default, this value equals -1 (nothing should be recorded) */
    long recordKey1;

    //! Indicates if a keycode should be recorded for keyset 2
    /*! Per default, this value equals -1 (nothing should be recorded) */
    long recordKey2;
		
    /* Joystick */
    IBOutlet NSTextField *left1;
    IBOutlet NSButton *left1button;
    IBOutlet NSTextField *right1;
    IBOutlet NSButton *right1button;
    IBOutlet NSTextField *up1;
    IBOutlet NSButton *up1button;
    IBOutlet NSTextField *down1;
    IBOutlet NSButton *down1button;
    IBOutlet NSTextField *fire1;
    IBOutlet NSButton *fire1button;
    IBOutlet NSTextField *left2;
    IBOutlet NSButton *left2button;
    IBOutlet NSTextField *right2;
    IBOutlet NSButton *right2button;
    IBOutlet NSTextField *up2;
    IBOutlet NSButton *up2button;
    IBOutlet NSTextField *down2;
    IBOutlet NSButton *down2button;
    IBOutlet NSTextField *fire2;
    IBOutlet NSButton *fire2button;
    
	/* Video */
	int customColor[16];
	IBOutlet NSSlider *eyeXSlider;
	IBOutlet NSSlider *eyeYSlider;
	IBOutlet NSSlider *eyeZSlider;
	IBOutlet NSPopUpButton *colorScheme;
	IBOutlet NSPopUpButton *videoFilter;
	IBOutlet NSColorWell *colorWell0;
	IBOutlet NSColorWell *colorWell1;
	IBOutlet NSColorWell *colorWell2;
	IBOutlet NSColorWell *colorWell3;
	IBOutlet NSColorWell *colorWell4;
	IBOutlet NSColorWell *colorWell5;
	IBOutlet NSColorWell *colorWell6;
	IBOutlet NSColorWell *colorWell7;
	IBOutlet NSColorWell *colorWell8;
	IBOutlet NSColorWell *colorWell9;
	IBOutlet NSColorWell *colorWell10;
	IBOutlet NSColorWell *colorWell11;
	IBOutlet NSColorWell *colorWell12;
	IBOutlet NSColorWell *colorWell13;
	IBOutlet NSColorWell *colorWell14;
	IBOutlet NSColorWell *colorWell15;
    IBOutlet NSButton *aspectRatioButton;
}

- (void) initialize:(MyController *)mycontroller;

// Update methods

//! Helper function for update
- (NSString *)keycodeInPlainText:(int)code character:(char)c;
- (void)updateKeymap:(int)map direction:(GamePadDirection)dir button:(NSButton *)b text:(NSTextField *)t;
- (void) update;

// Joystick
- (IBAction)recordKeyAction:(id)sender;

// Video
- (IBAction)changeColorScheme:(id)sender;
- (IBAction)setVideoFilterAction:(id)sender;
- (IBAction)setEyeXAction:(id)sender;
- (IBAction)setEyeYAction:(id)sender;
- (IBAction)setEyeZAction:(id)sender;
- (IBAction)setFullscreenAspectRatio:(id)sender;

// Action buttons
- (IBAction)useAsDefaultAction:(id)sender;
- (IBAction)factorySettingsAction:(id)sender;

@end
