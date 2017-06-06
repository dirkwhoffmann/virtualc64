/*
 * (C) 2009 - 2017 Benjamin Klein, Dirk Hoffmann. All rights reserved.
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

#include "C64GUI.h"

typedef struct {
    JoystickManager *manager;
    int locationID;
    IOHIDDeviceRef deviceRef;
} CallbackContext;


// ---------------------------------------------------------------------------------------------
//                                             IOHIDDeviceInfo
// ---------------------------------------------------------------------------------------------

IOHIDDeviceInfo::IOHIDDeviceInfo()
{
    _name = NULL;
    _locationID = 0;
}

IOHIDDeviceInfo::IOHIDDeviceInfo(const IOHIDDeviceInfo &copy)
{
    if(copy._name) {
        size_t len = strlen(copy._name);
        _name = new char[len + 1];
        strncpy(_name, copy._name, len);
    } else {
        _name = NULL;
    }
    
    _locationID = copy._locationID;
}

IOHIDDeviceInfo::IOHIDDeviceInfo(IOHIDDeviceRef device)
{
    CFTypeRef typeRef;
    
    _name = NULL;
    _locationID = 0;
    
    if (device == NULL) {
        NSLog(@"IOHIDDeviceRef == NULL");
        return;
    }
    
    if((typeRef = IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)))) {
        
        if (CFStringGetTypeID() == CFGetTypeID(typeRef)) {
            CFIndex len = CFStringGetLength( (CFStringRef) typeRef ) + 1;
            
            _name = new char[ len ];
            _name[ len - 1 ] = 0;
            
            CFStringGetCString((CFStringRef)typeRef, _name, len, kCFStringEncodingMacRoman);
        }
    }
    
    if((typeRef = IOHIDDeviceGetProperty( device, CFSTR( kIOHIDLocationIDKey)))  &&  (CFNumberGetTypeID() == CFGetTypeID(typeRef)))
        CFNumberGetValue((CFNumberRef)typeRef, kCFNumberIntType, &_locationID);
}

IOHIDDeviceInfo::~IOHIDDeviceInfo()
{
    if(_name)
        delete [] _name;
}

// ---------------------------------------------------------------------------------------------
//                                             JoystickManager
// ---------------------------------------------------------------------------------------------

JoystickManager::JoystickManager(C64Proxy *proxy)
{
    _proxy = proxy;
    _manager = NULL;
}

bool JoystickManager::initialize()
{
    const int deviceFilter[][2] = {
        { kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick },
        { kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad }
    };
    
    CFMutableArrayRef matchingArray = NULL;
    CFMutableDictionaryRef dict = NULL;
    CFNumberRef number;
    IOReturn status;
    bool success = false;
    
    // Allocate matching array
    if(!(matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks))) {
        NSLog(@"Cannot create mutable array");
        goto bailout;
    }
    
    // Fill matching array
    for(unsigned n = 0; n < sizeof(deviceFilter) / sizeof(deviceFilter[0]); n++) {
        
        if (!(dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                               &kCFTypeDictionaryKeyCallBacks,
                                               &kCFTypeDictionaryValueCallBacks))) {
            NSLog( @"Cannot create mutable dictionary");
            goto bailout;
        }
        
        if (!(number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &deviceFilter[n][0]))) {
            NSLog(@"Cannot create CFNumberCreate");
            goto bailout;
        }
        
        CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsagePageKey), number);
        CFRelease(number);
        
        if(deviceFilter[n][1]) {
            if (!(number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &deviceFilter[n][1]))) {
                NSLog(@"Cannot create CFNumberCreate");
                goto bailout;
            }
            
            CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), number);
            CFRelease(number);
        }
        
        CFArrayAppendValue(matchingArray, dict);
    }
    
    // Create HIDManager
    if (!(_manager = IOHIDManagerCreate(kCFAllocatorDefault, 0))) {
        NSLog(@"Cannot create HIDManager");
        goto bailout;
    }
    
    // Configure HIDManager
    IOHIDManagerSetDeviceMatchingMultiple(_manager, matchingArray);
    IOHIDManagerRegisterDeviceMatchingCallback(_manager, MatchingCallback_static, this);
    IOHIDManagerScheduleWithRunLoop(_manager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    
    // Open HIDManager
    if ((status = IOHIDManagerOpen(_manager, kIOHIDOptionsTypeNone)) != kIOReturnSuccess) {
        NSLog(@"Failed to open HIDManager (status = %i)", status);
        goto bailout;
    }
    
    [_proxy putMessage:MSG_JOYSTICK_REMOVED];
    NSLog(@"HID manager is initialized");

    success = true;
    
bailout:
    
    if (matchingArray)
        CFRelease(matchingArray);
    if (dict)
        CFRelease(dict);
    
    return success;
}

JoystickManager::~JoystickManager()
{
    IOHIDManagerClose(_manager, kIOHIDOptionsTypeNone);
    CFRelease(_manager);
}

bool JoystickManager::joystickIsPluggedIn(int nr)
{
    assert (nr >= 1 && nr <= 2);
    return usbjoy[nr - 1].pluggedIn;
}

void
JoystickManager::bindJoystick(int nr, JoystickProxy *joy)
{
    assert (nr >= 1 && nr <= 2);

    if (usbjoy[nr - 1].pluggedIn) {
    
        usbjoy[nr - 1].bindJoystick(joy);

        if (joy == NULL)
            fprintf(stderr, "Remove binding for %s USB joystick\n", nr == 1 ? "first" : "second");
        else
            fprintf(stderr, "Bind %s USB joystick to %p\n", nr == 1 ? "first" : "second", joy);
    }
}

void
JoystickManager::unbindJoysticksFromPortA()
{
    for (unsigned i = 0; i < 2; i++) {
        if (usbjoy[i].joystick == [_proxy joystickA])
            usbjoy[i].unbindJoystick();
    }
}

void
JoystickManager::unbindJoysticksFromPortB()
{
    for (unsigned i = 0; i < 2; i++) {
        if (usbjoy[i].joystick == [_proxy joystickB])
            usbjoy[i].unbindJoystick();
    }
}

bool JoystickManager::addJoystickProxyWithLocationID(int locationID)
{
    
    if (!usbjoy[0].pluggedIn) {
        usbjoy[0].pluggedIn = true;
        usbjoy[0].locationID = locationID;
        NSLog(@"Joystick with ID %d added to slot 0", locationID);
        return true;
    }
    
    if (!usbjoy[1].pluggedIn) {
        usbjoy[1].pluggedIn = true;
        usbjoy[1].locationID = locationID;
        NSLog(@"Joystick with ID %d added to slot 1", locationID);
        return true;
    }
    
    return false;
}

USBJoystick *JoystickManager::getJoystickProxyWithLocationID(int locationID)
{
    if (usbjoy[0].pluggedIn && usbjoy[0].locationID == locationID)
        return &usbjoy[0];

    if (usbjoy[1].pluggedIn && usbjoy[1].locationID == locationID)
        return &usbjoy[1];
    
    return NULL;
}

void JoystickManager::removeJoystickProxyWithLocationID(int locationID)
{
    if (usbjoy[0].pluggedIn && usbjoy[0].locationID == locationID) {
        usbjoy[0].pluggedIn = false;
        usbjoy[0].locationID = 0;
        NSLog(@"Joystick with ID %d removed from slot 0", locationID);
    }

    if (usbjoy[1].pluggedIn && usbjoy[1].locationID == locationID) {
        usbjoy[1].pluggedIn = false;
        usbjoy[1].locationID = 0;
        NSLog(@"Joystick with ID %d removed from slot 1", locationID);
    }
}

void JoystickManager::listJoystickManagers()
{
    NSLog(@"USB joystick slot 1: (ID %d)", usbjoy[0].locationID);
    NSLog(@"USB joystick slot 2: (ID %d)", usbjoy[1].locationID);
}




void JoystickManager::MatchingCallback_static(void *inContext,
                                              IOReturn inResult,
                                              void *inSender,
                                              IOHIDDeviceRef inIOHIDDeviceRef)
{
    assert (inContext != NULL);
    
    JoystickManager *context = (JoystickManager *)inContext;
    context->MatchingCallback(inContext, inResult, inSender, inIOHIDDeviceRef);
}

void 
JoystickManager::MatchingCallback(void *inContext,
                                  IOReturn inResult,
                                  void *inSender,
                                  IOHIDDeviceRef inIOHIDDeviceRef)
{
    // NSLog(@"%s",__PRETTY_FUNCTION__);

	IOHIDDeviceInfo devInfo = IOHIDDeviceInfo(inIOHIDDeviceRef);
    char *devInfoName = devInfo.GetName();

	if( inResult != kIOReturnSuccess )
	{
		NSLog(@"Device %p (%s) is not in successful state (%i)\n",
              inIOHIDDeviceRef, devInfoName ? devInfoName : "", inResult);
		return;
	}
    
    if(getJoystickProxyWithLocationID(devInfo.GetLocationID()) != NULL) {
        NSLog(@"Device %p (%s) already opend.\n",
              inIOHIDDeviceRef, devInfoName ? devInfoName : "");
        return;
	}
	
    if(usbjoy[0].pluggedIn && usbjoy[1].pluggedIn) {
		NSLog(@"Ignoring %s (%p): Maximum number of devices reached.\n",
              devInfoName ? devInfoName : "", inIOHIDDeviceRef);
		return;
	}
	
	IOReturn status;
	if( ( status = IOHIDDeviceOpen(inIOHIDDeviceRef, kIOHIDOptionsTypeNone ) ) != kIOReturnSuccess) {
		NSLog(@"Failed to open device %p (%s) (status = %i)\n",
              inIOHIDDeviceRef, devInfoName ? devInfoName : "", status );
		return;
	}
    
    // Prepare context information (will be passed to the callback functions)
    CallbackContext *context = (CallbackContext *)malloc(sizeof(CallbackContext));
    context->manager = this;
    context->locationID = devInfo.GetLocationID();
    context->deviceRef = inIOHIDDeviceRef;
    
    // Register callback functions
	IOHIDDeviceRegisterRemovalCallback( inIOHIDDeviceRef, RemoveCallback_static, (void *)context);
	IOHIDDeviceRegisterInputValueCallback( inIOHIDDeviceRef, InputValueCallback_static, (void *)context); 
    
    // Add proxy object to list of connected USB joysticks
    if (addJoystickProxyWithLocationID(devInfo.GetLocationID())) {
        [_proxy putMessage:MSG_JOYSTICK_ATTACHED];
        NSLog(@"Successfully opened device %s (ID %d)\n",
              devInfoName ? devInfoName : "", devInfo.GetLocationID());
    }
}

void 
JoystickManager::RemoveCallback_static(void *inContext, IOReturn inResult, void *inSender)
{
    assert (inContext != NULL);

    CallbackContext *context = (CallbackContext *)inContext;
    context->manager->RemoveCallback(inContext, inResult, inSender);
}

void 
JoystickManager::RemoveCallback(void *inContext, IOReturn inResult, void *inSender)
{
    USBJoystick *proxy;
    
    // NSLog(@"%s", __PRETTY_FUNCTION__);
    
	IOHIDDeviceInfo devInfo = IOHIDDeviceInfo((IOHIDDeviceRef)inSender);
    char *devInfoName = devInfo.GetName();
    
	if(inResult != kIOReturnSuccess) {
		NSLog(@"Device %s (ID %d) is not in successful state (%i)\n",
              devInfoName ? devInfoName : "", devInfo.GetLocationID(), inResult);
		return;
	}
    
    proxy = getJoystickProxyWithLocationID(devInfo.GetLocationID());
    
    if(proxy == NULL) {
		NSLog(@"Device %s (ID %d) not found in open list\n",
              devInfoName ? devInfoName : "", devInfo.GetLocationID());
		return;
	}
	
	IOHIDDeviceRegisterInputValueCallback((IOHIDDeviceRef)inSender, NULL, this);
	IOHIDDeviceRegisterRemovalCallback((IOHIDDeviceRef)inSender, NULL, this);
	IOHIDDeviceClose((IOHIDDeviceRef)inSender, kIOHIDOptionsTypeNone);

    removeJoystickProxyWithLocationID(devInfo.GetLocationID());
    [_proxy putMessage:MSG_JOYSTICK_REMOVED];

    NSLog(@"Successfully closed device %s (ID %d)\n",
          devInfoName ? devInfoName : "", devInfo.GetLocationID());
}

void 
JoystickManager::InputValueCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef )
{
    assert (inContext != NULL);

    CallbackContext *context = (CallbackContext *)inContext;
    context->manager->InputValueCallback(inContext, inResult, inSender, inIOHIDValueRef);
}

void 
JoystickManager::InputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef)
{
    CallbackContext *context = (CallbackContext *)inContext;

    // NSLog(@"%s", __PRETTY_FUNCTION__);

	if( inResult != kIOReturnSuccess )
	{
		NSLog(@"Device %p (ID %d) is not in successful state (%i)\n",
              context->deviceRef, context->locationID, inResult);
		return;
	}
	
    USBJoystick *proxy = getJoystickProxyWithLocationID(context->locationID);
    if (proxy == NULL) {
		NSLog(@"Device %p (ID %d) not found in open list\n",
              context->deviceRef, context->locationID);
		return;
	}
    
	IOHIDElementRef element = IOHIDValueGetElement(inIOHIDValueRef);
	IOHIDElementType elementType = IOHIDElementGetType(element);
	uint32_t elementPage = IOHIDElementGetUsagePage(element);
	uint32_t elementUsage = IOHIDElementGetUsage(element);
	
	if(elementType == kIOHIDElementTypeInput_Button) {
		if(elementPage == kHIDPage_Button) {
			// set values to conform to 0 and 1
			IOHIDElement_SetDoubleProperty(element, CFSTR(kIOHIDElementCalibrationMinKey), 0);
			IOHIDElement_SetDoubleProperty(element, CFSTR(kIOHIDElementCalibrationMaxKey), 1);
			IOHIDElement_SetDoubleProperty(element, CFSTR(kIOHIDElementCalibrationGranularityKey), 1);
			bool pressed = ( ceil( IOHIDValueGetScaledValue( inIOHIDValueRef, kIOHIDValueScaleTypeCalibrated ) ) == 1 );
			
			proxy->setButtonPressed(pressed);
        } else {
			NSLog(@"Device %p (ID %d) type and page mismatch (Type=%i, Page=%i)\n",
                  context->deviceRef, context->locationID, elementType, elementPage );
        }
	}
	else if((elementType == kIOHIDElementTypeInput_Axis) || (elementType == kIOHIDElementTypeInput_Misc /* why misc? */ ))
	{
		if( elementPage == kHIDPage_GenericDesktop )
		{
			// set values to conform to -1 / 0 / 1
			IOHIDElement_SetDoubleProperty( element, CFSTR(kIOHIDElementCalibrationMinKey), -1 );
			IOHIDElement_SetDoubleProperty( element, CFSTR(kIOHIDElementCalibrationMaxKey), 1 );
			IOHIDElement_SetDoubleProperty( element, CFSTR(kIOHIDElementCalibrationGranularityKey), 1 );
			int axis = ceil( IOHIDValueGetScaledValue( inIOHIDValueRef, kIOHIDValueScaleTypeCalibrated ) );
			
			switch(elementUsage) {

                case kHIDUsage_GD_X:
                    
                    proxy->setAxisX(axis == -1 ? JOYSTICK_LEFT :
                                    (axis == 1 ? JOYSTICK_RIGHT : JOYSTICK_RELEASED));
					break;

                case kHIDUsage_GD_Y:
                                       
                    proxy->setAxisY(axis == -1 ? JOYSTICK_UP :
                                    (axis == 1 ? JOYSTICK_DOWN : JOYSTICK_RELEASED));
                    break;

                default:
					NSLog(@"Device %p (ID %d) page and page usage mismatch (Type=%i, Page=%i)\n",
                          context->deviceRef, context->locationID, elementPage, elementUsage);
			}
		}
		else
			NSLog(@"Device %p (ID %d) type and page mismatch (Type=%i, Page=%i)\n",
                  context->deviceRef, context->locationID, elementType, elementPage);
	}
}

void 
JoystickManager::IOHIDElement_SetDoubleProperty(IOHIDElementRef element, CFStringRef key, double value)
{
	CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &value);
    if (number)
	{
        IOHIDElementSetProperty(element, key, number);
        CFRelease(number);
    }
}


