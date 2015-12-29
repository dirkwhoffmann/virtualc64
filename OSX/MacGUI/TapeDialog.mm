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

@implementation TapeDialog

@synthesize archive;
@synthesize doAutoType;
@synthesize doPressPlay; 

- (void) initialize:(TAPContainerProxy *)tapproxy c64proxy:(C64Proxy *)proxy
{
    assert(tapproxy != NULL);
    assert(proxy != NULL);
    
    archive = [tapproxy container];
    c64 = proxy;
    
    doAutoType = YES;
    doPressPlay = YES;
    
    // Get physical path of archive
    NSString *archivePath = [tapproxy getPath];
    NSString *archiveDescr = [NSString stringWithFormat:@"TAP archive (V%ld)", (long)[tapproxy TAPversion]];
    
    // Set title
    [headerText setStringValue:archivePath];
    [diskIconFrame setTitle:archiveDescr];
    
    [self update];
}

- (void)update
{
    [typeButton setState:doAutoType];
    [pressPlayButton setState:doPressPlay];
    [pressPlayButton setEnabled:doAutoType];
}

#pragma mark Action methods

- (IBAction)autoTypeAction:(id)sender
{
    doAutoType = [(NSButton *)sender state];
    [self update];
}

- (IBAction)pressPlayAction:(id)sender
{
    doPressPlay = [(NSButton *)sender state];
    [self update];
}

@end
