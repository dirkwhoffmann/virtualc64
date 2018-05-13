/*!
 * @header      Mouse1351.cpp
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

#include "C64.h"

Mouse1351::Mouse1351() {
    
    setDescription("Mouse1351");
    debug(3, "    Creating Mouse1351 at address %p...\n", this);
    
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
Mouse1351::connect(unsigned port)
{
    assert(port == 0 || port == 1 || port == 2);
    
    debug("Connecting Mouse1351 to port %d\n", port);
    this->port = port;
    
    if (port == 0) {
        // Clean up SID side
        c64->sid.potX = 0xFF;
        c64->sid.potY = 0xFF;
    }
}

void
Mouse1351::setXY(double x, double y, bool s)
{
    // Translate into C64 coordinate system (this is a hack)
    const double xmax = 380.0;
    const double ymax = 268.0;
    x = x * xmax + 22;
    y = y * ymax + 10;
    mouseTargetX = (uint32_t)MIN(MAX(x, 0.0), xmax);
    mouseTargetY = (uint32_t)MIN(MAX(y, 0.0), ymax);
    silent = s;
}

void
Mouse1351::setLeftButton(bool pressed)
{
    ControlPort *p = (port == 1) ? &c64->port1 : (port == 2) ? &c64->port2 : NULL;

    if (p) {
        p->trigger(pressed ? PRESS_FIRE : RELEASE_FIRE);
    }
}

void
Mouse1351::setRightButton(bool pressed)
{
    ControlPort *p = (port == 1) ? &c64->port1 : (port == 2) ? &c64->port2 : NULL;
    
    if (p) {
        // WHAT TO DO HERE? 
    }
}

void
Mouse1351::execute()
{
    if (mouseX == mouseTargetX && mouseY == mouseTargetY)
        return;
    
    if (mouseTargetX < mouseX) mouseX -= MIN(mouseX - mouseTargetX, 31);
    else if (mouseTargetX > mouseX) mouseX += MIN(mouseTargetX - mouseX, 31);
    if (mouseTargetY < mouseY) mouseY -= MIN(mouseY - mouseTargetY, 31);
    else if (mouseTargetY > mouseY) mouseY += MIN(mouseTargetY - mouseY, 31);
    
    // Update SID registers if mouse is connected
    if (port != 0 && !silent) {
        c64->sid.potX = ((mouseX & 0x3F) << 1) | 0x00;
        c64->sid.potY = ((mouseY & 0x3F) << 1) | 0x00;
    }
}
