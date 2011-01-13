/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

#import "Speedometer.h"
#import "basic.h"

@implementation Speedometer

@synthesize mhz;
@synthesize fps;

- (id)init
{
	self = [super init];
    if (self) {	
		cycles = 0L;
		frames = 0L;
		timestamp = msec();
	}
	return self;
}

- (void)updateWithCurrentCycle:(long)currentCycles currentFrame:(long)currentFrames
{
	long currentTimestamp = msec();
	float elapsedTime = (float)(currentTimestamp - timestamp);
	
	// Measure clock frequency
	float elapsedCycles = (float)(currentCycles - cycles);
	mhz = elapsedCycles / elapsedTime;

	// Measure frames per second
	float elapsedFrames = (float)(currentFrames - frames);
	fps = elapsedFrames * 1000000.0 / elapsedTime;
	
	// Check values
	if (mhz < 0.0) mhz = 0.0;
	if (frames < 0.0) frames = 0.0;
	
	// Keep values 
	timestamp = currentTimestamp;
	cycles = currentCycles;
	frames = currentFrames;		
}

@end
