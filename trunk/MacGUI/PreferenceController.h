/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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
#import "C64Proxy.h"

/* System */
extern NSString *VC64PALorNTSCKey;
extern NSString *VC64BasicRomFileKey;
extern NSString *VC64CharRomFileKey;
extern NSString *VC64KernelRomFileKey;
extern NSString *VC64VC1541RomFileKey;

/* Peripherals */
extern NSString *VC64WarpLoadKey;

/* Audio */
extern NSString *VC64SIDFilterKey;

/* Video */
extern NSString *VC64ColorSchemeKey;
extern NSString *VC64VideoFilterKey;
extern NSString *VC64CustomCol0Key;
extern NSString *VC64CustomCol1Key;
extern NSString *VC64CustomCol2Key;
extern NSString *VC64CustomCol3Key;
extern NSString *VC64CustomCol4Key;
extern NSString *VC64CustomCol5Key;
extern NSString *VC64CustomCol6Key;
extern NSString *VC64CustomCol7Key;
extern NSString *VC64CustomCol8Key;
extern NSString *VC64CustomCol9Key;
extern NSString *VC64CustomCol10Key;
extern NSString *VC64CustomCol11Key;
extern NSString *VC64CustomCol12Key;
extern NSString *VC64CustomCol13Key;
extern NSString *VC64CustomCol14Key;
extern NSString *VC64CustomCol15Key;

@interface PreferenceController : NSWindowController {

	C64Proxy *c64;
	MyDocument *mydoc;
	
	/* System */
	IBOutlet NSButtonCell *pal;
	IBOutlet NSButtonCell *ntsc;
	IBOutlet NSMatrix *palntsc;
	IBOutlet NSButton *flag;
	IBOutlet NSTextField *systemText1;
	IBOutlet NSTextField *systemText2;
	IBOutlet NSTextField *systemText3;
	
	/* Peripherals */
	IBOutlet NSButton *warpLoad;
	
	/* Audio */
	IBOutlet NSButton *SIDFilter;
	
	/* Video */
	int customColor[16];
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
}

- (void)setC64:(C64Proxy *)proxy;
- (void)setDoc:(MyDocument *)doc;
- (void)updateColorWell:(NSColorWell *)well color:(int)rgba;
- (void)updateColorWells:(VIC::ColorScheme)scheme;

// System
- (IBAction)setPalAction:(id)sender;
- (IBAction)setNtscAction:(id)sender;
- (IBAction)togglePalNtscAction:(id)sender;

// VC 1541
- (IBAction)warpLoadAction:(id)sender;

// SID
- (IBAction)sidFilterAction:(id)sender;

// VIC
- (IBAction)changeColorScheme:(id)sender;
- (IBAction)setVideoFilterAction:(id)sender;
- (IBAction)setColorAction:(id)sender;

@end
