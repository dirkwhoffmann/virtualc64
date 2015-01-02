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

#import "ROMDropTargetView.h"

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
        [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType,nil]];
        self.allowDrop = YES;
    }
    
    return self;
}




- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    NSLog(@"ROMDropTargetView:draggingEntered");
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
    NSLog(@"ROMDropTargetView:prepareForDragOperation");
    
    highlight = NO;
    [self setNeedsDisplay: YES];
    
    // Verify data type
    return [NSImage canInitWithPasteboard:[sender draggingPasteboard]];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSLog(@"ROMDropTargetView:performDragOperation");
    return YES;
}


@end
