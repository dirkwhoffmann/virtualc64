#import "CheatboxImageBrowserCell.h"

@implementation CheatboxImageBrowserCell

- (NSImageAlignment)imageAlignment
{
	return NSImageAlignTop;
}

- (NSRect) selectionFrame
{
	NSRect frame = [super selectionFrame];
	frame.origin.y += 12;
	frame.size.height -= 24; 

	return frame;
}


@end



