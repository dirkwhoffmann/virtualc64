/*
 * (C) 2009 Benjamin Klein. All rights reserved.
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

#ifndef __JOYSTICK_MANAGER_H__
#define __JYOSTICK_MANAGER_H__

#include "C64.h"
#include "C64Proxy.h"
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <map>
#include <set>
using std::map;
using std::set;

class JoystickProxy
{
	public:
		JoystickProxy();
		JoystickProxy( Joystick *joystick );
		
		void ChangeButton( int index, bool pressed );
		void ChangeAxisX( JoystickAxisState state ) const;
		void ChangeAxisY( JoystickAxisState state ) const;
		
		Joystick *GetJoystick() const;
		
	private:
		Joystick *_joystick;
		set<int> _pressedButtons;
};

class JoystickManager 
{
	public:
		JoystickManager( C64Proxy *proxy );
		~JoystickManager();
		
		bool Initialize();
		void Dispose();
		
		static void MatchingCallback_static( void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef );
		void MatchingCallback( void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef );
		
		static void RemoveCallback_static( void *inContext, IOReturn inResult, void *inSender );
		void RemoveCallback( void *inContext, IOReturn inResult, void *inSenderf );
		
		static void InputValueCallback_static( void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef );
		void InputValueCallback( void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef );

	private:
		void IOHIDElement_SetDoubleProperty( IOHIDElementRef element, CFStringRef key, double value );
		
		C64Proxy *_proxy;
		
		bool _initialized;
		IOHIDManagerRef _manager;
		map<int, JoystickProxy> _joysticks;
		
		static const int UsageToSearch[][ 2 ];
		static const unsigned MaxJoystickCount;
};

class IOHIDDeviceInfo
{
	public:
		IOHIDDeviceInfo( IOHIDDeviceRef device );
		IOHIDDeviceInfo( const IOHIDDeviceInfo &copy );
		~IOHIDDeviceInfo();
		
		int GetLocationID() const;
		const char *GetName() const;
		
	private:
		int _locationID;
		char *_name;
};

#endif