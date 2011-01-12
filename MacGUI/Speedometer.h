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


#import <Cocoa/Cocoa.h>

@interface Speedometer : NSObject {

	
	// MOVE msec() FROM BASIC.H INTO THIS CLASS
	
	//! Current emulation speed
	/*! Call update before reading value */
	float mhz;

	//! Current drawing speed
	/*! Call update before reading value */
	float fps;
	
	//! Previous cycle count
	/*! Needed to calculate current clock frequency */
	long cycles;
	
	//! Previous frame count
	/*! Needed to determine frames per second */
	long frames;
	
	//! Remembers when the update method was called 
	/*! Needed to determine current clock frequency and frames per second */
	long timestamp;
}

@property (readonly) float mhz;
@property (readonly) float fps;

- (void)updateWithCurrentCycle:(long)cycles currentFrame:(long)frames;

@end
