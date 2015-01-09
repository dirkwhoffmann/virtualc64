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

/* System */
NSString *VC64PALorNTSCKey    = @"VC64PALorNTSCKey";
NSString *VC64BasicRomFileKey = @"VC64BasicRomFileKey";
NSString *VC64CharRomFileKey  = @"VC64CharRomFileKey";
NSString *VC64KernelRomFileKey= @"VC64KernelRomFileKey";
NSString *VC64VC1541RomFileKey= @"VC64VC1541RomFileKey";

/* Peripherals */
NSString *VC64WarpLoadKey     = @"VC64WarpLoadKey";

/* Audio */
NSString *VC64SIDFilterKey    = @"VC64SIDFilterKey";
NSString *VC64SIDReSIDKey     = @"VC64SIDReSIDKey";
NSString *VC64SIDChipModelKey = @"VC64SIDChipModelKey";
NSString *VC64SIDSamplingMethodKey = @"VC64SIDSamplingMethodKey";

/* Video */
NSString *VC64EyeX            = @"VC64EyeX";
NSString *VC64EyeY            = @"VC64EyeY";
NSString *VC64EyeZ            = @"VC64EyeZ";
NSString *VC64ColorSchemeKey  = @"VC64ColorSchemeKey";
NSString *VC64VideoFilterKey  = @"VC64VideoFilterKey";

- (void)initialize:(MyController *)mycontroller
{
    controller = mycontroller;
    c64 = [controller c64];
	[self update];
}

- (IBAction)useAsDefaultAction:(id)sender
{
    [controller saveUserDefaults];
}

- (IBAction)factorySettingsAction:(id)sender
{
    NSLog(@"Restoring factoring settings");
    
    
	// System
    [self setPalAction:self];
    
	// Peripherals
	[c64 setWarpLoad:true];
	
	// Audio
	[c64 setReSID:YES];
    [c64 setAudioFilter:NO];
    [c64 setChipModel:1];
    [c64 setSamplingMethod:0];
    
	// Video     
    [c64 setColorScheme:VIC::CCS64];
    
    [self update];
    [self useAsDefaultAction:self];
}

- (IBAction)setPalAction:(id)sender
{
	[c64 setPAL];
    [[controller screen] setPAL];
	[self update];	
}

- (IBAction)setNtscAction:(id)sender
{
	[c64 setNTSC];
    [[controller screen] setNTSC];
	[self update];	
}

- (IBAction)warpLoadAction:(id)sender
{
	[c64 setWarpLoad:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)SIDFilterAction:(id)sender
{
	if ([(NSButton *)sender state]) {
		[c64 setAudioFilter:true];
	} else {
		[c64 setAudioFilter:false];
	}
}

- (IBAction)SIDReSIDAction:(id)sender
{
	if ([(NSButton *)sender state]) {
		[c64 setReSID:true];
	} else {
		[c64 setReSID:false];
	}
}

- (IBAction)SIDSamplingMethodAction:(id)sender
{
    int value = [[sender selectedItem] tag];
    [c64 setSamplingMethod:value];
}

- (IBAction)SIDChipModelAction:(id)sender
{
    int value = [[sender selectedItem] tag];
    [c64 setChipModel:value];
}

- (IBAction)changeColorScheme:(id)sender
{
	VIC::ColorScheme scheme = (VIC::ColorScheme)[[sender selectedItem] tag];
    [c64 setColorScheme:scheme];
	[self update];    
}

- (IBAction)setVideoFilterAction:(id)sender
{	
	[[controller screen] setAntiAliasing:[[videoFilter selectedItem] tag]];
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

- (void) update
{	
	/* System */
	if ([c64 isPAL]) {
        [machineType selectItemWithTag:0];
		[systemText1 setStringValue:@"PAL machine"];
		[systemText2 setStringValue:@"0.985 MHz"];
		[systemText3 setStringValue:@"63 cycles per rasterline"];
	} else {
        [machineType selectItemWithTag:1];
		[systemText1 setStringValue:@"NTSC machine"];
		[systemText2 setStringValue:@"1.022 MHz"];
		[systemText3 setStringValue:@"65 cycles per rasterline"];
	}
         
	/* Peripherals */
	[warpLoad setState:[c64 warpLoad]];
	
	/* Audio */
    [SIDUseReSID setState:[c64 reSID]];
    [SIDFilter setState:[c64 audioFilter]];
    [SIDChipModel selectItemWithTag:[c64 chipModel]];
    [SIDSamplingMethod selectItemWithTag:[c64 samplingMethod]];

	/* Video */
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

@end
