/*
 * (C) 2009 - 2015 Benjamin Klein, Dirk Hoffmann. All rights reserved.
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

using std::make_pair; // DO WE NEED THIS?

// ---------------------------------------------------------------------------------------------
//                                             JoystickManagerProxy
// ---------------------------------------------------------------------------------------------

JoystickManagerProxy::JoystickManagerProxy()
{
    _joystick = NULL;
    
}

void JoystickManagerProxy::bindJoystick(Joystick *joy)
{
    _joystick = joy;
}

void JoystickManagerProxy::ChangeButton(int index, bool pressed)
{
    bool found = (_pressedButtons.find(index) != _pressedButtons.end());

    if(pressed) {
        if(!found) _pressedButtons.insert(index);
    } else {
        if (found) _pressedButtons.erase(index);
    }

    if (_joystick)
        _joystick->SetButtonPressed(_pressedButtons.size() != 0);
}

void JoystickManagerProxy::ChangeAxisX(JoystickAxisState state) const
{
    if (_joystick)
        _joystick->SetAxisX(state);
}

void JoystickManagerProxy::ChangeAxisY(JoystickAxisState state) const
{
    if (_joystick)
        _joystick->SetAxisY(state);
}

// ---------------------------------------------------------------------------------------------
//                                             JoystickManager
// ---------------------------------------------------------------------------------------------

const int JoystickManager::UsageToSearch[][2] =
{
	/* Page */						/* Usage (0 for none ) */
	{	kHIDPage_GenericDesktop,	kHIDUsage_GD_Joystick	},
	{	kHIDPage_GenericDesktop,	kHIDUsage_GD_GamePad	},
};
const unsigned JoystickManager::MaxJoystickCount = 2;

JoystickManager::JoystickManager(C64Proxy *proxy)
{
    _proxy = proxy;
    _manager = NULL;
    locationID1 = 0;
    locationID2 = 0;
    proxy1 = NULL;
    proxy2 = NULL;
}

JoystickManager::~JoystickManager()
{
    Dispose();
}

bool JoystickManager::joystickIsPluggedIn(int nr)
{
    assert (nr >= 1 && nr <= 2);
    
    if (nr == 1) return (proxy1 != NULL);
    if (nr == 2) return (proxy2 != NULL);
    return false;
}

void JoystickManager::bindJoystick(int nr, Joystick *joy)
{
    assert (nr >= 1 && nr <= 2);

    if (nr == 1 && proxy1 != NULL) {
        
        proxy1->bindJoystick(joy);

        if (joy == NULL)
            fprintf(stderr, "Remove binding for first USB joystick\n");
        else
            fprintf(stderr, "Bind first USB joystick to %p\n", joy);
    }
    if (nr == 2 && proxy2 != NULL) {

        proxy2->bindJoystick(joy);

        if (joy == NULL)
            fprintf(stderr, "Remove binding for second USB joystick\n");
        else
            fprintf(stderr, "Bind second USB joystick to %p\n", joy);
    }
}

void JoystickManager::addJoystickProxyWithLocationID(int locationID, JoystickManagerProxy *proxy)
{
    if (proxy1 == NULL) {
        locationID1 = locationID;
        proxy1 = proxy;
        return;
    }
    if (proxy2 == NULL) {
        locationID2 = locationID;
        proxy2 = proxy;
        return;
    }
}

JoystickManagerProxy *JoystickManager::getJoystickProxyWithLocationID(int locationID)
{
    if (locationID1 == locationID) {
        assert(proxy1 != NULL);
        return proxy1;
    }
    if (locationID2 == locationID) {
        assert(proxy2 != NULL);
        return proxy2;
    }
    return NULL;
}

void JoystickManager::removeJoystickProxyWithLocationID(int locationID)
{
    if (locationID1 == locationID) {
        locationID1 = 0;
        // delete proxy1; MEMORY LEAK?
        proxy1 = NULL;
        return;
    }
    if (locationID2 == locationID) {
        locationID2 = 0;
        // delete proxy2; MEMORY LEAK?
        proxy2 = NULL;
        return;
    }
}

void JoystickManager::listJoystickManagers()
{
    NSLog(@"Slot 1: %p (ID %d)", proxy1, locationID1);
    NSLog(@"Slot 2: %p (ID %d)", proxy2, locationID2);
}

bool JoystickManager::Initialize()
{
    // NSLog(@"JoystickManager::Initialize");
    // NSLog(@"%s", __PRETTY_FUNCTION__);
    
	CFMutableArrayRef matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

    if( !matchingArray ) {
		NSLog(@"Cannot create mutable array");
		return false;
	}
    
	for(unsigned n = 0; n < sizeof( UsageToSearch ) / sizeof( UsageToSearch[0]); n++) {
		CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

        if(!dict) {
			NSLog( @"Cannot create mutable dictionary");
			CFRelease( matchingArray );
			return false;
		}
		
		CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &UsageToSearch[n][0]);

        if(!number) {
			NSLog(@"Cannot create CFNumberCreate");
			CFRelease(matchingArray);
			CFRelease(dict);
			return false;	
		}
        
		CFDictionarySetValue(dict, CFSTR( kIOHIDDeviceUsagePageKey), number);
		CFRelease(number);
		
		if(UsageToSearch[n][1]) {
			number = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &UsageToSearch[n][1]);

            if( !number ) {
                NSLog(@"Cannot create CFNumberCreate");
				CFRelease(matchingArray);
				CFRelease(dict);
				return false;	
			}
            
			CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), number);
			CFRelease(number);
		}
		
		CFArrayAppendValue(matchingArray, dict);
		CFRelease(dict);
	}
	
	_manager = IOHIDManagerCreate(kCFAllocatorDefault, 0);

    if(!_manager) {
		NSLog(@"Cannot create HIDManager");
		CFRelease(matchingArray);
		return false;
	}
	
	IOHIDManagerSetDeviceMatchingMultiple(_manager, matchingArray);
	CFRelease(matchingArray);
	IOHIDManagerRegisterDeviceMatchingCallback(_manager, MatchingCallback_static, this);
	IOHIDManagerScheduleWithRunLoop(_manager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
	
	IOReturn status = IOHIDManagerOpen(_manager, kIOHIDOptionsTypeNone);

    if(status != kIOReturnSuccess) {
		NSLog(@"Failed to open HIDManager (status = %i)", status);
		return false;
	}
    
    [_proxy putMessage:MSG_JOYSTICK_REMOVED];
    NSLog(@"HID manager is initialized");
    return true;
}

void JoystickManager::Dispose()
{
    // NSLog(@"%s", __PRETTY_FUNCTION__);

	IOHIDManagerClose(_manager, kIOHIDOptionsTypeNone);
	CFRelease(_manager);
}

void JoystickManager::MatchingCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef)
{
    assert (inContext != NULL);
    
    JoystickManager *context = (JoystickManager *)inContext;
    context->MatchingCallback( inContext, inResult, inSender, inIOHIDDeviceRef);
}

void 
JoystickManager::MatchingCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef)
{
    // NSLog(@"%s",__PRETTY_FUNCTION__);

	IOHIDDeviceInfo devInfo = IOHIDDeviceInfo(inIOHIDDeviceRef);
	
	if( inResult != kIOReturnSuccess )
	{
		NSLog(@"Device %p (%s) is not in successful state (%i)\n",
              inIOHIDDeviceRef, devInfo.GetName(), inResult);
		return;
	}
	
    if(getJoystickProxyWithLocationID(devInfo.GetLocationID()) != NULL) {
        NSLog(@"Device %p (%s) already opend.\n",
              inIOHIDDeviceRef, devInfo.GetName());
        return;
	}
	
    if(proxy1 != NULL && proxy2 != NULL) {
		NSLog(@"Ignoring %s (%p): Maximum number of devices reached.\n",
              devInfo.GetName(), inIOHIDDeviceRef);
		return;
	}
	
	IOReturn status;
	if( ( status = IOHIDDeviceOpen(inIOHIDDeviceRef, kIOHIDOptionsTypeNone ) ) != kIOReturnSuccess) {
		NSLog(@"Failed to open device %p (%s) (status = %i)\n",
              inIOHIDDeviceRef, devInfo.GetName(), status );
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
    addJoystickProxyWithLocationID(devInfo.GetLocationID(), new JoystickManagerProxy());
    [_proxy putMessage:MSG_JOYSTICK_ATTACHED];
    
	NSLog(@"Successfully opened device %s (ID %d)\n",
          devInfo.GetName(), devInfo.GetLocationID());
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
    JoystickManagerProxy *proxy;
    
    // NSLog(@"%s", __PRETTY_FUNCTION__);
    
	IOHIDDeviceInfo devInfo = IOHIDDeviceInfo((IOHIDDeviceRef)inSender);
	    
	if(inResult != kIOReturnSuccess) {
		NSLog(@"Device %s (ID %d) is not in successful state (%i)\n",
              devInfo.GetName(), devInfo.GetLocationID(), inResult);
		return;
	}
    
    proxy = getJoystickProxyWithLocationID(devInfo.GetLocationID());
    
    if(proxy == NULL) {
		NSLog(@"Device %s (ID %d) not found in open list\n",
              devInfo.GetName(), devInfo.GetLocationID());
		return;
	}
	
	IOHIDDeviceRegisterInputValueCallback((IOHIDDeviceRef)inSender, NULL, this);
	IOHIDDeviceRegisterRemovalCallback((IOHIDDeviceRef)inSender, NULL, this);
	IOHIDDeviceClose((IOHIDDeviceRef)inSender, kIOHIDOptionsTypeNone);

    removeJoystickProxyWithLocationID(devInfo.GetLocationID());
    [_proxy putMessage:MSG_JOYSTICK_REMOVED];

    NSLog(@"Successfully closed device %s (ID %d)\n",
          devInfo.GetName(), devInfo.GetLocationID());
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
	
    JoystickManagerProxy *proxy = getJoystickProxyWithLocationID(context->locationID);
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
			
			proxy->ChangeButton(elementUsage, pressed);
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
			IOHIDElement_SetDoubleProperty( element, CFSTR( kIOHIDElementCalibrationMinKey ), -1 );
			IOHIDElement_SetDoubleProperty( element, CFSTR( kIOHIDElementCalibrationMaxKey ), 1 );
			IOHIDElement_SetDoubleProperty( element, CFSTR( kIOHIDElementCalibrationGranularityKey ), 1 );
			int axis = ceil( IOHIDValueGetScaledValue( inIOHIDValueRef, kIOHIDValueScaleTypeCalibrated ) );
			
			switch( elementUsage )
			{
				case kHIDUsage_GD_X:
					if( axis == -1 )
						proxy->ChangeAxisX(JOYSTICK_AXIS_X_LEFT);
					else if( axis == 1 )
						proxy->ChangeAxisX(JOYSTICK_AXIS_X_RIGHT);
					else
						proxy->ChangeAxisX(JOYSTICK_AXIS_X_NONE);
					break;
				case kHIDUsage_GD_Y:
					if( axis == -1 )
						proxy->ChangeAxisY(JOYSTICK_AXIS_Y_UP);
					else if( axis == 1 )
						proxy->ChangeAxisY(JOYSTICK_AXIS_Y_DOWN);
					else
						proxy->ChangeAxisY(JOYSTICK_AXIS_Y_NONE);
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
	if(copy._name)
	{
		size_t len = strlen(copy._name);
		_name = new char[len + 1];
		strncpy(_name, copy._name, len);
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
