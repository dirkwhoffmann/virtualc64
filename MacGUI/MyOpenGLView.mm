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

@synthesize c64, frames, enableOpenGL, drawIn3D, drawC64texture, drawBackground, drawEntireCube, antiAliasing;

// --------------------------------------------------------------------------------
//                                  Initializiation
// --------------------------------------------------------------------------------

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pixFmt
{
	NSLog(@"MyOpenGLView::initWithFrame");
	
	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt]) == nil) {
		NSLog(@"ERROR: Can't initiaize VICscreen\n");
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)c
{
	NSLog(@"MyOpenGLView::initWithCoder");

	if ((self = [super initWithCoder:c]) == nil) {
		NSLog(@"ERROR: Can't initiaize VICscreen\n");
	}
		return self;
}

-(void)awakeFromNib
{
	NSLog(@"MyOpenGLView::awakeFromNib");
	    
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
	antiAliasing = true;
	
	// Core video and graphics stuff
	displayLink = nil;
	frames = 0;
	enableOpenGL = true;  
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
	NSLog(@"Deallocating OpenGL ressources");
	
	// release display link
    if (displayLink) {
    	CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
    
	// release current frame
    //if (currentFrame) {
    //	CVOpenGLTextureRelease(currentFrame);
    //    currentFrame = NULL;
    //}
	    
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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

		NSLog(@"Display link activated successfully");
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

- (void)setPAL
{
   	[self setEyeX:PAL_INITIAL_EYE_X];
	[self setEyeY:PAL_INITIAL_EYE_Y];
	[self setEyeZ:PAL_INITIAL_EYE_Z]; 
}

- (void)setNTSC
{
   	[self setEyeX:NTSC_INITIAL_EYE_X];
	[self setEyeY:NTSC_INITIAL_EYE_Y];
	[self setEyeZ:NTSC_INITIAL_EYE_Z]; 
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
	NSLog(@"Zooming in...\n");

	currentEyeZ     = 6;
	targetXAngle    = 0;
	targetYAngle    = 0;
	targetZAngle    = 0;
	
	[self computeAnimationDeltaSteps:120 /* 2 sec */];
}

- (void)rotateBack
{
	NSLog(@"Rotating back...\n");

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
	NSLog(@"Rotating...\n");

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
	NSLog(@"Scrolling...\n");
	
	currentEyeY    = 0.9;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;

	[self computeAnimationDeltaSteps:120];		
}

- (void)fadeIn
{
	NSLog(@"Fading in...\n");
	
	
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
    // Determine screen geometry (differs between NTSC and PAL)
	textureXStart = (float)c64->vic->getFirstVisiblePixel() / (float)TEXTURE_WIDTH;
	textureXEnd = (float)c64->vic->getLastVisiblePixel() / (float)TEXTURE_WIDTH;
	textureYStart = (float)c64->vic->getFirstVisibleLine() / (float)TEXTURE_HEIGHT;
	textureYEnd = (float)c64->vic->getLastVisibleLine() / (float)TEXTURE_HEIGHT;
	dimX = 0.64;
	dimY = dimX * (float)c64->vic->getTotalScreenHeight() / (float)c64->vic->getTotalScreenWidth() / c64->vic->getPixelAspectRatio();
}

- (void)updateScreenTexture
{
	glBindTexture(GL_TEXTURE_2D, texture);			
	if (c64) {
		void *buf = c64->vic->screenBuffer(); 
		assert(buf != NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c64->vic->getTotalScreenWidth(), TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buf);
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
	// Changing the shadeModel has no effect
	// glShadeModel(antiAliasing ? GL_SMOOTH : GL_FLAT);

    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    [self updateScreenTexture];
	
	// Set location
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
				  
	// Set viewpoint
	gluLookAt(0, 0, 1.33, 0, 0, 0, 0, 1, 0);
		
	bool animation = [self animates];

	// bool drawBackground = animation || !drawC64texture;
	// bool drawBackground = !drawC64texture;
	if (drawBackground) {
		float depth = -5.0f;
		float scale = 9.2f;
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

		// NSLog(@"drawingC64Texture");
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
    // NSLog(@"%@ accepting first responder...", self);
	return YES;
}

- (BOOL)resignFirstResponder
{
    // NSLog(@"%@ resigning first responder...", self);
	return YES;
}

- (BOOL)becomeFirstResonder 
{
    // NSLog(@"%@ is becoming first responder...", self);
	return YES;
}

- (BOOL)pullJoystick:(int)nr withKey:(char)c up:(char)u down:(char)d left:(char)l right:(char)r fire:(char)f
{
    assert (nr == 1 || nr == 2);
    Joystick *joy = ((nr == 1) ? c64->joystick1 : c64->joystick2);
    
    if (c == u) { joy->SetAxisY(JOYSTICK_AXIS_Y_UP); return YES; }
    if (c == d) { joy->SetAxisY(JOYSTICK_AXIS_Y_DOWN); return YES; }
    if (c == l) { joy->SetAxisX(JOYSTICK_AXIS_X_LEFT); return YES; }
    if (c == r) { joy->SetAxisX(JOYSTICK_AXIS_X_RIGHT); return YES; }
    if (c == f) { joy->SetButtonPressed(true); return true; }
    
    return NO;
}

- (BOOL)pullJoystick:(int)nr withKeycode:(int)k up:(int)u down:(int)d left:(int)l right:(int)r fire:(int)f
{
    assert (nr == 1 || nr == 2);
    Joystick *joy = ((nr == 1) ? c64->joystick1 : c64->joystick2);
    
    if (k == u) { joy->SetAxisY(JOYSTICK_AXIS_Y_UP); return YES; }
    if (k == d) { joy->SetAxisY(JOYSTICK_AXIS_Y_DOWN); return YES; }
    if (k == l) { joy->SetAxisX(JOYSTICK_AXIS_X_LEFT); return YES; }
    if (k == r) { joy->SetAxisX(JOYSTICK_AXIS_X_RIGHT); return YES; }
    if (k == f) { joy->SetButtonPressed(true); return YES; }

    return NO;
}

- (BOOL)pullJoystick:(int)nr withKey:(char)c withKeycode:(int)k device:(int)d
{
    switch (d) {
        case IPD_KEYBOARD_1:
            return [self pullJoystick:nr withKeycode:k up:MAC_CU down:MAC_CD left:MAC_CL right:MAC_CR fire:MAC_SPC];
        case IPD_KEYBOARD_2:
            return [self pullJoystick:nr withKeycode:k up:MAC_CU down:MAC_CD left:MAC_CL right:MAC_CR fire:18 /* 1 */];
        case IPD_KEYBOARD_3:
            return [self pullJoystick:nr withKey:c up:'w' down:'y' left:'a' right:'s' fire:'-'];
        case IPD_KEYBOARD_4:
            return [self pullJoystick:nr withKey:c up:'8' down:'2' left:'4' right:'6' fire:'0'];
    }

    return NO;
}

- (BOOL)releaseJoystick:(int)nr withKey:(char)c up:(char)u down:(char)d left:(char)l right:(char)r fire:(char)f
{
    assert (nr == 1 || nr == 2);
    Joystick *joy = ((nr == 1) ? c64->joystick1 : c64->joystick2);
    
    if (c == u) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (c == d) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (c == l) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (c == r) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (c == f) { joy->SetButtonPressed(false); return YES; }

    return NO;
}

- (BOOL)releaseJoystick:(int)nr withKeycode:(int)k up:(int)u down:(int)d left:(int)l right:(int)r fire:(int)f
{
    assert (nr == 1 || nr == 2);
    Joystick *joy = ((nr == 1) ? c64->joystick1 : c64->joystick2);
    
    if (k == u) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (k == d) { joy->SetAxisY(JOYSTICK_AXIS_NONE); return YES; }
    if (k == l) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (k == r) { joy->SetAxisX(JOYSTICK_AXIS_NONE); return YES; }
    if (k == f) { joy->SetButtonPressed(false); return YES; }

    return NO;
}

- (BOOL)releaseJoystick:(int)nr withKey:(char)c withKeycode:(int)k device:(int)d
{
    switch (d) {
        case IPD_KEYBOARD_1:
            return [self releaseJoystick:nr withKeycode:k up:MAC_CU down:MAC_CD left:MAC_CL right:MAC_CR fire:MAC_SPC];
        case IPD_KEYBOARD_2:
            return [self releaseJoystick:nr withKeycode:k up:MAC_CU down:MAC_CD left:MAC_CL right:MAC_CR fire:18 /* 1 */];
        case IPD_KEYBOARD_3:
            return [self releaseJoystick:nr withKey:c up:'w' down:'y' left:'a' right:'s' fire:'-'];
        case IPD_KEYBOARD_4:
            return [self releaseJoystick:nr withKey:c up:'8' down:'2' left:'4' right:'6' fire:'0'];
    }

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
    
    // NSLog(@"keyDown: '%c' keycode: %ld flags: %ld", (char)c, (long)keycode, (long)flags);
    
    // Ignore keys that are already pressed
    if (pressedKeys[(unsigned char)keycode])
        return;
    
    // Ignore command key
    if (flags & NSCommandKeyMask)
        return;
    
    // Simulate joysticks
    if ([self pullJoystick:1 withKey:(char)c withKeycode:keycode device:[controller inputDeviceA]])
        return;
    if ([self pullJoystick:2 withKey:(char)c withKeycode:keycode device:[controller inputDeviceB]])
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
    unsigned char  c       = [[event characters] UTF8String][0];
    unsigned short keycode = [event keyCode];
    
    // NSLog(@"keyUp: '%c' keycode: %ld flags: %ld", (char)c, (long)keycode);
    
    // Simulate joysticks
    if ([self releaseJoystick:1 withKey:(char)c withKeycode:keycode device:[controller inputDeviceA]])
        return;
    if ([self releaseJoystick:2 withKey:(char)c withKeycode:keycode device:[controller inputDeviceB]])
        return;

    // Only proceed if the released key is on the records
    if (!pressedKeys[(unsigned char)keycode])
        return;
    
    // Release key
    // NSLog(@"Releasing stored key %c for keycode %ld",pressedKeys[keycode], (long)keycode);
    c64->keyboard->releaseKey(pressedKeys[keycode]);
    pressedKeys[(unsigned char)keycode] = 0;
}

#if 0
- (void)flagsChanged:(NSEvent *)event
{
	unsigned int flags = [event modifierFlags];

    // NSLog(@"flagsChanged: %ld", (long)flags);
    
    if (flags & NSAlternateKeyMask) {
		c64->keyboard->pressCommodoreKey();
        // NSLog(@"commodore key pressed");
    } else {
        c64->keyboard->releaseCommodoreKey();
        // NSLog(@"commodore key released");
    }
}
#endif


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

    NSLog(@"Pasting %@", text);
    [[[controller c64] keyboard] typeText:text];
}


// --------------------------------------------------------------------------------
//                                  Drag and Drop 
// --------------------------------------------------------------------------------

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	NSLog(@"draggingEntered");
	if ([sender draggingSource] == self)
		return NSDragOperationNone;

	NSPasteboard *pb = [sender draggingPasteboard];
	NSString *besttype = [pb availableTypeFromArray:[NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];

	if (besttype == NSFilenamesPboardType) {
		NSLog(@"Dragged in filename");
		return NSDragOperationCopy;
	}

	if (besttype == NSPasteboardTypeString) {
		NSLog(@"Dragged in string");
		return NSDragOperationCopy;
	}

	if (besttype == NSFileContentsPboardType) {
		NSLog(@"Dragged in file contents");
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
	NSLog(@"performDragOperation");
	//NSString *type;
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
		NSLog(@"Got filename %@", path);
		
        // Check snapshot version if applicable
        int major, minor;
        if (Snapshot::isSnapshot([path UTF8String], &major, &minor)) {
            if (major != V_MAJOR || minor != V_MINOR) {
                NSLog(@"Unsupported snapshot version");
                [[controller document] showVersionNumberAlert];
                return NO;
            }
        }

        // Is it an image file?
        V64Snapshot *snapshot = [V64Snapshot snapshotFromFile:path];
        if (snapshot) {
            [[controller c64] loadFromSnapshot:snapshot];
            NSLog(@"Image file loaded");
            return YES;
        }

        // Is it a ROM file?
		if ([[controller document] loadRom:path]) {
			NSLog(@"ROM loaded");
			return YES;
		}
		
		// Is it raw VC 1541 data?
		if (VC1541::isG64Image([path UTF8String])) {
			c64->floppy->readG64Image([path UTF8String]);
			// [self rotate];
			NSLog(@"G64 loaded");
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
