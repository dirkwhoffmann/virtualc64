#import "CheatboxImageBrowserCell.h"

@implementation CheatboxImageBrowserCell

- (NSImageAlignment)imageAlignment
{
	return NSImageAlignTop;
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
	frame.origin.y += 25;
	frame.size.height -= 50; 

	return frame;
}

@end



