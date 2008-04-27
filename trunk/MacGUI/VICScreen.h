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

#ifndef VICSCREEN_INC
#define VICSCREEN_INC

#include "C64.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/glu.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>

#define LIGHT_X_TAG 0
#define THETA_TAG   1
#define RADIUS_TAG  2

// Forward declaration
@class MyDocument;

// Keyboard constants
const uint16_t MAC_A = 0;
const uint16_t MAC_B = 11;
const uint16_t MAC_C = 8;
const uint16_t MAC_D = 2;
const uint16_t MAC_E = 14;
const uint16_t MAC_F = 3;
const uint16_t MAC_G = 5;
const uint16_t MAC_H = 4;
const uint16_t MAC_I = 34;
const uint16_t MAC_J = 38;
const uint16_t MAC_K = 40;
const uint16_t MAC_L = 37;
const uint16_t MAC_M = 46;
const uint16_t MAC_N = 45;
const uint16_t MAC_O = 31;
const uint16_t MAC_P = 35;
const uint16_t MAC_Q = 12;
const uint16_t MAC_R = 15;
const uint16_t MAC_S = 1;
const uint16_t MAC_T = 17;
const uint16_t MAC_U = 32;
const uint16_t MAC_V = 9;
const uint16_t MAC_W = 13;
const uint16_t MAC_X = 7;
const uint16_t MAC_Y = 6;
const uint16_t MAC_Z = 16;
const uint16_t MAC_F1 = 122;
const uint16_t MAC_F2 = 120;
const uint16_t MAC_F3 = 99;
const uint16_t MAC_F4 = 118;
const uint16_t MAC_F5 = 96;
const uint16_t MAC_F6 = 97;
const uint16_t MAC_F7 = 98;
const uint16_t MAC_F8 = 100;
const uint16_t MAC_1 = 18;
const uint16_t MAC_2 = 19;
const uint16_t MAC_3 = 20;
const uint16_t MAC_4 = 21;
const uint16_t MAC_5 = 23;
const uint16_t MAC_6 = 22;
const uint16_t MAC_7 = 26;
const uint16_t MAC_8 = 28;
const uint16_t MAC_9 = 25;
const uint16_t MAC_0 = 29;
const uint16_t MAC_APO = 24;
const uint16_t MAC_DEL = 51;
const uint16_t MAC_RET = 36;
const uint16_t MAC_CL = 123;
const uint16_t MAC_CR = 124;
const uint16_t MAC_CU = 126;
const uint16_t MAC_CD = 125;
const uint16_t MAC_PLS = 30;
const uint16_t MAC_MNS = 44;
const uint16_t MAC_MUL = 200; 
const uint16_t MAC_DIV = 24; 
const uint16_t MAC_DOT = 47;
const uint16_t MAC_COM = 43;
const uint16_t MAC_SEM = 41;
const uint16_t MAC_SPC = 49;
const uint16_t MAC_ESC = 53;
const uint16_t MAC_HAT = 10;
const uint16_t MAC_CMP = 50; // ??? 53;

// Graphics constants
const int TEXTURE_WIDTH = 512;
const int TEXTURE_HEIGHT= 512;
const int TEXTURE_DEPTH = 4;

const int BG_TEXTURE_WIDTH = 1024; // 512;
const int BG_TEXTURE_HEIGHT= 512; // 512;
const int BG_TEXTURE_DEPTH = 4;

@interface VICScreen : NSOpenGLView
{
	IBOutlet MyDocument *myDoc;
	C64 *c64;
    NSBitmapImageRep *theImage;
	NSOpenGLContext *glcontext;
	
	/* C64 screen texture data */
	uint8_t data[TEXTURE_WIDTH * TEXTURE_HEIGHT * TEXTURE_DEPTH];

	/* Background image texture data */
	//uint32_t bg_data[BG_TEXTURE_HEIGHT][BG_TEXTURE_WIDTH][BG_TEXTURE_DEPTH];

	int *screenBuffer;
	bool highlighted;
	bool emulateJoystick1;
	bool emulateJoystick2;
	float currentXAngle, targetXAngle, deltaX;
	float currentYAngle, targetYAngle, deltaY;
	float currentZAngle, targetZAngle, deltaZ;
	float currentDistance, targetDistance, deltaDistance;
	float currentZoffset, targetZoffeset, deltaOffset;
	int frames;
	int readyToDraw;
	
	// Texture storage
	GLuint texture[2];
	
	// Mapping from Mac keycode to the C64 row/column format
	uint16_t kb[256];

}

- (void) prepare;
- (void) setC64:(C64 *)c64;
- (void) startAnimation;
- (void) stopAnimation;
- (void) toggleAnimation;
- (void) zoom;
- (void) rotate;
- (void) rotateBack;
// - (void) tiltOn;
// - (void) tiltOff;
- (int) getFrames;
- (void) updateAngles;
- (int) isReadyToDraw;
- (void) setReadyToDraw:(int)b;
- (void) updateTexture:(int *)screenBuffer;
- (bool)loadBackgroundTexture:(uint8_t *)texture_data;
- (NSImage *)screenshot;

@end

#endif
