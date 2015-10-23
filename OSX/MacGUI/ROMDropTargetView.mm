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

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];

    if (self) {
        
        [self registerForDraggedTypes:@[NSFilenamesPboardType]];
        self.allowDrop = YES;
        
    }
    return self;
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    if (!self.allowDrop)
        return NSDragOperationNone;

    originalImage = [self image];

    NSPasteboard *pb = [sender draggingPasteboard];
    NSString *besttype = [pb availableTypeFromArray:@[NSFilenamesPboardType]];
    
    if (besttype == NSFilenamesPboardType) {

        NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];

        if ([[controller c64] isRom:path]) {
            [self setImage:[NSImage imageNamed:@"romchip"]];
            [self setNeedsDisplay];
            return NSDragOperationCopy;
        }
    }
    
    return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
    if (!self.allowDrop)
        return;

    [self setImage:originalImage];
    [self setNeedsDisplay];
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pb = [sender draggingPasteboard];
    
    // Load ROM
    NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];
    return [[controller document] loadRom:path];
}


@end
