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

#import "C64GUI.h"

@implementation MyDocument

@synthesize c64;
@synthesize attachedSnapshot;
@synthesize attachedArchive;
@synthesize attachedTape;
@synthesize attachedCartridge;

- (void)makeWindowControllers
{
	NSLog(@"MyDocument::makeWindowControllers");
    
	MyController *myController;
	
	myController = [[MyController alloc] initWithWindowNibName:@"MyDocument"];
	[self addWindowController:myController];
	
    // Kepp aspect ratio during window resizing
	// [[self windowForSheet] setContentAspectRatio:NSMakeSize(804,621 + 20 /* control bar below C64 canvas */)];

    // Restore previous position and size
    // [[self windowForSheet] setFrameUsingName:@"dirkwhoffmann.de.virtualC64.window"];
}

- (id)init
{
	NSLog(@"MyDocument::init");
	
    self = [super init];

    attachedSnapshot = nil;
    attachedArchive = nil;
    attachedTape = nil;
	attachedCartridge = nil;
		
	return self;
}

- (void)awakeFromNib
{
    NSLog(@"MyDocument::awakeFromNib");
}

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
    if (!(attachedArchive = [D64ArchiveProxy archiveFromArbitraryFile:path]))
		return NO;
	
	return YES;
}

/*
- (BOOL)detachCartridge
{
	[c64 detachCartridge];
    // TODO: CHECK: DO WE NEED TO DEALLOC MANUALLY?
	attachedCartridge = nil;
	
	return YES;
}
*/

- (BOOL)loadRom:(NSString *)filename
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	bool success = NO;
	
	if ([c64 loadBasicRom:filename]) {
        NSLog(@"loadRom:setObject:%@ forKey:VC64BasicRomFileKey", filename);
		[defaults setObject:filename forKey:VC64BasicRomFileKey];
		success = YES;
	}
	else if ([c64 loadCharRom:filename]) {
        NSLog(@"loadRom:setObject:%@ forKey:VC64CharRomFileKey", filename);
		[defaults setObject:filename forKey:VC64CharRomFileKey];
		success = YES;
	}
	else if ([c64 loadKernelRom:filename]) {
        NSLog(@"loadRom:setObject:%@ forKey:VC64KernelRomFileKey", filename);
		[defaults setObject:filename forKey:VC64KernelRomFileKey];
		success = YES;
	}
	else if ([c64 loadVC1541Rom:filename]) {
        NSLog(@"loadRom:setObject:%@ forKey:VC64VC1541RomFileKey", filename);
		[defaults setObject:filename forKey:VC64VC1541RomFileKey];
		success = YES;
	}
	
	return success;
}

- (void)showVersionNumberAlert
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Snapshot from other VirtualC64 release"];
    [alert setInformativeText:@"The snapshot was created with a different version of VirtualC64 and cannot be opened."];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}


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
    
    // Is it a snapshot?
    if ([SnapshotProxy isSnapshotFile:filename]) {
        
        // Do version numbers match?
        if ([SnapshotProxy isSnapshotFile:filename
                                    major:V_MAJOR
                                    minor:V_MINOR
                                 subminor:V_SUBMINOR]) {
            
            // Try to read snapshot ...
            if (![self setSnapshotWithName:filename]) {
                NSLog(@"Error while reading snapshot\n");
                return NO;
            }
            return YES;
        }
        
        [self showVersionNumberAlert];
        return NO;
    }
    
#if 0
    // Is it some supported file format?
    ContainerType format = 0;
    if ([D64ArchiveProxy isD64File:filename])        format = D64_CONTAINER;
    else if ([T64ArchiveProxy isT64File:filename])   format = T64_CONTAINER;
    else if ([PRGArchiveProxy isPRGFile:filename])   format = PRG_CONTAINER;
    else if ([P00ArchiveProxy isP00File:filename])   format = P00_CONTAINER;
    else if ([G64ArchiveProxy isG64File:filename])   format = G64_CONTAINER;
    else if ([NIBArchiveProxy isNIBFile:filename])   format = NIB_CONTAINER;
    else if ([TAPContainerProxy isTAPFile:filename]) format = TAP_CONTAINER;
    else if ([CRTContainerProxy isCRTFile:filename]) format = CRT_CONTAINER;
#endif 

    // Check file type
    ContainerType format = UNKNOWN_CONTAINER_FORMAT;
    if ([type caseInsensitiveCompare:@"D64"] == NSOrderedSame)
        format = D64_CONTAINER;
    else if ([type caseInsensitiveCompare:@"T64"] == NSOrderedSame)
        format = T64_CONTAINER;
    else if ([type caseInsensitiveCompare:@"PRG"] == NSOrderedSame)
        format = PRG_CONTAINER;
    else if ([type caseInsensitiveCompare:@"P00"] == NSOrderedSame)
        format = P00_CONTAINER;
    else if ([type caseInsensitiveCompare:@"G64"] == NSOrderedSame)
        format = G64_CONTAINER;
    else if ([type caseInsensitiveCompare:@"NIB"] == NSOrderedSame)
        format = NIB_CONTAINER;
    else if ([type caseInsensitiveCompare:@"TAP"] == NSOrderedSame)
        format = TAP_CONTAINER;
    else if ([type caseInsensitiveCompare:@"CRT"] == NSOrderedSame)
        format = CRT_CONTAINER;
    
    switch (format) {
            
        case D64_CONTAINER:
        case T64_CONTAINER:
        case PRG_CONTAINER:
        case P00_CONTAINER:
		
            if ([self setArchiveWithName:filename])
                return YES;
            else break;
            
        case G64_CONTAINER:
            
            if ([self setG64ArchiveWithName:filename])
                return YES;
            else break;

        case NIB_CONTAINER:
            
            if ([self setNIBArchiveWithName:filename])
                return YES;
            else break;

        case TAP_CONTAINER:
            
            if ([self setTAPContainerWithName:filename])
                return YES;
            else break;

        case CRT_CONTAINER:

            if ([self setCRTContainerWithName:filename])
                return YES;
            else break;

        default:

            NSLog(@"Unsupported file type\n");
            return NO;
    }

    NSLog(@"Error while reading file\n");
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
