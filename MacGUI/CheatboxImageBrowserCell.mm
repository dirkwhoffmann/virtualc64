#import "CheatboxImageBrowserCell.h"

@implementation CheatboxImageBrowserCell

- (NSImageAlignment)imageAlignment
{
	return NSImageAlignTop;
	// return NSImageAlignBottom;
	// return [super imageAlignment];
}


#if 0
- (NSRect) frame
{
	NSRect frame = [super frame];
	// frame.origin.y -= 15;
	//frame.size.height -= 40; 
	
	return frame;
}

- (NSRect) imageContainerFrame
{
	NSRect frame = [super imageContainerFrame];	
	return frame;
}
#endif


- (NSRect) selectionFrame
{
	NSRect frame = [super selectionFrame];
	frame.origin.y += 12;
	frame.size.height -= 24; 

	return frame;
}


@end



