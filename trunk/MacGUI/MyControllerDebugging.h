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

#ifndef INC_MYCONTROLLER_DEBUGGING
#define INC_MYCONTROLLER_DEBUGGING

#import "MyController.h"

@interface MyController(Debugging)

- (IBAction)hideSpritesAction:(id)sender;
- (IBAction)markIRQLinesAction:(id)sender;
- (IBAction)markDMALinesAction:(id)sender;

- (IBAction)traceC64CpuAction:(id)sender;
- (IBAction)traceIecAction:(id)sender;
- (IBAction)traceVC1541CpuAction:(id)sender;
- (IBAction)traceViaAction:(id)sender;

- (IBAction)dumpC64:(id)sender;
- (IBAction)dumpC64CPU:(id)sender;
- (IBAction)dumpC64CIA1:(id)sender;
- (IBAction)dumpC64CIA2:(id)sender;
- (IBAction)dumpC64VIC:(id)sender;
- (IBAction)dumpC64SID:(id)sender;
- (IBAction)dumpC64Memory:(id)sender;
- (IBAction)dumpVC1541:(id)sender;
- (IBAction)dumpVC1541CPU:(id)sender;
- (IBAction)dumpVC1541VIA1:(id)sender;
- (IBAction)dumpVC1541VIA2:(id)sender;
- (IBAction)dumpVC1541Memory:(id)sender;
- (IBAction)dumpKeyboard:(id)sender;
- (IBAction)dumpIEC:(id)sender;

@end

#endif
