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

#import <Cocoa/Cocoa.h>
#import <QuickTime/QuickTime.h>

#define LIGHT_X_TAG 0
#define THETA_TAG   1
#define RADIUS_TAG  2

// Forward declaration
@class MyController;

// Keyboard constants (US MacBook Pro)
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
const uint16_t MAC_Y = 16;
const uint16_t MAC_Z = 6;
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
const uint16_t MAC_APO = 39;
const uint16_t MAC_DEL = 51;
const uint16_t MAC_RET = 36;
const uint16_t MAC_CL = 123;
const uint16_t MAC_CR = 124;
const uint16_t MAC_CU = 126;
const uint16_t MAC_CD = 125;
const uint16_t MAC_PLS = 24;
const uint16_t MAC_MNS = 27;
const uint16_t MAC_MUL = 200; 
const uint16_t MAC_DIV = 44; 
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

const int BG_TEXTURE_WIDTH = 1024;
const int BG_TEXTURE_HEIGHT= 512;
const int BG_TEXTURE_DEPTH = 4;

@interface MyOpenGLView : NSOpenGLView
{
	IBOutlet MyController *controller;
	
	C64 *c64;
    NSBitmapImageRep *theImage;
	NSOpenGLContext *glcontext;
	CVDisplayLinkRef displayLink;
	NSRecursiveLock *lock;

	bool emulateJoystick1;
	bool emulateJoystick2;
	float currentXAngle, targetXAngle, deltaXAngle;
	float currentYAngle, targetYAngle, deltaYAngle;
	float currentZAngle, targetZAngle, deltaZAngle;
	float currentEyeX, targetEyeX, deltaEyeX;
	float currentEyeY, targetEyeY, deltaEyeY;
	float currentEyeZ, targetEyeZ, deltaEyeZ;
	
	int frames;

	//! If false, the C64 screen is not drawn (background will be visible)
	bool drawC64texture;

	//! If false, only the front facing part of the texture cube is drawn
	bool drawEntireCube;
	
	//! Usually set to true. If set to false, OpenGL drawing is disabled
	/*! Setting the variable to false only makes sense during performance tests */
	bool enableOpenGL;
	
	// Mapping from Mac keycode to the C64 row/column format
	uint16_t kb[256];

	// View point
	// DEPRECATED
	// float eyeX, eyeY, eyeZ;

	// Texture cut-out (Visible texture area)
	// TODO: Rename
	float TEX_LEFT;
	float TEX_RIGHT;
	float TEX_TOP;
	float TEX_BOTTOM;
	
	// Size of drawn rectangle 
	float dimX, dimY;
	
	// Textures
	uint8_t data[TEXTURE_WIDTH * TEXTURE_HEIGHT * TEXTURE_DEPTH];
	GLuint texture;   // C64 screen
	GLuint bgTexture; // Background	
}

@property C64 *c64;
@property (readonly) int frames;
@property bool enableOpenGL;

- (void) cleanUp;

#pragma mark Animation

//! Returns true if view is currently drawing animation effects
- (bool) animates;

- (void)setEyeX:(float)newX;
- (void)setEyeY:(float)newY;
- (void)setEyeZ:(float)newZ;

//! Trigger animation effect. Move smoothly to specified location
// - (void)moveToX:(float)newX;
// - (void)moveToY:(float)newY;
// - (void)moveToZ:(float)newZ;

//! Trigger animation effect. Zooms in from far away
- (void) zoom;

//! Trigger animation effect. Scroll in from below
- (void) scroll;

//! Trigger animation effect. Rotate cube in one direction
- (void) rotate;

//! Trigger animation effect. Rotate cube in other direction
- (void) rotateBack;


//! Compute geometry parameters for next animation cycle
- (void) updateAngles;


#pragma mark Full screen mode

- (void) setFullscreenMode:(bool)b;
- (void) toggleFullscreenMode;

#pragma mark Drawing

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;
- (void) drawC64texture:(bool)value;

#pragma mark Graphics conversion

- (NSImage *)screenshot;
- (NSImage *) flipImage: (NSImage *)image;
- (NSImage *) expandImage: (NSImage *)image toSize:(NSSize) size;
- (NSImage *) extendImage: (NSImage *)image toSize:(NSSize) size;
- (int) makeTexture:(NSImage *)img;

@end
