/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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


// TODOs
// - Add a separate NSWindowController (MyWindowController) and make it the files owner
// - Add categories 
//    CpuPanelController (data source for CPU panel)
//    MemoryPanelController (data source for Memory panel)
//    CiaPanelController (data source for CIA panel)
//    VicPanelController (does this make sense???) 
//    TimeTravelController 


#ifndef INC_MYDOCUMENT
#define INC_MYDOCUMENT

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>
#import "C64.h"
#import "Formatter.h"
#import "Disassembler.h"
#import "VICScreen.h"
#import "MountDialog.h"
#import "RomDialog.h"
#import "MyController.h"
#import "PreferenceController.h"
#import "AudioDevice.h"
#import "C64Proxy.h"
#import "JoystickManager.h"
#import "TimeTravelTableView.h"

@interface MyDocument : NSDocument
{
	// Data model
	// Implements a bridge between C++ (simulator) and Objective-C (GUI)
	C64Proxy *c64;
}


@property C64Proxy *c64;

//-(void)initC64WithScreen:

@end

#endif

