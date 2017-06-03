/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
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

@implementation HardwareDialog

// System
NSString *VC64PALorNTSCKey = @"VC64PALorNTSCKey";

// Peripherals
NSString *VC64WarpLoadKey = @"VC64WarpLoadKey";
NSString *VC64DriveNoiseKey = @"VC64DriveNoiseKey";
NSString *VC64BitAccuracyKey = @"VC64BitAccuracyKey";

// Audio
NSString *VC64SIDFilterKey    = @"VC64SIDFilterKey";
NSString *VC64SIDReSIDKey     = @"VC64SIDReSIDKey";
NSString *VC64SIDChipModelKey = @"VC64SIDChipModelKey";
NSString *VC64SIDSamplingMethodKey = @"VC64SIDSamplingMethodKey";

- (void)initialize:(MyController *)mycontroller
{
    controller = mycontroller;
    c64 = [controller c64];
    [self update];
}

- (IBAction)useAsDefaultAction:(id)sender
{
    NSLog(@"Saving emulator user defaults");
    [controller saveVirtualMachineUserDefaults];
}

- (IBAction)factorySettingsAction:(id)sender
{
    NSLog(@"Restoring virtual machine factoring settings");
    
    // System
    [self setPalAction:self];
    
    // Peripherals
    [c64 setWarpLoad:true];
    [[c64 vc1541] setSendSoundMessages:true];
    [[c64 vc1541] setBitAccuracy:true];

    // Audio
    [c64 setReSID:YES];
    [c64 setChipModel:0];
    [c64 setSamplingMethod:0];
    [c64 setAudioFilter:NO];

    [self update];
    [self useAsDefaultAction:self];
}

- (IBAction)setPalAction:(id)sender
{
    NSLog(@"setPalAction");
    
    [c64 setPAL];
    [self update];
}

- (IBAction)setNtscAction:(id)sender
{
    NSLog(@"setNtscAction");
    
    [c64 setNTSC];
    [self update];	
}

- (IBAction)warpLoadAction:(id)sender
{
    NSLog(@"warpLoadAction");
    
    [c64 setWarpLoad:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)driveNoiseAction:(id)sender
{
    NSLog(@"driveNoiseAction");
    
    [[c64 vc1541] setSendSoundMessages:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)bitAccurateAction:(id)sender
{
    NSLog(@"bitAccurateAction");
    
    [[c64 vc1541] setBitAccuracy:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)SIDFilterAction:(id)sender
{
    NSLog(@"SIDFilterAction");

    if ([(NSButton *)sender state]) {
        [c64 setAudioFilter:true];
    } else {
        [c64 setAudioFilter:false];
    }
    [self update];
}

- (IBAction)SIDReSIDAction:(id)sender
{
    NSLog(@"SIDReSIDAction");

    if ([(NSButton *)sender state]) {
        [c64 setReSID:true];
    } else {
        [c64 setReSID:false];
    }
    [self update];
}

- (IBAction)SIDSamplingMethodAction:(id)sender
{
    NSLog(@"SIDSamplingMethodAction");
    
    long value = [[sender selectedItem] tag];
    [c64 setSamplingMethod:value];
    [self update];
}

- (IBAction)SIDChipModelAction:(id)sender
{
    NSLog(@"SIDChipModelAction");
    
    long value = [[sender selectedItem] tag];
    [c64 setChipModel:value];
    [self update];
}

- (void)update
{
    // NSLog(@"update");
    
    /* System */
    if ([c64 isPAL]) {
        [machineType selectItemWithTag:0];
        [flag setImage:[NSImage imageNamed:@"flag_eu"]];
        [systemText setStringValue:@"PAL machine"];
        [systemText2 setStringValue:@"63 cycles per rasterline"];
    } else {
        [machineType selectItemWithTag:1];
        [flag setImage:[NSImage imageNamed:@"flag_usa"]];
        [systemText setStringValue:@"NTSC machine"];
        [systemText2 setStringValue:@"65 cycles per rasterline"];
    }
    
    /* VC1541 */
    [warpLoad setState:[c64 warpLoad]];
    [driveNoise setState:[[c64 vc1541] soundMessagesEnabled]];
    [bitAccurate setState:[[c64 vc1541] bitAccuracy]];
        
    /* Audio */
    [SIDUseReSID setState:[c64 reSID]];
    [SIDFilter setState:[c64 audioFilter]];
    [SIDFilter setEnabled:[c64 reSID]];
    [SIDChipModel selectItemWithTag:[c64 chipModel]];
    [SIDChipModel setEnabled:[c64 reSID]];
    [SIDSamplingMethod selectItemWithTag:[c64 samplingMethod]];
    [SIDSamplingMethod setEnabled:[c64 reSID]];
}

@end

