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

@implementation MountDragView

@synthesize allowDrag;
// @synthesize allowDrop;
// @synthesize delegate;

// NSString *kPrivateDragUTI = @"com.yourcompany.cocoadraganddrop";

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    
    if (self) {
        
        // [self registerForDraggedTypes:@[NSFilenamesPboardType]];
        self.allowDrag = YES;
        
    }
    return self;
}

#pragma mark - Source Operations

- (void)mouseDown:(NSEvent*)event
{
    if (self.allowDrag) {
        NSPoint dragPosition;
        NSRect imageLocation;
        
        dragPosition = [self convertPoint:[event locationInWindow] fromView:nil];
        dragPosition.x -= 16;
        dragPosition.y -= 16;
        imageLocation.origin = dragPosition;
        imageLocation.size = NSMakeSize(32,32);
        [self dragPromisedFilesOfTypes:[NSArray arrayWithObject:NSPasteboardTypeTIFF] fromRect:imageLocation source:self slideBack:YES event:event];
    }
}

- (void)dragImage:(NSImage *)anImage at:(NSPoint)viewLocation offset:(NSSize)initialOffset event:(NSEvent *)event pasteboard:(NSPasteboard *)pboard source:(id)sourceObj slideBack:(BOOL)slideFlag
{
    //create a new transparent copy of the drag image
    NSImage* dragImage=[[NSImage alloc] initWithSize:[[self image] size]];
    [dragImage lockFocus];
    [[self image] drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:0.33];
    [dragImage unlockFocus];
    
    [super dragImage:dragImage at:self.bounds.origin offset:NSZeroSize event:event pasteboard:pboard source:sourceObj slideBack:slideFlag];
}

- (NSArray *)namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination
{
    // Ask the controller to perform the drag operation
    if (![controller exportToD64:[dropDestination path]]) {
        NSLog(@"Failed to create D64 archive");
              
    }
    
    
    return [NSArray arrayWithObjects:@"test.png", nil];
}

- (NSDragOperation)draggingSession:(NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context
{
    switch (context) {

        case NSDraggingContextOutsideApplication:
            return NSDragOperationCopy;
    }
    
#if 0
            // case NSDraggingContextWithinApplication:
        default:
            return NSDragOperationCopy;
    }
#endif
    return NSDragOperationCopy;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event
{
     return YES;
}

- (void)pasteboard:(NSPasteboard *)sender item:(NSPasteboardItem *)item provideDataForType:(NSString *)type
{
     //sender has accepted the drag and now we need to send the data for the type we promised
    if ([type compare: NSPasteboardTypeTIFF] == NSOrderedSame) {
        
        //set data for TIFF type on the pasteboard as requested
        [sender setData:[[self image] TIFFRepresentation] forType:NSPasteboardTypeTIFF];
        
    } else if ([type compare: NSPasteboardTypePDF] == NSOrderedSame) {
        
        //set data for PDF type on the pasteboard as requested
        [sender setData:[self dataWithPDFInsideRect:[self bounds]] forType:NSPasteboardTypePDF];
    }
    
}
@end
