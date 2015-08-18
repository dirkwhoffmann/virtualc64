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

/* System */
NSString *VC64PALorNTSCKey    = @"VC64PALorNTSCKey";

/* Audio */
NSString *VC64SIDFilterKey    = @"VC64SIDFilterKey";
NSString *VC64SIDReSIDKey     = @"VC64SIDReSIDKey";
NSString *VC64SIDChipModelKey = @"VC64SIDChipModelKey";
NSString *VC64SIDSamplingMethodKey = @"VC64SIDSamplingMethodKey";

- (void)initialize:(MyController *)mycontroller archiveName:(NSString *)name noOfFiles:(unsigned)files
{
    controller = mycontroller;
    c64 = [controller c64];
    archiveName = name;
    noOfFiles = files;
    
    // Do some consistency checking...
    if ([[c64 vc1541] hasDisk] && name == NULL) {
        NSLog(@"WARNING: VC1541 has disk, but no disk name is provided.");
        assert(0);
        archiveName = @"";
    }
    
    [self update];
}

- (IBAction)useAsDefaultAction:(id)sender
{
    // TODO: DON'T SAVE ALL SETTINGS
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

    [self update];
    [self useAsDefaultAction:self];
}

- (IBAction)setPalAction:(id)sender
{
    NSLog(@"setPalAction");
    
    [c64 setPAL];
    [[controller screen] setPAL];
    [self update];
}

- (IBAction)setNtscAction:(id)sender
{
    NSLog(@"setNtscAction");
    
    [c64 setNTSC];
    [[controller screen] setNTSC];
    [self update];	
}

- (IBAction)warpLoadAction:(id)sender
{
    NSLog(@"warpLoadAction");
    
    [c64 setWarpLoad:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)writeProtectionAction:(id)sender
{
    NSLog(@"writeProtectionAction");
    
    [[c64 vc1541] setWriteProtection:[(NSButton *)sender state]];
    [self update];
}

- (IBAction)ejectDiskAction:(id)sender
{
    NSLog(@"ejectDiskAction");

    [[c64 vc1541] ejectDisk];
    [self update];
}

- (IBAction)ejectCartridgeAction:(id)sender
{
    NSLog(@"ejectCartridgeAction");

    [c64 detachCartridge];
    [[controller document] setCartridge:NULL];
    [c64 reset];
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
}

- (IBAction)SIDReSIDAction:(id)sender
{
    NSLog(@"SIDReSIDAction");

    if ([(NSButton *)sender state]) {
        [c64 setReSID:true];
    } else {
        [c64 setReSID:false];
    }
}

- (IBAction)SIDSamplingMethodAction:(id)sender
{
    NSLog(@"SIDSamplingMethodAction");
    
    int value = [[sender selectedItem] tag];
    [c64 setSamplingMethod:value];
}

- (IBAction)SIDChipModelAction:(id)sender
{
    NSLog(@"SIDChipModelAction");
    
    int value = [[sender selectedItem] tag];
    [c64 setChipModel:value];
}

- (void)update
{
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
    // [warpText setHidden:![c64 warpLoad]];
    
    if ([[c64 vc1541] hasDisk]) {
        
        [discIcon setHidden:NO];
        [diskText setStringValue:archiveName];
        [diskText2 setStringValue:[NSString stringWithFormat:@"%d file%s",
                                   noOfFiles, noOfFiles == 1 ? "" : "s"]];
        [diskEject setEnabled:YES];
        [diskEject setHidden:NO];
        [diskEjectText setEnabled:YES];
        [diskEjectText setHidden:NO];
        [writeProtected setHidden:NO];
        [writeProtected setState:[[c64 vc1541] writeProtection]];
        
    } else {
        
        [discIcon setHidden:YES];
        [diskText setStringValue:@"No disk inserted"];
        [diskText2 setStringValue:@""]; // TODO: Number of files
        [diskEject setEnabled:NO];
        [diskEject setHidden:YES];
        [diskEjectText setEnabled:NO];
        [diskEjectText setHidden:YES];
        [writeProtected setHidden:YES];
    }
    
    /* Expansion port */
    if ([c64 isCartridgeAttached]) { // TODO: Move to ExpansionPortProxy
        
        [cartridgeIcon setHidden:NO];
        [cartridgeText setStringValue:[NSString stringWithFormat:@"Type %d cartridge",
                                       [[c64 expansionport] cartridgeType]]];
        [cartridgeText2 setStringValue:[NSString stringWithFormat:@"%d chip%s (%d KB total)",
                                        [[c64 expansionport] numberOfChips],
                                        [[c64 expansionport] numberOfChips] == 1 ? "" : "s",
                                        [[c64 expansionport] numberOfBytes]]];
        
    } else {
        
        [cartridgeIcon setHidden:YES];
        [cartridgeText setStringValue:@"No cartridge attached"];
        [cartridgeText2 setStringValue:@""];
        [cartridgeEject setEnabled:NO];
        [cartridgeEject setHidden:YES];
        [cartridgeEjectText setEnabled:NO];
        [cartridgeEjectText setHidden:YES];
    }
    
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

