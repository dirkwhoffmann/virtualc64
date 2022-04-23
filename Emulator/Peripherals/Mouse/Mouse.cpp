// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Mouse.h"
#include "C64.h"
#include "IOUtils.h"
#include <cmath>

Mouse::Mouse(C64 &ref, ControlPort& pref) : SubComponent(ref), port(pref)
{
    subComponents = std::vector<C64Component *> {
        
        &mouse1350,
        &mouse1351,
        &mouseNeos
    };
}

void Mouse::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)

    targetX = 0;
    targetY = 0;
}

void
Mouse::resetConfig()
{
    setConfigItem(OPT_MOUSE_MODEL, MOUSE_C1350);
    setConfigItem(OPT_SHAKE_DETECTION, true);
    setConfigItem(OPT_MOUSE_VELOCITY, 100);
}

i64
Mouse::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_MOUSE_MODEL:      return config.model;
        case OPT_SHAKE_DETECTION:  return config.shakeDetection;
        case OPT_MOUSE_VELOCITY:   return config.velocity;

        default:
            fatalError;
    }
}

void
Mouse::setConfigItem(Option option, i64 value)
{    
    switch (option) {
            
        case OPT_MOUSE_MODEL:
            
            config.model = (MouseModel)value;
            _reset(true);
            return;
            
        case OPT_SHAKE_DETECTION:
            
            config.shakeDetection = value;
            return;
            
        case OPT_MOUSE_VELOCITY:
                        
            if (value < 0 || value > 255) {
                throw VC64Error(ERROR_OPT_INVARG, "0 ... 255");
            }
            config.velocity= value;
            updateScalingFactors();
            return;

        default:
            return;
    }
}

void
Mouse::updateScalingFactors()
{
    assert((unsigned long)config.velocity < 256);
    scaleX = scaleY = (double)config.velocity / 100.0;
}

void
Mouse::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;

    if (category & dump::Config) {
        
        os << tab("Mouse nr") << dec(port.nr) << std::endl;
        os << tab("Model") << MouseModelEnum::key(config.model) << std::endl;
        os << tab("Shake detection") << bol(config.shakeDetection) << std::endl;
        os << tab("Velocity") << config.velocity << std::endl;
    }

    if (category & dump::State) {
        
        os << tab("Mouse nr") << dec(port.nr) << std::endl;
        os << tab("targetX") << targetX << std::endl;
        os << tab("targetY") << targetY << std::endl;
    }    
}

bool
Mouse::detectShakeXY(double x, double y)
{
    if (config.shakeDetection && shakeDetector.isShakingAbs(x)) {
        msgQueue.put(MSG_SHAKING);
        return true;
    }
    return false;
}

bool
Mouse::detectShakeDxDy(double dx, double dy)
{
    if (config.shakeDetection && shakeDetector.isShakingRel(dx)) {
        msgQueue.put(MSG_SHAKING);
        return true;
    }
    return false;
}

void
Mouse::setXY(double x, double y)
{
    debug(PRT_DEBUG, "setXY(%f,%f)\n", x, y);
        
    targetX = x * scaleX;
    targetY = y * scaleX;
    
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::setDxDy(double dx, double dy)
{
    debug(PRT_DEBUG, "setDxDy(%f,%f)\n", dx, dy);

    targetX += dx * scaleX;
    targetY += dy * scaleY;
    
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::setLeftButton(bool value)
{
    debug(PRT_DEBUG, "setLeftButton(%d)\n", value);
    
    switch(config.model) {
            
        case MOUSE_C1350: mouse1350.setLeftMouseButton(value); break;
        case MOUSE_C1351: mouse1351.setLeftMouseButton(value); break;
        case MOUSE_NEOS:  mouseNeos.setLeftMouseButton(value); break;
            
        default:
            fatalError;
    }
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::setRightButton(bool value)
{
    debug(PRT_DEBUG, "setRightButton(%d)\n", value);

    switch(config.model) {
            
        case MOUSE_C1350: mouse1350.setRightMouseButton(value); break;
        case MOUSE_C1351: mouse1351.setRightMouseButton(value); break;
        case MOUSE_NEOS:  mouseNeos.setRightMouseButton(value); break;
            
        default:
            fatalError;
    }
    port.device = CPDEVICE_MOUSE;
}

void
Mouse::trigger(GamePadAction event)
{
    assert_enum(GamePadAction, event);

    debug(PRT_DEBUG, "trigger(%lld)\n", event);
    
    switch (event) {

        case PRESS_LEFT: setLeftButton(true); break;
        case RELEASE_LEFT: setLeftButton(false); break;
        case PRESS_RIGHT: setRightButton(true); break;
        case RELEASE_RIGHT: setRightButton(false); break;
            
        default:
            break;
    }
}

void
Mouse::risingStrobe()
{
    mouseNeos.risingStrobe((i64)targetX, (i64)targetY);
}

void
Mouse::fallingStrobe()
{
    mouseNeos.fallingStrobe((i64)targetX, (i64)targetY);
}

void
Mouse::updatePotX()
{
    if (config.model == MOUSE_C1351) {
        mouse1351.executeX((i64)targetX);
    }
}

void
Mouse::updatePotY()
{
    if (config.model == MOUSE_C1351) {
        mouse1351.executeY((i64)targetY);
    }
}

void updatePotY();

u8
Mouse::readPotX() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readPotX();
        case MOUSE_C1351: return mouse1351.readPotX();
        case MOUSE_NEOS:  return mouseNeos.readPotX();
            
        default:
            fatalError;
    }
}

u8
Mouse::readPotY() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readPotY();
        case MOUSE_C1351: return mouse1351.readPotY();
        case MOUSE_NEOS:  return mouseNeos.readPotY();
            
        default:
            fatalError;
    }
}

void
Mouse::updateControlPort()
{
    if (config.model == MOUSE_NEOS) {
        mouseNeos.updateControlPort((i64)targetX, (i64)targetY);
    }
}

u8
Mouse::getControlPort() const
{
    switch(config.model) {
            
        case MOUSE_C1350: return mouse1350.readControlPort();
        case MOUSE_C1351: return mouse1351.readControlPort();
        case MOUSE_NEOS:  return mouseNeos.readControlPort();
            
        default:
            fatalError;
    }
}

void
Mouse::execute()
{
    switch(config.model) {
            
        case MOUSE_C1350:
            
            mouse1350.execute((i64)targetX, (i64)targetY);
            break;
            
        case MOUSE_C1351:
            
            // Coordinates are updated in readPotX() and readPotY()
            break;
            
        case MOUSE_NEOS:
            
            // Coordinates are updated in latchPosition()
            break;
            
        default:
            fatalError;
    }
}

bool
ShakeDetector::isShakingAbs(double newx)
{
    return isShakingRel(newx - x);
}

bool
ShakeDetector::isShakingRel(double dx) {
    
    // Accumulate the travelled distance
    x += dx;
    dxsum += abs(dx);
    
    // Check for a direction reversal
    if (dx * dxsign < 0) {
    
        u64 dt = util::Time::now().asNanoseconds() - lastTurn;
        dxsign = -dxsign;

        // A direction reversal is considered part of a shake, if the
        // previous reversal happened a short while ago.
        if (dt < 400 * 1000 * 1000) {
  
            // Eliminate jitter by demanding that the mouse has travelled
            // a long enough distance.
            if (dxsum > 400) {
                
                dxturns += 1;
                dxsum = 0;
                
                // Report a shake if the threshold has been reached.
                if (dxturns > 3) {
                    
                    // printf("Mouse shake detected\n");
                    lastShake = util::Time::now().asNanoseconds();
                    dxturns = 0;
                    return true;
                }
            }
            
        } else {
            
            // Time out. The user is definitely not shaking the mouse.
            // Let's reset the recorded movement histoy.
            dxturns = 0;
            dxsum = 0;
        }
        
        lastTurn = util::Time::now().asNanoseconds();
    }
    
    return false;
}
