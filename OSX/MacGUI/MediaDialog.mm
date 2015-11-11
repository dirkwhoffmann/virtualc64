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

@implementation MediaDialog

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

// VC 1541

- (IBAction)diskEjectAction:(id)sender
{
    NSLog(@"diskEjectAction");
    
    [controller driveEjectAction:sender];
    [self update];
}

- (IBAction)diskWriteProtectAction:(id)sender
{
    NSLog(@"diskWriteProtectAction");
    
    [[c64 vc1541] setWriteProtection:[(NSButton *)sender state]];
    [self update];
}

// VC 1530

- (IBAction)tapeEjectAction:(id)sender
{
    NSLog(@"tapeEjectAction");
    [[c64 datasette] ejectTape];

}

- (IBAction)tapePlayAction:(id)sender
{
    NSLog(@"tapePlayAction");
    [[c64 datasette] pressPlay];
}

- (IBAction)tapeStopAction:(id)sender
{
    NSLog(@"tapeStopAction");
    [[c64 datasette] pressStop];


}

- (IBAction)tapeRewindAction:(id)sender
{
    NSLog(@"tapeRewindAction");
    [[c64 datasette] pressRewind];
}

- (IBAction)tapeHeadAction:(id)sender
{
    NSLog(@"tapeHeadAction");
    
    int value = [sender intValue];
    NSLog(@"value = %d", value);
    
    int seconds = (value * [[c64 datasette] duration]) / 100;
    NSLog(@"tapeHeadAction: percentage = %d, seconds = %d\n", value, seconds);
    [[c64 datasette] setHeadPositionInSeconds:seconds];
}

// Expansion port

- (IBAction)cartridgeEjectAction:(id)sender
{
    NSLog(@"cartridgeEjectAction");
    
    [c64 detachCartridge];
    [[controller document] setCartridge:NULL];
    [c64 reset];
    [self update];
}

- (void)update
{
    /* VC1541 */
    
    if ([[c64 vc1541] hasDisk]) {
        [diskIcon setHidden:NO];
        [diskText setStringValue:archiveName];
        [diskEject setEnabled:YES];
        [diskEjectText setEnabled:YES];
        [diskWriteProtected setEnabled:[[c64 vc1541] bitAccuracy]];
        [diskWriteProtected setState:[[c64 vc1541] writeProtection]];
    } else {
        [diskIcon setHidden:YES];
        [diskText setStringValue:@"No disk"];
        [diskEject setEnabled:NO];
        [diskEjectText setEnabled:NO];
        [diskWriteProtected setEnabled:NO];
    }

    /* VC1530 */
    DatasetteProxy *datasette = [c64 datasette];
    if ([datasette hasTape]) {
        [tapeIcon setHidden:NO];
        // [tapeText setStringValue:archiveName];
        [tapePlay setEnabled:YES];
        [tapeRewind setEnabled:YES];
        [tapeStop setEnabled:YES];
        [tapeEject setEnabled:YES];
        [tapeEjectText setEnabled:YES];
        [tapeHead setStringValue:[NSString stringWithFormat:@"%02d:%02d",
                                  [datasette headPositionInSeconds] / 60,
                                  [datasette headPositionInSeconds] % 60]];
        [tapeEnd  setStringValue:[NSString stringWithFormat:@"%02d:%02d",
                                  [datasette duration] / 60,
                                  [datasette duration] % 60]];
    } else {
        [tapeIcon setHidden:YES];
        [tapeText setStringValue:@"No tape"];
        [tapePlay setEnabled:NO];
        [tapeRewind setEnabled:NO];
        [tapeStop setEnabled:NO];
        [tapeEject setEnabled:NO];
        [tapeEjectText setEnabled:NO];
        [tapeHead setStringValue:[NSString stringWithFormat:@"%02d:%02d", 0, 0]];
        [tapeEnd  setStringValue:[NSString stringWithFormat:@"%02d:%02d", 0, 0]];
    }
    int elapsedTime = [[c64 datasette] headPositionInSeconds];
    // NSLog(@"ElapsedTime: %d\n", elapsedTime);
    int totalTime = [[c64 datasette] duration];
    // NSLog(@"TotalTime: %d\n", totalTime);
    int sliderPosition = (totalTime == 0) ? 0 : (elapsedTime * 100 / totalTime);
    // NSLog(@"sliderPosition: %d\n", sliderPosition);
    [tapeSlider setIntValue:sliderPosition];
    
    /* Expansion port */
    
    if ([c64 isCartridgeAttached]) {
        [cartridgeIcon setHidden:NO];
        [cartridgeText setStringValue:[NSString stringWithFormat:@"Type %d cartridge",
                                       [[c64 expansionport] cartridgeType]]];
        [cartridgeEject setEnabled:YES];
        [cartridgeEject setHidden:NO];
        [cartridgeEjectText setEnabled:YES];
    } else {
        [cartridgeIcon setHidden:YES];
        [cartridgeText setStringValue:@"No cartridge"];
        [cartridgeEject setEnabled:NO];
        [cartridgeEjectText setEnabled:NO];
    }
}

@end
