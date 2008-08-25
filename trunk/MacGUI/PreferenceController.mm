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

#include "C64.h"
#import "MyDocument.h"
#import "PreferenceController.h"

/* System */
NSString *VC64PALorNTSCKey    = @"VC64PALorNTSCKey";
NSString *VC64IllegalInstrKey = @"VC64IllegalInstrKey";
NSString *VC64FastResetKey    = @"VC64FastResetKey";
NSString *VC64BasicRomFileKey = @"VC64BasicRomFileKey";
NSString *VC64CharRomFileKey  = @"VC64CharRomFileKey";
NSString *VC64KernelRomFileKey= @"VC64KernelRomFileKey";
NSString *VC64VC1541RomFileKey= @"VC64VC1541RomFileKey";

/* Peripherals */
NSString *VC64Real1541Key     = @"VC64Real1541Key";
NSString *VC64WarpLoadKey     = @"VC64WarpLoadKey";

/* Audio */
NSString *VC64SIDVolumeKey    = @"VC64SIDVolumeKey";
NSString *VC64SIDFilterKey    = @"VC64SIDFilterKey";

/* Video */
NSString *VC64ColorSchemeKey  = @"VC64ColorSchemeKey";
NSString *VC64VideoFilterKey  = @"VC64VideoFilterKey";
NSString *VC64CustomCol0Key   = @"VC64CustomCol0Key";
NSString *VC64CustomCol1Key   = @"VC64CustomCol1Key";
NSString *VC64CustomCol2Key   = @"VC64CustomCol2Key";
NSString *VC64CustomCol3Key   = @"VC64CustomCol3Key";
NSString *VC64CustomCol4Key   = @"VC64CustomCol4Key";
NSString *VC64CustomCol5Key   = @"VC64CustomCol5Key";
NSString *VC64CustomCol6Key   = @"VC64CustomCol6Key";
NSString *VC64CustomCol7Key   = @"VC64CustomCol7Key";
NSString *VC64CustomCol8Key   = @"VC64CustomCol8Key";
NSString *VC64CustomCol9Key   = @"VC64CustomCol9Key";
NSString *VC64CustomCol10Key  = @"VC64CustomCol10Key";
NSString *VC64CustomCol11Key  = @"VC64CustomCol11Key";
NSString *VC64CustomCol12Key  = @"VC64CustomCol12Key";
NSString *VC64CustomCol13Key  = @"VC64CustomCol13Key";
NSString *VC64CustomCol14Key  = @"VC64CustomCol14Key";
NSString *VC64CustomCol15Key  = @"VC64CustomCol15Key";

@implementation PreferenceController

- (id)init
{
	self = [super initWithWindowNibName:@"Preferences"];
	return self;
}

- (void)setC64:(C64Proxy *)proxy
{
	c64 = proxy;
}

- (void)setDoc:(MyDocument *)doc
{
	mydoc = doc;
}

- (void)windowDidLoad
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	int colorSchemeTag;
	
	/* System */
	if ([defaults integerForKey:VC64PALorNTSCKey])
		[self setNtscAction:self];
	else
		[self setPalAction:self];
		
	/* Peripherals */
	[warpLoad setState:[defaults boolForKey:VC64WarpLoadKey]];
	
	/* Audio */
	[SIDFilter setState:[defaults boolForKey:VC64SIDFilterKey]];
	
	/* Video */
	colorSchemeTag = [defaults integerForKey:VC64ColorSchemeKey];
	[colorScheme selectItemWithTag:colorSchemeTag];
	[videoFilter selectItemWithTag:[defaults boolForKey:VC64VideoFilterKey]];
	customColor[0] = [defaults integerForKey:VC64CustomCol0Key];
	customColor[1] = [defaults integerForKey:VC64CustomCol1Key];
	customColor[2] = [defaults integerForKey:VC64CustomCol2Key];
	customColor[3] = [defaults integerForKey:VC64CustomCol3Key];
	customColor[4] = [defaults integerForKey:VC64CustomCol4Key];
	customColor[5] = [defaults integerForKey:VC64CustomCol5Key];
	customColor[6] = [defaults integerForKey:VC64CustomCol6Key];
	customColor[7] = [defaults integerForKey:VC64CustomCol7Key];
	customColor[8] = [defaults integerForKey:VC64CustomCol8Key];
	customColor[9] = [defaults integerForKey:VC64CustomCol9Key];
	customColor[10] = [defaults integerForKey:VC64CustomCol10Key];
	customColor[11] = [defaults integerForKey:VC64CustomCol11Key];
	customColor[12] = [defaults integerForKey:VC64CustomCol12Key];
	customColor[13] = [defaults integerForKey:VC64CustomCol13Key];
	customColor[14] = [defaults integerForKey:VC64CustomCol14Key];
	customColor[15] = [defaults integerForKey:VC64CustomCol15Key];	
	[self updateColorWells:(VIC::ColorScheme)colorSchemeTag];
}

- (void)updateColorWell:(NSColorWell *)well color:(int)rgba
{
	float r = (float)((rgba >> 24) & 0xff) / 255.0;
	float g = (float)((rgba >> 16) & 0xff) / 255.0;
	float b = (float)((rgba >> 8)  & 0xff) / 255.0;
	float a = (float)(rgba         & 0xff) / 255.0;
	NSColor *col = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
	[well setColor:col];
}

- (void)updateColorWells:(VIC::ColorScheme)scheme
{
	bool enabled;
	
	if (scheme == VIC::CUSTOM_PALETTE) {
		[self updateColorWell:colorWell0 color:customColor[0]];
		[self updateColorWell:colorWell1 color:customColor[1]];
		[self updateColorWell:colorWell2 color:customColor[2]];
		[self updateColorWell:colorWell3 color:customColor[3]];
		[self updateColorWell:colorWell4 color:customColor[4]];
		[self updateColorWell:colorWell5 color:customColor[5]];
		[self updateColorWell:colorWell6 color:customColor[6]];
		[self updateColorWell:colorWell7 color:customColor[7]];
		[self updateColorWell:colorWell8 color:customColor[8]];
		[self updateColorWell:colorWell9 color:customColor[9]];
		[self updateColorWell:colorWell10 color:customColor[10]];
		[self updateColorWell:colorWell11 color:customColor[11]];
		[self updateColorWell:colorWell12 color:customColor[12]];
		[self updateColorWell:colorWell13 color:customColor[13]];
		[self updateColorWell:colorWell14 color:customColor[14]];
		[self updateColorWell:colorWell15 color:customColor[15]];
		enabled = YES;	
	} else {
		[colorWell0 setColor:[c64 vicGetColor:scheme nr:0]];
		[colorWell1 setColor:[c64 vicGetColor:scheme nr:1]];
		[colorWell2 setColor:[c64 vicGetColor:scheme nr:2]];
		[colorWell3 setColor:[c64 vicGetColor:scheme nr:3]];
		[colorWell4 setColor:[c64 vicGetColor:scheme nr:4]];
		[colorWell5 setColor:[c64 vicGetColor:scheme nr:5]];
		[colorWell6 setColor:[c64 vicGetColor:scheme nr:6]];
		[colorWell7 setColor:[c64 vicGetColor:scheme nr:7]];
		[colorWell8 setColor:[c64 vicGetColor:scheme nr:8]];
		[colorWell9 setColor:[c64 vicGetColor:scheme nr:9]];
		[colorWell10 setColor:[c64 vicGetColor:scheme nr:10]];
		[colorWell11 setColor:[c64 vicGetColor:scheme nr:11]];
		[colorWell12 setColor:[c64 vicGetColor:scheme nr:12]];
		[colorWell13 setColor:[c64 vicGetColor:scheme nr:13]];
		[colorWell14 setColor:[c64 vicGetColor:scheme nr:14]];
		[colorWell15 setColor:[c64 vicGetColor:scheme nr:15]];
		enabled = NO;
	}
	[colorWell0 setEnabled:enabled];
	[colorWell1 setEnabled:enabled];
	[colorWell2 setEnabled:enabled];
	[colorWell3 setEnabled:enabled];
	[colorWell4 setEnabled:enabled];
	[colorWell5 setEnabled:enabled];
	[colorWell6 setEnabled:enabled];
	[colorWell7 setEnabled:enabled];
	[colorWell8 setEnabled:enabled];
	[colorWell9 setEnabled:enabled];
	[colorWell10 setEnabled:enabled];
	[colorWell11 setEnabled:enabled];
	[colorWell12 setEnabled:enabled];
	[colorWell13 setEnabled:enabled];
	[colorWell14 setEnabled:enabled];
	[colorWell15 setEnabled:enabled];
	
	[c64 vicSetColor:0 rgba:[colorWell0 color]];
	[c64 vicSetColor:1 rgba:[colorWell1 color]];
	[c64 vicSetColor:2 rgba:[colorWell2 color]];
	[c64 vicSetColor:3 rgba:[colorWell3 color]];
	[c64 vicSetColor:4 rgba:[colorWell4 color]];
	[c64 vicSetColor:5 rgba:[colorWell5 color]];
	[c64 vicSetColor:6 rgba:[colorWell6 color]];
	[c64 vicSetColor:7 rgba:[colorWell7 color]];
	[c64 vicSetColor:8 rgba:[colorWell8 color]];
	[c64 vicSetColor:9 rgba:[colorWell9 color]];
	[c64 vicSetColor:10 rgba:[colorWell10 color]];
	[c64 vicSetColor:11 rgba:[colorWell11 color]];
	[c64 vicSetColor:12 rgba:[colorWell12 color]];
	[c64 vicSetColor:13 rgba:[colorWell13 color]];
	[c64 vicSetColor:14 rgba:[colorWell14 color]];
	[c64 vicSetColor:15 rgba:[colorWell15 color]];
}

- (IBAction)setPalAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	[palntsc selectCellWithTag:0];
	[systemText1 setStringValue:@"PAL machine"];
	[systemText2 setStringValue:@"0.985 MHz"];
	[systemText3 setStringValue:@"63 cycles per rasterline"];
	[flag setState:false];
	[c64 setPAL];
	[defaults setInteger:[[palntsc selectedCell] tag] forKey:VC64PALorNTSCKey];
}

- (IBAction)setNtscAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	[palntsc selectCellWithTag:1];
	[systemText1 setStringValue:@"NTSC machine"];
	[systemText2 setStringValue:@"1.022 MHz"];
	[systemText3 setStringValue:@"65 cycles per rasterline"];
	[flag setState:true];
	[c64 setNTSC];
	[defaults setInteger:[[palntsc selectedCell] tag] forKey:VC64PALorNTSCKey];
}

- (IBAction)togglePalNtscAction:(id)sender
{
	if ([sender state]) {
		[self setNtscAction:self];
	} else {
		[self setPalAction:self];
	}
}

- (IBAction)warpLoadAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	[mydoc setWarpLoad:[sender state]]; 
	[defaults setBool:[warpLoad state] forKey:VC64WarpLoadKey];
}

- (IBAction)sidFilterAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	if ([sender state]) {
		// [c64 setSIDFilter:true];
	} else {
		// [c64 setSIDFilter:false];
	}
	[defaults setBool:[SIDFilter state] forKey:VC64SIDFilterKey];
}

- (IBAction)changeColorScheme:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	VIC::ColorScheme scheme = (VIC::ColorScheme)[[sender selectedItem] tag];
	[self updateColorWells:scheme];

	[defaults setInteger:[[colorScheme selectedItem] tag] forKey:VC64ColorSchemeKey];
}

- (IBAction)setVideoFilterAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

	// set filter...
	// not implemented yet
	
	[defaults setInteger:[[videoFilter selectedItem] tag] forKey:VC64VideoFilterKey];
}

- (IBAction)setColorAction:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSColor *selectedColor;
	NSString *key;
	
	float r,g,b,a;
	
	int col = 0;
	if (sender == colorWell0) { col = 0; key = VC64CustomCol0Key; }
	if (sender == colorWell1) { col = 1; key = VC64CustomCol1Key; }
	if (sender == colorWell2) { col = 2; key = VC64CustomCol2Key; }
	if (sender == colorWell3) { col = 3; key = VC64CustomCol3Key; }
	if (sender == colorWell4) { col = 4; key = VC64CustomCol4Key; }
	if (sender == colorWell5) { col = 5; key = VC64CustomCol5Key; }
	if (sender == colorWell6) { col = 6; key = VC64CustomCol6Key; }
	if (sender == colorWell7) { col = 7; key = VC64CustomCol7Key; }
	if (sender == colorWell8) { col = 8; key = VC64CustomCol8Key; }
	if (sender == colorWell9) { col = 9; key = VC64CustomCol9Key; }
	if (sender == colorWell10) { col = 10; key = VC64CustomCol10Key; }
	if (sender == colorWell11) { col = 11; key = VC64CustomCol11Key; }	
	if (sender == colorWell12) { col = 12; key = VC64CustomCol12Key; }
	if (sender == colorWell13) { col = 13; key = VC64CustomCol13Key; }
	if (sender == colorWell14) { col = 14; key = VC64CustomCol14Key; }
	if (sender == colorWell15) { col = 15; key = VC64CustomCol15Key; }
	
	// [self writeColor:[sender color] forKey:key];

	selectedColor = [sender color];
	[selectedColor getRed:&r green:&g blue:&b alpha:&a];
	customColor[col] = (uint8_t)(r * 0xff);
	customColor[col] = (customColor[col] << 8) | (uint8_t)(g * 0xff);
	customColor[col] = (customColor[col] << 8) | (uint8_t)(b * 0xff);
	customColor[col] = (customColor[col] << 8) | (uint8_t)(a * 0xff);	
	[c64 vicSetColor:col rgba:[sender color]];
	[defaults setInteger:customColor[col] forKey:key];

#if 0
	[defaults setInteger:customColor[1] forKey:VC64CustomCol1Key];
	[defaults setInteger:customColor[2] forKey:VC64CustomCol2Key];
	[defaults setInteger:customColor[3] forKey:VC64CustomCol3Key];
	[defaults setInteger:customColor[4] forKey:VC64CustomCol4Key];
	[defaults setInteger:customColor[5] forKey:VC64CustomCol5Key];
	[defaults setInteger:customColor[6] forKey:VC64CustomCol6Key];
	[defaults setInteger:customColor[7] forKey:VC64CustomCol7Key];
	[defaults setInteger:customColor[8] forKey:VC64CustomCol8Key];
	[defaults setInteger:customColor[9] forKey:VC64CustomCol9Key];
	[defaults setInteger:customColor[10] forKey:VC64CustomCol10Key];
	[defaults setInteger:customColor[11] forKey:VC64CustomCol11Key];
	[defaults setInteger:customColor[12] forKey:VC64CustomCol12Key];
	[defaults setInteger:customColor[13] forKey:VC64CustomCol13Key];
	[defaults setInteger:customColor[14] forKey:VC64CustomCol14Key];
	[defaults setInteger:customColor[15] forKey:VC64CustomCol15Key];
#endif
}

@end
