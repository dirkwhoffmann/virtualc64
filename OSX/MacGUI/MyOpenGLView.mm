/*
 * (C) 2006 - 2008 Dirk W. Hoffmann. All rights reserved.
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

#import "C64GUI.h"

// Background texture
const float BG_TEX_LEFT   = 0.0; 
const float BG_TEX_RIGHT  = 1.0; // 642.0 / BG_TEXTURE_WIDTH;
const float BG_TEX_TOP    = 0.0; 
const float BG_TEX_BOTTOM = 1.0; // 482.0 / BG_TEXTURE_HEIGHT;


static CVReturn MyRenderCallback(CVDisplayLinkRef displayLink, 
								 const CVTimeStamp *inNow, 
								 const CVTimeStamp *inOutputTime, 
								 CVOptionFlags flagsIn, 
								 CVOptionFlags *flagsOut, 
                                 void *displayLinkContext)
{
    @autoreleasepool {
        
        return [(__bridge MyOpenGLView *)displayLinkContext getFrameForTime:inOutputTime flagsOut:flagsOut];
    
    }
}

void checkForOpenGLErrors()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		NSLog(@"OpenGL PANIC: %s", gluErrorString(error));
		exit(0);
	}
}

@implementation MyOpenGLView

@synthesize c64, frames, enableOpenGL, drawIn3D, drawC64texture, drawBackground, drawEntireCube;

// --------------------------------------------------------------------------------
//                                  Initializiation
// --------------------------------------------------------------------------------

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pixFmt
{
	// NSLog(@"MyOpenGLView::initWithFrame");
	
	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt]) == nil) {
		NSLog(@"ERROR: Can't initiaize OpenGLView");
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)c
{
	NSLog(@"MyOpenGLView::initWithCoder");

	if ((self = [super initWithCoder:c]) == nil) {
		NSLog(@"ERROR: Can't initiaize OpenGLView");
	}
		return self;
}

-(void)awakeFromNib
{
	// NSLog(@"MyOpenGLView::awakeFromNib");
	    
	// DEPRECATED
	c64 = [c64proxy c64];
	
	// Create lock used by the draw method
	lock = [NSRecursiveLock new];
		
	// Set initial scene position and drawing properties
	targetXAngle = targetYAngle = targetZAngle = 0;
	deltaXAngle = deltaYAngle = deltaZAngle = 0;
	currentEyeX = currentEyeY = currentEyeZ = 0;
	deltaEyeX = deltaEyeY = deltaEyeZ = 0;
	drawC64texture = false;
	drawBackground = true;
	drawEntireCube = false;
	
	// Core video and graphics stuff
	displayLink = nil;
	frames = 0;
	enableOpenGL = true;
    drawInEntireWindow = false;
    drawIn3D = true;
    
	// Keyboard initialization
    for (int i = 0; i < 256; i++) {
        pressedKeys[i] = 0;
    }
    
	// Register for drag and drop
	[self registerForDraggedTypes:
	 [NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];
}

- (void) dealloc 
{
	[self cleanUp];
}

-(void)cleanUp
{    
	// NSLog(@"Deallocating OpenGL ressources");
	
	// release display link
    if (displayLink) {
    	CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
    
    if (lock) {
        lock = nil;
    }
}

- (void)prepareOpenGL
{
	NSLog(@"MyOpenGLView::prepareOpenGL");
	
	// Set up context
	glcontext = [self openGLContext];
	assert(glcontext != NULL);
	[glcontext makeCurrentContext];
	
	// Configure the view
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); 
	checkForOpenGLErrors();

	// Disable everything we don't need
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	
	// Create C64 monitor texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, (GLuint *)&texture);
	checkForOpenGLErrors();
	assert(texture > 0);
	
	glBindTexture(GL_TEXTURE_2D, texture);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    [c64proxy setVideoFilter:[c64proxy videoFilter]];
    glTexImage2D(GL_TEXTURE_2D, 0, 4, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	checkForOpenGLErrors();
	
	// Create background texture
	NSURL *url = [[NSWorkspace sharedWorkspace] desktopImageURLForScreen:[NSScreen mainScreen]];
	NSImage *bgImage = [[NSImage alloc] initWithContentsOfURL:url];
	NSImage *bgImageResized = [self expandImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
	bgTexture = [self makeTexture:bgImageResized];
	checkForOpenGLErrors();
	
	// Turn on synchronization
	const GLint VBL = 1;
	[[self openGLContext] setValues:&VBL forParameter:NSOpenGLCPSwapInterval];
	checkForOpenGLErrors();
	
    // Create display link for the main display
    CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
	checkForOpenGLErrors();
	
    if (displayLink != NULL) {
		CVReturn success;
    	// Set the current display of a display link
    	if ((success = CVDisplayLinkSetCurrentCGDisplay(displayLink, kCGDirectMainDisplay)) != 0) {
			NSLog(@"CVDisplayLinkSetCurrentCGDisplay failed with return code %d", success);
			CVDisplayLinkRelease(displayLink);
			exit(0);
		}
        
        // Set the renderer output callback function
        if ((success = CVDisplayLinkSetOutputCallback(displayLink, &MyRenderCallback, (__bridge void *)self)) != 0) {
			NSLog(@"CVDisplayLinkSetOutputCallback failed with return code %d", success);
  	        CVDisplayLinkRelease(displayLink);
			exit(0);
		}
        
        // Activates display link
    	if ((success = CVDisplayLinkStart(displayLink)) != 0) {
			NSLog(@"CVDisplayLinkStart failed with return code %d", success);
		        CVDisplayLinkRelease(displayLink);
			exit(0);				
		}
        
        // Determine refresh rate (currently unused)
        CVTime nominal = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(displayLink);
        if (nominal.flags & kCVTimeIsIndefinite) {
            NSLog(@"CVDisplayLink: Cannot determine your machines video refresh rate");
            refreshRate = 0.0;
        } else {
            refreshRate = (double)nominal.timeScale / (double)nominal.timeValue;
            NSLog(@"CVDisplayLink: OpenGL will refresh with a rate of %.2f Hz", refreshRate);
        }
    }
}

// --------------------------------------------------------------------------------
//                               Animation effects
// --------------------------------------------------------------------------------

- (bool)animates
{	
	return (currentXAngle != targetXAngle || currentYAngle != targetYAngle || currentZAngle != targetZAngle || 		
			currentEyeX != targetEyeX || currentEyeY != targetEyeY || currentEyeZ != targetEyeZ);
}

- (float)eyeX
{
    return currentEyeX;
}

- (void)setEyeX:(float)newX
{
	currentEyeX = targetEyeX = newX;
}

- (float)eyeY
{
    return currentEyeY;
}

- (void)setEyeY:(float)newY
{
	currentEyeY = targetEyeY = newY;
}

- (float)eyeZ
{
    return currentEyeZ;
}

- (void)setEyeZ:(float)newZ
{
    currentEyeZ = targetEyeZ = newZ;
}

- (void)setVideoFilter:(unsigned)filter
{    
    // Set up context
    glcontext = [self openGLContext];
    assert(glcontext != NULL);
    [glcontext makeCurrentContext];
    
    glBindTexture(GL_TEXTURE_2D, texture);

    switch (filter) {
        case GLFILTER_NONE:
            NSLog(@"Setting video filter to GLFILTER_NONE\n");
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
            
        default:
            NSLog(@"Setting default video filter\n");
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
    }

    checkForOpenGLErrors();
}

- (bool)drawInEntireWindow
{
    return drawInEntireWindow;
}

- (void)setDrawInEntireWindow:(bool)b
{
    /*
    NSRect newRect = [self frame];
    NSLog(@"X: %f Y: %f W: %f H: %f\n", newRect.origin.x, newRect.origin.y, newRect.size.width, newRect.size.height);

    NSRect wrect = [[controller window] frame];
    NSLog(@"X: %f Y: %f W: %f H: %f\n", wrect.origin.x, wrect.origin.y, wrect.size.width, wrect.size.height);
    */
    
    if (drawInEntireWindow == b)
        return;
    
    NSRect r = self.frame;
    float borderThickness;
    if (b) {
        NSLog(@"Expanding OpenGL view");
        r.origin.y -= 24;
        r.size.height += 24;
        borderThickness = 0.0;
    } else {
        NSLog(@"Shrinking OpenGL view");
        r.origin.y += 24;
        r.size.height -= 24;
        borderThickness = 22.0;
    }
    
    self.frame = r;
    [[self window] setContentBorderThickness:borderThickness forEdge: NSMinYEdge];
    drawInEntireWindow = b;
}

- (int)fingerprintForKey:(int)keycode withModifierFlags:(int)flags
{
    // The recorded fingerprint consists of the keycode. If the key is a number key (0 - 9), the
    // fingerprint also contains the NSNumericPadKeyMask flag to distinguish keys from the
    // numeric keypad from "normal" keys.
    
    flags &= NSNumericPadKeyMask;
    
    switch (keycode) {
        case kVK_ANSI_1: case kVK_ANSI_2: case kVK_ANSI_3: case kVK_ANSI_4: case kVK_ANSI_5:
        case kVK_ANSI_6: case kVK_ANSI_7: case kVK_ANSI_8: case kVK_ANSI_9: case kVK_ANSI_0:
            flags &= NSNumericPadKeyMask; // keep NSNumericPadKeyMask flag
            
        default:
            flags = 0; // standard case: we only keep the keycode
     }
    
    return keycode | flags;
}

- (int)joyKeycode:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);

    switch (nr) {
        case 1: return joyKeycode[0][dir];
        case 2: return joyKeycode[1][dir];
    }
    
    assert(0);
    return 0;
}

- (char)joyChar:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: return joyChar[0][dir];
        case 2: return joyChar[1][dir];
    }
    
    assert(0);
    return 0;
}

- (void)setJoyKeycode:(int)keycode keymap:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);

    switch (nr) {
        case 1: joyKeycode[0][dir] = keycode; return;
        case 2: joyKeycode[1][dir] = keycode; return;
    }

    assert(0);
}

- (void)setJoyChar:(char)c keymap:(int)nr direction:(JoystickDirection)dir
{
    assert(dir >= 0 && dir <= 4);
    
    switch (nr) {
        case 1: joyChar[0][dir] = c; return;
        case 2: joyChar[1][dir] = c; return;
    }
    
    assert(0);
}

- (void)updateAngles
{
	if ([self animates]) {
	
		if (fabs(currentXAngle - targetXAngle) < fabs(deltaXAngle)) currentXAngle = targetXAngle;
		else														currentXAngle += deltaXAngle;

		if (fabs(currentYAngle - targetYAngle) < fabs(deltaYAngle)) currentYAngle = targetYAngle;
		else														currentYAngle += deltaYAngle;

		if (fabs(currentZAngle - targetZAngle) < fabs(deltaZAngle)) currentZAngle = targetZAngle;
		else														currentZAngle += deltaZAngle;

		if (fabs(currentEyeX - targetEyeX) < fabs(deltaEyeX))       currentEyeX   = targetEyeX;
		else														currentEyeX   += deltaEyeX;

		if (fabs(currentEyeY - targetEyeY) < fabs(deltaEyeY))       currentEyeY   = targetEyeY;
		else														currentEyeY   += deltaEyeY;

		if (fabs(currentEyeZ - targetEyeZ) < fabs(deltaEyeZ))       currentEyeZ   = targetEyeZ;
		else														currentEyeZ   += deltaEyeZ;
		
		if (currentXAngle >= 360.0) currentXAngle -= 360.0;
		if (currentXAngle < 0.0) currentXAngle += 360.0;
		if (currentYAngle >= 360.0) currentYAngle -= 360.0;	
		if (currentYAngle < 0.0) currentYAngle += 360.0;
		if (currentZAngle >= 360.0) currentZAngle -= 360.0;	
		if (currentZAngle < 0.0) currentZAngle += 360.0;

	} else {
		drawEntireCube = false;
	}
}

- (void)computeAnimationDeltaSteps:(int)animationCycles
{
	deltaXAngle = (targetXAngle - currentXAngle) / animationCycles;
	deltaYAngle = (targetYAngle - currentYAngle) / animationCycles;	
	deltaZAngle = (targetZAngle - currentZAngle) / animationCycles;	
	deltaEyeX = (targetEyeX - currentEyeX) / animationCycles;
	deltaEyeY = (targetEyeY - currentEyeY) / animationCycles;
	deltaEyeZ = (targetEyeZ - currentEyeZ) / animationCycles;
}

- (void)zoom
{
	NSLog(@"Zooming in...\n\n");

	currentEyeZ     = 6;
	targetXAngle    = 0;
	targetYAngle    = 0;
	targetZAngle    = 0;
	
	[self computeAnimationDeltaSteps:120 /* 2 sec */];
}

- (void)rotateBack
{
	NSLog(@"Rotating back...\n\n");

	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   += 90;
	
	[self computeAnimationDeltaSteps:60 /* 1 sec */];

	if (targetYAngle >= 360) 
		targetYAngle -= 360;

	drawEntireCube = true;
}

- (void)rotate
{
	NSLog(@"Rotating...\n\n");

	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   -= 90;
	drawEntireCube = true;
	
	[self computeAnimationDeltaSteps:60 /* 1 sec */];

	if (targetYAngle < 0) 
		targetYAngle += 360;
}

- (void)scroll
{
	NSLog(@"Scrolling...\n\n");
	
	currentEyeY    = 0.9;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;

	[self computeAnimationDeltaSteps:120];		
}

- (void)fadeIn
{
	NSLog(@"Fading in...\n\n");
	
	
	currentXAngle  = -90;
	currentEyeZ    = 5.0;
	
	currentEyeY    = 2.5;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;
	
	[self computeAnimationDeltaSteps:120];	
}

#if 0
- (void)moveToX:(float)newX
{
	targetEyeX = newX;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}

- (void)moveToY:(float)newY 
{
	targetEyeY = newY;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}

- (void)moveToZ:(float)newZ 
{
	targetEyeZ = newZ;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}
#endif


// --------------------------------------------------------------------------------
//                                    Graphics
// --------------------------------------------------------------------------------

- (NSImage *) flipImage:(NSImage *)image
{
	assert(image != nil);
	
	NSSize size = [image size];
	NSImage *newImage = [[NSImage alloc] initWithSize:size];

    if (image) {
        [NSGraphicsContext saveGraphicsState];
        [newImage lockFocus];

        NSAffineTransform* t = [NSAffineTransform transform];
        [t translateXBy:0 yBy:size.height];
        [t scaleXBy:1 yBy:-1];
        [t concat];
        
        [image drawInRect:NSMakeRect(0, 0, size.width,size.height)];

        [newImage unlockFocus];
        [NSGraphicsContext restoreGraphicsState];
    }
    
    return newImage;
}

- (NSImage *) expandImage:(NSImage *)image toSize:(NSSize)size
{
	NSImage *newImage = [[NSImage alloc] initWithSize:size];

	if (image) {
        [NSGraphicsContext saveGraphicsState];
        [newImage lockFocus];

        NSAffineTransform* t = [NSAffineTransform transform];
        [t translateXBy:0 yBy:size.height];
        [t scaleXBy:1 yBy:-1];
        [t concat];
        
        [image drawInRect:NSMakeRect(0,0,size.width,size.height)
                 fromRect:NSMakeRect(0,0,[image size].width, [image size].height) 
                operation:NSCompositeSourceOver fraction:1.0];
        
        [newImage unlockFocus];
        [NSGraphicsContext restoreGraphicsState];
    }
    
    return newImage;
}

- (int) makeTexture:(NSImage *)image
{
	assert(image != nil);

	unsigned int tid = 0;
	int texformat = GL_RGB;
	
	NSBitmapImageRep *imgBitmap = [[NSBitmapImageRep alloc] initWithData:[image TIFFRepresentation]];
	
	if ([imgBitmap samplesPerPixel] == 4)
		texformat = GL_RGBA;
	else if ([imgBitmap samplesPerPixel] == 3)
		texformat = GL_RGB;
	else if ([imgBitmap samplesPerPixel] == 2)
		texformat = GL_LUMINANCE_ALPHA;
	else if ([imgBitmap samplesPerPixel] == 1)
		texformat = GL_LUMINANCE;
	
	glGenTextures(1, (GLuint *)&tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA,
					  [imgBitmap pixelsWide], 
					  [imgBitmap pixelsHigh], 
					  texformat, 
					  GL_UNSIGNED_BYTE, 
					  [imgBitmap bitmapData]);
	return tid;
}

- (void)update
{
    [lock lock];
	[super update];
    [lock unlock];
}

- (void) reshape
{
	NSRect rect = [self bounds]; 
	// NSLog(@"%@ reshape (%d %d)", self, (int)rect.size.width, (int)rect.size.height);

	[glcontext makeCurrentContext];
	
	glViewport(0, 0, (int)rect.size.width, (int)rect.size.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60.0, rect.size.width/rect.size.height, 0.2, 7);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)timeStamp flagsOut:(CVOptionFlags*)flagsOut
{
	@autoreleasepool {
	
        // Update angles for screen animation
        [self updateAngles];
		
        // Draw scene
        [self drawRect:NSZeroRect];
                
		return kCVReturnSuccess;
	}
}

- (void)determineScreenGeometry
{
    if (c64->isPAL()) {
        // PAL border will be 36 pixels wide and 34 pixels heigh
        textureXStart = (float)(PAL_LEFT_BORDER_WIDTH - 36.0) / (float)TEXTURE_WIDTH;
        textureXEnd = (float)(PAL_LEFT_BORDER_WIDTH + PAL_CANVAS_WIDTH + 36.0) / (float)TEXTURE_WIDTH;
        textureYStart = (float)(PAL_UPPER_BORDER_HEIGHT - 34.0) / (float)TEXTURE_HEIGHT;
        textureYEnd = (float)(PAL_UPPER_BORDER_HEIGHT + PAL_CANVAS_HEIGHT + 34.0) / (float)TEXTURE_HEIGHT;
    } else {
        // NTSC border will be 42 pixels wide and 9 pixels heigh
        textureXStart = (float)(NTSC_LEFT_BORDER_WIDTH - 42.0) / (float)TEXTURE_WIDTH;
        textureXEnd = (float)(NTSC_LEFT_BORDER_WIDTH + NTSC_CANVAS_WIDTH + 42.0) / (float)TEXTURE_WIDTH;
        textureYStart = (float)(NTSC_UPPER_BORDER_HEIGHT - 9) / (float)TEXTURE_HEIGHT;
        textureYEnd = (float)(NTSC_UPPER_BORDER_HEIGHT + NTSC_CANVAS_HEIGHT + 9) / (float)TEXTURE_HEIGHT;
    }
    
    // Enable this for debugging (will display the whole texture)
    /*
    textureXStart = 0.0;
    textureXEnd = 1.0;
    textureYStart = 0.0;
    textureYEnd = 1.0;
    */
}

- (void)updateScreenTexture
{
	glBindTexture(GL_TEXTURE_2D, texture);			
	if (c64) {
		void *buf = c64->vic->screenBuffer(); 
		assert(buf != NULL);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NTSC_PIXELS, PAL_RASTERLINES, GL_RGBA, GL_UNSIGNED_BYTE, buf);
        checkForOpenGLErrors();
	}
}

- (void)drawRect2D:(NSRect)r
{
    const int XSize = 640;
    const int YSize = 480;

    [self determineScreenGeometry];
    
    [glcontext makeCurrentContext];
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, XSize, YSize, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);    
    glDisable(GL_DEPTH_TEST);
    
    // Clear screen and depth buffer
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    [self updateScreenTexture];
    
    glBindTexture(GL_TEXTURE_2D, texture);		
    glBegin(GL_QUADS);			
        glTexCoord2f(textureXEnd, textureYStart);
    glVertex2f(640,0);		// Top Right Of The Quad (Front)
    glTexCoord2f(textureXStart, textureYStart);
    glVertex2f(0,0);		// Top Left Of The Quad (Front)
    glTexCoord2f(textureXStart, textureYEnd);
    glVertex2f(0,480);		// Bottom Left Of The Quad (Front)
    glTexCoord2f(textureXEnd, textureYEnd);
    glVertex2f(640,480);	// Bottom Right Of The Quad (Front)
    glEnd();		
}

- (void)drawRect3D:(NSRect)r
{	
    [self determineScreenGeometry];
	
	[glcontext makeCurrentContext];

    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    [self updateScreenTexture];
	
	// Set location
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
				  
	// Set viewpoint
	// gluLookAt(0, 0, 1.33, 0, 0, 0, 0, 1, 0);
    gluLookAt(0, 0, 1.4, 0, 0, 0, 0, 1, 0);
    
	bool animation = [self animates];

	// bool drawBackground = animation || !drawC64texture;
	// bool drawBackground = !drawC64texture;
    // drawBackground = false;
    if (drawBackground) {
        float depth = -5.0f;
        float scale = 9.3f;
		glBindTexture(GL_TEXTURE_2D, bgTexture); 
		glBegin(GL_QUADS);		
		glTexCoord2f(BG_TEX_RIGHT, BG_TEX_TOP);
		glVertex3f(scale*0.64f, scale*-0.4f, depth); // Top right
		glTexCoord2f(BG_TEX_LEFT, BG_TEX_TOP);
		glVertex3f(scale*-0.64f, scale*-0.4f, depth); // Top left
		glTexCoord2f(BG_TEX_LEFT, BG_TEX_BOTTOM);
		glVertex3f(scale*-0.64f, scale*0.4f, depth); // Bottom left
		glTexCoord2f(BG_TEX_RIGHT, BG_TEX_BOTTOM);
		glVertex3f(scale*0.64f, scale*0.4f, depth); // Bottom right
		glEnd();		
	}

	// Zoom in or zoom out
	glTranslatef(-currentEyeX, -currentEyeY, -0.065306 - currentEyeZ);

	if (animation) {
		
		// Rotate around Z axis
		glRotatef(currentZAngle,0.0f,0.0f,1.0f);
		
		// Rotate around Y axis
		glRotatef(currentYAngle,0.0f,1.0f,0.0f);
		
		// Rotate around X axis
		glRotatef(currentXAngle,1.0f,0.0f,0.0f);

	}
	
	if (drawC64texture) {
        const float dimX = 0.64;
        const float dimY = 0.48;
        
		// If emulation is halted, we brighten up the display by adding some fog...
		if (c64->isHalted()) {
			GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
			glFogfv(GL_FOG_COLOR, fogColor);
			glFogi(GL_FOG_MODE, GL_EXP);
			glFogf(GL_FOG_DENSITY, 1.0f);				
			glHint(GL_FOG_HINT, GL_DONT_CARE);	
			glFogf(GL_FOG_START, 0.0f);	
			glFogf(GL_FOG_END, 2.0f);
			glEnable(GL_FOG);	
		} else {
			glDisable(GL_FOG);
		}		

		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);			
        
		// FRONT
		glTexCoord2f(textureXEnd, textureYStart);
		glVertex3f( dimX, dimY, dimX);		// Top Right Of The Quad (Front)
		glTexCoord2f(textureXStart, textureYStart);
		glVertex3f(-dimX, dimY, dimX);		// Top Left Of The Quad (Front)
		glTexCoord2f(textureXStart, textureYEnd);
		glVertex3f(-dimX,-dimY, dimX);		// Bottom Left Of The Quad (Front)
		glTexCoord2f(textureXEnd, textureYEnd);
		glVertex3f( dimX,-dimY, dimX);		// Bottom Right Of The Quad (Front)
	
		if (drawEntireCube) {
			
			// TOP
			glColor3f(1.0f,1.0f,1.0f);				// Set The Color
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Right (TOP)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Left (TOP)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX, dimY, dimX);		// Bottom Left (TOP)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX, dimY, dimX);		// Bottom Right (TOP)
				
			// BOTTOM
			glColor3f(1.0f,1.0f,1.0f);			    // Set The Color
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX,-dimY, dimX);		// Top Right (BOTTOM)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX,-dimY, dimX);		// Top Left (BOTTOM)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Left (BOTTOM)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);	    // Bottom right (BOTTOM)

			// BACK
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);		// Bottom Left Of The Quad (Back)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Right Of The Quad (Back)
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Right Of The Quad (Back)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Left Of The Quad (Back)
						  
			// LEFT
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f(-dimX, dimY, dimX);		// Top Right Of The Quad (Left)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Left Of The Quad (Left)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Left Of The Quad (Left)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f(-dimX,-dimY, dimX);		// Bottom Right Of The Quad (Left)
				
			// RIGHT
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Right Of The Quad (Right)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f( dimX, dimY, dimX);		// Top Left Of The Quad (Right)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f( dimX,-dimY, dimX);		// Bottom Left Of The Quad (Right)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);		// Bottom Right Of The Quad (Right)
		}
			
		glEnd();		
	}
}

- (void)drawRect:(NSRect)r
{	 
	if (!c64 || !enableOpenGL) 
		return;
	
	[lock lock];     
	frames++;

    // CGLLockContext([[self openGLContext] CGLContextObj]);
    
    if (drawIn3D) {
        [self drawRect3D:r];
    } else {
        [self drawRect2D:r];
    }

	// Flush screen
	glFinish();
	[glcontext flushBuffer];
    
    [lock unlock];
}

- (NSImage *)screenshot
{
	int height=(int)NSHeight([self visibleRect]);
	int width=(int)NSWidth([self visibleRect]);
	
	NSBitmapImageRep *imageRep;
	NSImage *image;
	
	imageRep=[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL
													  pixelsWide:width
													  pixelsHigh:height
												   bitsPerSample:8
												 samplesPerPixel:4
														hasAlpha:YES
														isPlanar:NO
												  colorSpaceName:NSCalibratedRGBColorSpace
													 bytesPerRow:width*4
													bitsPerPixel:0];
    
    [lock lock];

    [[self openGLContext] makeCurrentContext];
	glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,[imageRep bitmapData]);
	image=[[NSImage alloc] initWithSize:NSMakeSize(width,height)];
	[image addRepresentation:imageRep];

    [lock unlock];

    NSImage *screenshot = [self flipImage:image];
    return screenshot;
}

// --------------------------------------------------------------------------------
//                                  Keyboard events 
// --------------------------------------------------------------------------------

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)resignFirstResponder
{
	return YES;
}

- (BOOL)becomeFirstResonder 
{
	return YES;
}

- (BOOL)pullJoystick:(int)nr withKeycode:(int)k device:(int)d
{
    assert (nr == 1 || nr == 2);
    
    if (d != IPD_KEYSET_1 && d != IPD_KEYSET_2)
        return NO;
    
    unsigned keyset = (d == IPD_KEYSET_1) ? 0 : 1;
    Joystick *joy = (nr == 1) ? c64->joystick1 : c64->joystick2;

    if (k == joyKeycode[keyset][JOYSTICK_UP]) { joy->SetAxisY(JOYSTICK_AXIS_Y_UP); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_DOWN]) { joy->SetAxisY(JOYSTICK_AXIS_Y_DOWN); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_LEFT]) { joy->SetAxisX(JOYSTICK_AXIS_X_LEFT); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_RIGHT]) { joy->SetAxisX(JOYSTICK_AXIS_X_RIGHT); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_FIRE]) { joy->SetButtonPressed(true); return YES; }

    return NO;
}

- (BOOL)releaseJoystick:(int)nr withKeycode:(int)k device:(int)d
{
    assert (nr == 1 || nr == 2);
    
    if (d != IPD_KEYSET_1 && d != IPD_KEYSET_2)
        return NO;
    
    unsigned keyset = (d == IPD_KEYSET_1) ? 0 : 1;
    Joystick *joy = (nr == 1) ? c64->joystick1 : c64->joystick2;
    
    if (k == joyKeycode[keyset][JOYSTICK_UP]) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_DOWN]) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_LEFT]) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_RIGHT]) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (k == joyKeycode[keyset][JOYSTICK_FIRE]) { joy->SetButtonPressed(false); return YES; }
    
    return NO;
}

- (int)translateKey:(char)key plainkey:(char)plainkey keycode:(short)keycode flags:(int)flags
{
    switch (keycode) {
        case MAC_F1: return Keyboard::C64KEY_F1;
        case MAC_F2: return Keyboard::C64KEY_F2;
        case MAC_F3: return Keyboard::C64KEY_F3;
        case MAC_F4: return Keyboard::C64KEY_F4;
        case MAC_F5: return Keyboard::C64KEY_F5;
        case MAC_F6: return Keyboard::C64KEY_F6;
        case MAC_F7: return Keyboard::C64KEY_F7;
        case MAC_F8: return Keyboard::C64KEY_F8;
        case MAC_DEL: return (flags & NSShiftKeyMask) ? Keyboard::C64KEY_INS : Keyboard::C64KEY_DEL;
        case MAC_RET: return Keyboard::C64KEY_RET;
        case MAC_CL: return Keyboard::C64KEY_CL;
        case MAC_CR: return Keyboard::C64KEY_CR;
        case MAC_CU: return Keyboard::C64KEY_CU;
        case MAC_CD: return Keyboard::C64KEY_CD;
        case MAC_HAT: return '^';
        case MAC_TILDE_US: if (plainkey != '<' && plainkey != '>') return Keyboard::C64KEY_ARROW; else break;
    }
    
    if (flags & NSAlternateKeyMask) {
        // Commodore key (ALT) is pressed
        return (int)plainkey | Keyboard::C64KEY_COMMODORE;
    } else {
        // No special translation needed here
        return (int)key;
    }
}

- (void)keyDown:(NSEvent *)event
{
	unsigned char  c       = [[event characters] UTF8String][0];
    unsigned char  c_unmod = [[event charactersIgnoringModifiers] UTF8String][0];
	unsigned short keycode = [event keyCode];
	unsigned int   flags   = [event modifierFlags];
    int c64key;
    
    // NSLog(@"keyDown: '%c' keycode: %02X flags: %08X", (char)c, keycode, flags);
    
    // Ignore keys that are already pressed
    if (pressedKeys[(unsigned char)keycode])
        return;
    
    // Ignore command key
    if (flags & NSCommandKeyMask)
        return;
    
    // Simulate joysticks
    int fingerprint = [self fingerprintForKey:keycode withModifierFlags:flags];
    if ([self pullJoystick:1 withKeycode:fingerprint device:[controller inputDeviceA]])
        return;
    if ([self pullJoystick:2 withKeycode:fingerprint device:[controller inputDeviceB]])
        return;

    // Remove alternate key modifier if present
    if (flags & NSAlternateKeyMask)
        c = [[event charactersIgnoringModifiers] UTF8String][0];

    // Translate key
    if (!(c64key = [self translateKey:c plainkey:c_unmod keycode:keycode flags:flags]))
        return;
    
    // Press key
    // NSLog(@"Storing key %c for keycode %ld",c64key, (long)keycode);
    pressedKeys[(unsigned char)keycode] = c64key;
    c64->keyboard->pressKey(c64key);
}

- (void)keyUp:(NSEvent *)event
{
    unsigned short keycode = [event keyCode];
	unsigned int   flags   = [event modifierFlags];
    
    // NSLog(@"keyUp: keycode: %02X flags: %08X", keycode, flags);

    // Simulate joysticks
    int fingerprint = [self fingerprintForKey:keycode withModifierFlags:flags];
    if ([self releaseJoystick:1 withKeycode:fingerprint device:[controller inputDeviceA]])
        return;
    if ([self releaseJoystick:2 withKeycode:fingerprint device:[controller inputDeviceB]])
        return;
    
    // Only proceed if the released key is on the records
    if (!pressedKeys[(unsigned char)keycode])
        return;
    
    // Release key
    // NSLog(@"Releasing stored key %c for keycode %ld",pressedKeys[keycode], (long)keycode);
    c64->keyboard->releaseKey(pressedKeys[keycode]);
    pressedKeys[(unsigned char)keycode] = 0;
}

- (void)flagsChanged:(NSEvent *)event
{
    // Note: We only respond to this message if one of the special keys is used for joystick emulation

    unsigned int flags = [event modifierFlags];
    int keycode;
    
    // Check if one of the supported special keys has been pressed or released
    if (flags & NSAlternateKeyMask)
        keycode = NSAlternateKeyMask;
    else if (flags & NSShiftKeyMask)
        keycode = NSShiftKeyMask;
    else if (flags & NSCommandKeyMask)
        keycode = NSCommandKeyMask;
    else if (flags & NSControlKeyMask)
        keycode = NSControlKeyMask;
    else {
        // NSLog(@"Release Joystick");
        // Relase joysticks
        (void)([self releaseJoystick:1 withKeycode:NSAlternateKeyMask device:[controller inputDeviceA]]);
        (void)([self releaseJoystick:1 withKeycode:NSShiftKeyMask device:[controller inputDeviceA]]);
        (void)([self releaseJoystick:1 withKeycode:NSCommandKeyMask device:[controller inputDeviceA]]);
        (void)([self releaseJoystick:1 withKeycode:NSControlKeyMask device:[controller inputDeviceA]]);
        (void)([self releaseJoystick:2 withKeycode:NSAlternateKeyMask device:[controller inputDeviceB]]);
        (void)([self releaseJoystick:2 withKeycode:NSShiftKeyMask device:[controller inputDeviceB]]);
        (void)([self releaseJoystick:2 withKeycode:NSCommandKeyMask device:[controller inputDeviceB]]);
        (void)([self releaseJoystick:2 withKeycode:NSControlKeyMask device:[controller inputDeviceB]]);
        return;
    }

    // Pull joysticks
    if ([self pullJoystick:1 withKeycode:keycode device:[controller inputDeviceA]])
        return;
    if ([self pullJoystick:2 withKeycode:keycode device:[controller inputDeviceB]])
        return;
}


// --------------------------------------------------------------------------------
//                                      Paste
// --------------------------------------------------------------------------------


- (void)paste:(id)sender
{
    NSPasteboard *gpBoard;
    NSString *text;

    
    gpBoard = [NSPasteboard generalPasteboard];
    if (!(text = [gpBoard stringForType:NSStringPboardType])) {
        NSLog(@"Paste failed");
        return;
    }

    [[[controller c64] keyboard] typeText:text];
}


// --------------------------------------------------------------------------------
//                                  Drag and Drop 
// --------------------------------------------------------------------------------

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	// NSLog(@"draggingEntered");
	if ([sender draggingSource] == self)
		return NSDragOperationNone;

	NSPasteboard *pb = [sender draggingPasteboard];
	NSString *besttype = [pb availableTypeFromArray:[NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];

	if (besttype == NSFilenamesPboardType) {
		// NSLog(@"Dragged in filename");
		return NSDragOperationCopy;
	}

	if (besttype == NSPasteboardTypeString) {
		// NSLog(@"Dragged in string");
		return NSDragOperationCopy;
	}

	if (besttype == NSFileContentsPboardType) {
		// NSLog(@"Dragged in file contents");
		return NSDragOperationCopy;
	}
	
	return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
	return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pb = [sender draggingPasteboard];
	
	if ([[pb types] containsObject:NSFileContentsPboardType]) {
		
        NSFileWrapper *fileWrapper = [pb readFileWrapper];
		NSData *fileData = [fileWrapper regularFileContents];
		V64Snapshot *snapshot = [V64Snapshot snapshotFromBuffer:[fileData bytes] length:[fileData length]];
		[[controller c64] loadFromSnapshot:snapshot];
		return YES;
	}

	if ([[pb types] containsObject:NSFilenamesPboardType]) {
		
        NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];			
		NSLog(@"Processing file %@", path);
		
        // Is it a snapshot?
        if (Snapshot::isSnapshot([path UTF8String])) {
            
            NSLog(@"  Snapshot found");
            
            // Do the version numbers match?
            if (Snapshot::isSnapshot([path UTF8String], V_MAJOR, V_MINOR, V_SUBMINOR)) {
                
                V64Snapshot *snapshot = [V64Snapshot snapshotFromFile:path];
                if (snapshot) {
                    [[controller c64] loadFromSnapshot:snapshot];
                    return YES;
                }
                
            } else {

                NSLog(@"  ERROR: Version number in snapshot must be %d.%d", V_MAJOR, V_MINOR);
                [[controller document] showVersionNumberAlert];
                return NO;
            }
        }
        
        // Is it a ROM file?
		if ([[controller document] loadRom:path]) {
			return YES;
		}
				
		// Is it a cartridge?
		if ([[controller document] setCartridgeWithName:path]) {
			[controller mountCartridge];
			return YES;
		}
		
		// Is it an archive?
		if ([[controller document] setArchiveWithName:path]) {
			[controller showMountDialog];
			return YES;
		}					
	}
	
	return NO;	
}

- (void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
}

@end
