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

@protocol DragDropImageViewDelegate;

@interface ROMDropTargetView : NSImageView <NSDraggingDestination>
{
    // Drop zone highlighting on/off
    BOOL highlight;
}

@property (assign) BOOL allowDrop;
@property (assign) id<DragDropImageViewDelegate> delegate;

- (id)initWithCoder:(NSCoder *)coder;

@end


#if 0

@protocol DragDropImageViewDelegate <NSObject>

- (void)dropComplete:(NSString *)filePath;

@end

#endif

