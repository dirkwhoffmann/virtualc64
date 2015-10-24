//
//  MyMetalViewController.m
//  V64
//
//  Created by Dirk Hoffmann on 24.10.15.
//
//

#import <MyMetalViewController.h>

@implementation MyMetalViewController {
    
    // Reference to the metal view
    MTKView *_view;
    
    // View Controller.
    dispatch_semaphore_t _semaphore;
}

- (void)viewDidLoad
{
    NSLog(@"MyMetalViewController::viewDidLoad");
    [super viewDidLoad];
    
    _semaphore = dispatch_semaphore_create(3);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {

    // Nothing to do here. We take care of it inside the view class
}

- (void)drawInMTKView:(nonnull MTKView *)view {

    // Nothing to do here. We take care of it inside the view class
    // NSLog(@"MyMetalController::drawInMTKView");

}

@end
