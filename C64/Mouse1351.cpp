/*!
 * @header      Mouse1351.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Mouse1351.h"

#include "C64.h"

Mouse1351::Mouse1351() {
    
    setDescription("Mouse1351");
    debug(3, "    Creating Mouse1351 %d at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &mouseX,          sizeof(mouseX),         CLEAR_ON_RESET },
        { &mouseTargetX,    sizeof(mouseTargetX),   CLEAR_ON_RESET },
        { &mouseY,          sizeof(mouseY),         CLEAR_ON_RESET },
        { &mouseTargetY,    sizeof(mouseTargetY),   CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Mouse1351::~Mouse1351()
{
}

void
Mouse1351::reset()
{
    VirtualComponent::reset();
}


void
Mouse1351::execute()
{
    debug("Executing mouse (x = %d y = %d)\n", mouseX, mouseY);
    
    if (mouseX == mouseTargetX && mouseY == mouseTargetY)
        return;
    
    if (mouseTargetX < mouseX) mouseX -= MIN(mouseX - mouseTargetX, 31);
    else if (mouseTargetX > mouseX) mouseX += MIN(mouseTargetX - mouseX, 31);
    if (mouseTargetY < mouseY) mouseY -= MIN(mouseY - mouseTargetY, 31);
    else if (mouseTargetY > mouseY) mouseY += MIN(mouseTargetY - mouseY, 31);
    
    // Let the new mouse coordinate show up in the SID register
    c64->sid.potX = ((mouseX & 0x3F) << 1) | 0x00;
    c64->sid.potY = ((mouseY & 0x3F) << 1) | 0x00;
}
