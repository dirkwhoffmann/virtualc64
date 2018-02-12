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
#import "VirtualC64-Swift.h"

@implementation MediaDialog

- (void)initialize:(MyController *)mycontroller
{
    controller = mycontroller;
    c64 = [controller c64];
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

- (IBAction)tapeHeadAction:(id)sender
{
    NSLog(@"tapeHeadAction");
    
    long value = [sender intValue];
    NSLog(@"value = %ld", value);

    printf("durationInCycles = %ld\n", [[c64 datasette] durationInCycles]);

    long cycles = (value * [[c64 datasette] durationInCycles]) / 100;
    // NSLog(@"tapeHeadAction: percentage = %d, seconds = %d\n", value, seconds);
    printf("cycles = %ld\n", cycles);
    [[c64 datasette] setHeadInCycles:cycles];
}

// Expansion port

- (IBAction)cartridgeEjectAction:(id)sender
{
    NSLog(@"cartridgeEjectAction");
    // [[controller document] setAttachedCartridge:nil];
    [c64 detachCartridgeAndReset];
    [self update];
}

- (void)update
{
    if (!c64)
        return;
    
    /* VC1541 */
    
    if ([[c64 vc1541] hasDisk]) {
        [diskIcon setHidden:NO];
        [diskText setStringValue:[NSString stringWithFormat:@"%ld track disk",
                                  (long)[[[c64 vc1541] disk] numTracks]]];
        [diskWriteProtected setEnabled:[[c64 vc1541] bitAccuracy]];
        [diskWriteProtected setState:[[c64 vc1541] writeProtection]];
    } else {
        [diskIcon setHidden:YES];
        [diskText setStringValue:@"No disk"];
        [diskWriteProtected setEnabled:NO];
    }

    /* VC1530 */
    
    DatasetteProxy *datasette = [c64 datasette];
    if ([datasette hasTape]) {
        [tapeIcon setHidden:NO];
        [tapeText setStringValue:[NSString stringWithFormat:@"Type %ld tape",
                                  (long)[datasette getType]]];
        [tapeHead setStringValue:[NSString stringWithFormat:@"%02d:%02d",
                                  [datasette headInSeconds] / 60,
                                  [datasette headInSeconds] % 60]];
        [tapeEnd  setStringValue:[NSString stringWithFormat:@"%02d:%02d",
                                  [datasette durationInSeconds] / 60,
                                  [datasette durationInSeconds] % 60]];
        long elapsed = [[c64 datasette] headInCycles];
        long total = [[c64 datasette] durationInCycles];
        int sliderPosition = (int)(100 * elapsed / total);
        [tapeSlider setIntValue:sliderPosition];
    } else {
        [tapeIcon setHidden:YES];
        [tapeText setStringValue:@"No tape"];
        [tapeHead setStringValue:[NSString stringWithFormat:@"%02d:%02d", 0, 0]];
        [tapeEnd  setStringValue:[NSString stringWithFormat:@"%02d:%02d", 0, 0]];
        [tapeSlider setIntValue:0];
        [tapeSlider setEnabled:NO];
    }
    
    /* Expansion port */
    
    if ([c64 isCartridgeAttached]) {
        [cartridgeIcon setHidden:NO];
        [cartridgeText setStringValue:[NSString stringWithFormat:@"Type %d cartridge",
                                       [[c64 expansionport] cartridgeType]]];
    } else {
        [cartridgeIcon setHidden:YES];
        [cartridgeText setStringValue:@"No cartridge"];
    }
}

@end
