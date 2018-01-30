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

#import "MyController.h"

@interface MyController(Menu)

// - (BOOL)validateMenuItem:(NSMenuItem *)item;

#pragma mark file menu

/*! @brief   Action function for save screenshot feature
 *  @details This method is called when the current emulator image is saved
 *           to disk.
 */
- (IBAction)saveScreenshotDialog:(id)sender;

/*! @brief   Action function for the quick save screenshot feature
 *  @details A screenshot is taken is saved to a file on the users desktop.
 */
- (IBAction)quicksaveScreenshot:(id)sender;

/*! @brief   Action function for export disk feature
 *  @details This method is called when exporting to archives that are capable of 
 *           storing multiple file. 
 *  @see     exportDiskDialogWorker
 */
// - (IBAction)exportDiskDialog:(id)sender;

/*! @brief   Main functionality of exportDiskDialog
 '  @result  true if disk contents has been exportet, false if operation was canceled. 
 */
// - (bool)exportDiskDialogWorker:(long)type;

/*! Action function for export disk feature
 *  This method is called when exporting to archives that are capable of storing a single file, only.
 */
// - (IBAction)exportFileFromDiskDialog:(id)sender;

#pragma mark edit view
// - (IBAction)toggleStatusBarAction:(id)sender;
// - (IBAction)showStatusBar;
// - (IBAction)hideStatusBar;

#pragma mark edit menu
// - (IBAction)resetAction:(id)sender;

#pragma mark keyboard menu
/*
- (IBAction)runstopAction:(id)sender;
- (IBAction)shiftRunstopAction:(id)sender;
- (IBAction)restoreAction:(id)sender;
- (IBAction)runstopRestoreAction:(id)sender;
- (IBAction)commodoreKeyAction:(id)sender;
- (IBAction)homeKeyAction:(id)sender;
- (IBAction)clearKeyAction:(id)sender;
- (IBAction)deleteKeyAction:(id)sender;
- (IBAction)insertKeyAction:(id)sender;
- (IBAction)loadDirectoryAction:(id)sender;
- (IBAction)loadFirstFileAction:(id)sender;
- (IBAction)formatDiskAction:(id)sender;
*/

#pragma mark peripherals menu
- (IBAction)datasetteEjectAction:(id)sender;
- (IBAction)datasettePressPlayAction:(id)sender;
- (IBAction)datasettePressStopAction:(id)sender;

@end
