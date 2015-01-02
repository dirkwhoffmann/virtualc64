/*
 * Authors: Dirk W. Hoffmann, 2015
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

@implementation ROMDropTargetView

@synthesize allowDrop;
@synthesize delegate;

// NSString *kPrivateDragUTI = @"www.dirkwhoffmann.de.virtualc64";

- (id)initWithCoder:(NSCoder *)coder
{
    NSLog(@"ROMDropTargetView:initWithCoder");
    
    self = [super initWithCoder:coder];

    if (self) {
        // Register pasteboard types
        [self registerForDraggedTypes:@[NSFilenamesPboardType]];
        self.allowDrop = YES;
    }
    
    return self;
}




- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    NSLog(@"ROMDropTargetView:draggingEntered");

    if (!self.allowDrop)
        return NSDragOperationNone;

    NSPasteboard *pb = [sender draggingPasteboard];
    NSString *besttype = [pb availableTypeFromArray:@[NSFilenamesPboardType]];
    
    if (besttype == NSFilenamesPboardType) {
        NSLog(@"ROMDropTargetView:Dragged in filename");
        return NSDragOperationCopy;
    }
    
    return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
    NSLog(@"ROMDropTargetView:draggingExited");
    
    highlight = NO;
    [self setNeedsDisplay:YES];
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pb = [sender draggingPasteboard];

    NSLog(@"ROMDropTargetView:prepareForDragOperation");
    
    highlight = NO;
    [self setNeedsDisplay: YES];

    // Did we receive a filename?
    if (![[pb types] containsObject:NSFilenamesPboardType])
        return NO;
    
    // Did we receive the name of a valid ROM image?
    NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];
    NSLog(@"Got filename %@", path);

#if 0
    // Is it a ROM file? DON'T LOAD HERE!
    if ([[controller document] loadRom:path]) {
        NSLog(@"ROM loaded");
        return YES;
    }
#endif
    
    return NO;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSLog(@"ROMDropTargetView:performDragOperation");
    return YES;
}


@end
