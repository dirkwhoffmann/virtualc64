/*
 * Author: Dirk W. Hoffmann, 2015
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

@implementation MyMetalView(Responder)


// --------------------------------------------------------------------------------
//                                  Drag and Drop
// --------------------------------------------------------------------------------

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    // NSLog(@"draggingEntered");
    if ([sender draggingSource] == self)
        return NSDragOperationNone;
    
    NSPasteboard *pb = [sender draggingPasteboard];
    NSString *besttype = [pb availableTypeFromArray:[NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];
    
    if (besttype == NSFilenamesPboardType) {
        // NSLog(@"Dragged in filename");
        return NSDragOperationCopy;
    }
    
    if (besttype == NSPasteboardTypeString) {
        // NSLog(@"Dragged in string");
        return NSDragOperationCopy;
    }
    
    if (besttype == NSFileContentsPboardType) {
        // NSLog(@"Dragged in file contents");
        return NSDragOperationCopy;
    }
    
    return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pb = [sender draggingPasteboard];
    
    if ([[pb types] containsObject:NSFileContentsPboardType]) {
        
        NSFileWrapper *fileWrapper = [pb readFileWrapper];
        NSData *fileData = [fileWrapper regularFileContents];
        SnapshotProxy *snapshot = [SnapshotProxy snapshotFromBuffer:[fileData bytes] length:(unsigned)[fileData length]];
        [[controller c64] loadFromSnapshot:snapshot];
        return YES;
    }
    
    if ([[pb types] containsObject:NSFilenamesPboardType]) {
        
        NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];
        NSLog(@"Processing file %@", path);
        
        // Is it a snapshot?
        if ([SnapshotProxy isSnapshotFile:path]) {
            
            NSLog(@"  Snapshot found");
            
            // Do the version numbers match?
            if ([SnapshotProxy isSnapshotFile:path
                                        major:V_MAJOR
                                        minor:V_MINOR
                                     subminor:V_SUBMINOR]) {
                
                SnapshotProxy *snapshot = [SnapshotProxy snapshotFromFile:path];
                if (snapshot) {
                    [[controller c64] loadFromSnapshot:snapshot];
                    return YES;
                }
                
            } else {
                
                NSLog(@"  ERROR: Version number in snapshot must be %d.%d", V_MAJOR, V_MINOR);
                [[controller document] showVersionNumberAlert];
                return NO;
            }
        }
        
        // Is it a ROM file?
        if ([[controller document] loadRom:path]) {
            return YES;
        }
        
        // Is it a cartridge?
        if ([[controller document] setCartridgeWithName:path]) {
            [controller mountCartridge];
            return YES;
        }

        // Is it a NIB archive?
        if ([[controller document] setNIBArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }

        // Is it a G64 archive?
        if ([[controller document] setG64ArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }
        
        // Is it a TAP container?
        if ([[controller document] setTAPContainerWithName:path]) {
            [controller showTapeDialog];
            return YES;
        }
        
        // Is it an archive other than G64?
        if ([[controller document] setArchiveWithName:path]) {
            [controller showMountDialog];
            return YES;
        }					
    }
    
    return NO;	
}

- (void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
}

@end



