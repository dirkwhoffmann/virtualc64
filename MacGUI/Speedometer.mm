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
@synthesize jitter;

- (id)init
{
	self = [super init];
    if (self) {	
		latched_cycle = 0L;
		latched_frame = 0L;
		latched_timestamp = msec();
	}
	return self;
}

- (void)updateWithCurrentCycle:(long)cycle currentFrame:(long)frame expectedSpeed:(double)expectedMhz;

{
    // Measure elapsed time in milliseconds
    long timestamp = msec();
    double elapsedTime = (double)(timestamp - latched_timestamp);
	
	// Measure clock frequency
	double elapsedCycles = (double)(cycle- latched_cycle);
	mhz = elapsedCycles / elapsedTime;

	// Measure frames per second
	double elapsedFrames = (double)(frame - latched_frame);
	fps = elapsedFrames * 1000000.0 / elapsedTime;
	
	// Check values
	if (mhz < 0.0) mhz = 0.0;
	if (fps < 0.0) fps = 0.0;
	
	// Keep values 
	latched_timestamp = timestamp;
	latched_cycle = cycle;
	latched_frame = frame;
}

@end
