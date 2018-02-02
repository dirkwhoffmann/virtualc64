/*
 * (C) 2006 - 2016 Dirk W. Hoffmann. All rights reserved.
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

/*
#import "C64GUI.h"
#import "VirtualC64-Swift.h"

@implementation MyDocument

@synthesize c64;
@synthesize attachedSnapshot;
@synthesize attachedArchive;
@synthesize attachedTape;
@synthesize attachedCartridge;

- (id)init
{
    self = [super init];
    
    [self initSwift];
    
    return self;
}
*/

/*
- (BOOL)setSnapshotWithName:(NSString *)path
{
    if (!(attachedSnapshot = [SnapshotProxy snapshotFromFile:path]))
        return NO;
    
    return YES;
}

- (BOOL)setG64ArchiveWithName:(NSString *)path
{
    if (!(attachedArchive = [G64ArchiveProxy archiveFromG64File:path]))
        return NO;
    
    return YES;
}

- (BOOL)setNIBArchiveWithName:(NSString *)path
{
    if (!(attachedArchive = [NIBArchiveProxy archiveFromNIBFile:path]))
        return NO;
    
    return YES;
}

- (BOOL)setTAPContainerWithName:(NSString *)path
{
    if (!(attachedTape = [TAPContainerProxy containerFromTAPFile:path]))
        return NO;
    
    return YES;
}

- (BOOL)setCRTContainerWithName:(NSString *)path
{
    if (!(attachedCartridge = [CRTContainerProxy containerFromCRTFile:path]))
        return NO;
    
    return YES;
}

- (BOOL)setArchiveWithName:(NSString *)path
{
    attachedArchive = [ArchiveProxy makeArchiveFromFile:path];
    return attachedArchive != NULL;
}
*/

/*
- (void)showVersionNumberAlert
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Snapshot from other VirtualC64 release"];
    [alert setInformativeText:@"The snapshot was created with a different version of VirtualC64 and cannot be opened."];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}
*/

/*
-(BOOL)writeToFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:writeToFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
	
    SnapshotProxy *s = [[SnapshotProxy alloc] init];
    [c64 saveToSnapshot:s];
	[s writeDataToFile:filename];
	
	return YES;
}

-(BOOL)readFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:readFromFile:%@ ofType:%@", filename, type);
    
    // Is it a snapshot from a different version?
    if ([SnapshotProxy isUsupportedSnapshot:filename]) {
        [self showVersionNumberAlert];
        return NO;
    }
    
    // Is it a snapshop with a matching version number?
    if ([self setSnapshotWithName:filename]) {
        return YES;
    }
    
    // Is it an archive?
    if ([self setArchiveWithName:filename]) {
        [self setFileURL:nil]; // Make the document 'Untitled'
        return YES;
    }
    
    // Is it a band tape?
    if ([self setTAPContainerWithName:filename]) {
        [self setFileURL:nil];
        return YES;
    }
    
    // Is it a cartridge?
    if ([self setCRTContainerWithName:filename]) {
        [self setFileURL:nil];
        return YES;
    }
   
    NSLog(@"Unable to read file\n");
    return NO;
}

- (BOOL)revertToSavedFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:revertToSavedFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
			
	SnapshotProxy *reverted = [SnapshotProxy snapshotFromFile:filename];

	if (!reverted) {
		NSLog(@"Error while reverting to older snapshot\n");
		return NO;
	}
		
	[c64 loadFromSnapshot:reverted];
	return YES;
}


@end
 */
