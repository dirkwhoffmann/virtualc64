//
//  BlurFilter.m
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import <BlurFilter.h>

@implementation BlurFilter

+ (instancetype)filterWithRadius:(float)radius device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    return [[self alloc] initWithRadius:radius device:dev library:lib];
}

- (instancetype)initWithRadius:(float)radius device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    if (!(self = [super initWithFunctionName:@"blur" device:dev library:lib]))
        return nil;
    
    // Build blur weight texture
    const float sigma = radius / 2.0;
    const int size = (round(radius) * 2) + 1;
    
    float delta = 0;
    float expScale = 0;;
    if (radius > 0.0) {
        delta = (radius * 2) / (size - 1);;
        expScale = -1 / (2 * sigma * sigma);
    }
    
    float *weights = (float *)malloc(sizeof(float) * size * size);
    
    float weightSum = 0;
    float y = -radius;
    for (int j = 0; j < size; ++j, y += delta) {
        float x = -radius;
        for (int i = 0; i < size; ++i, x += delta) {
            float weight = expf((x * x + y * y) * expScale);
            weights[j * size + i] = weight;
            weightSum += weight;
        }
    }
    
    const float weightScale = 1 / weightSum;
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size; ++i) {
            weights[j * size + i] *= weightScale;
        }
    }
    
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatR32Float
                                                       width:size
                                                      height:size
                                                   mipmapped:NO];
    
    blurWeightTexture = [dev newTextureWithDescriptor:textureDescriptor];
    
    MTLRegion region = MTLRegionMake2D(0, 0, size, size);
    [blurWeightTexture replaceRegion:region mipmapLevel:0 withBytes:weights bytesPerRow:sizeof(float) * size];
    free(weights);
    
    return self;
}

- (void)configureComputeCommandEncoder:(id <MTLComputeCommandEncoder>)encoder
{
    // NSLog(@"BlurFilter::configureCommandEncoder");
    [encoder setTexture:blurWeightTexture atIndex:2];
    
}


@end
