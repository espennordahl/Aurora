//
//  RenderViewController.m
//  Aurora
//
//  Created by Espen Nordahl on 14/08/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "RenderViewController.h"

@interface RenderViewController ()

@end

@implementation RenderViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    return self;
}

-(void)imageDidChange:(NSImage*)image
{
    NSLog(@"Received image update. Updating view!");
    NSLog(@"Image size: %f %f", image.size.width, image.size.height);
    NSImageView *view = (NSImageView *)self.view;

        // ugly hack to force refresh
    [view setImageFrameStyle:NSImageFramePhoto];
    view.image = image;
    CGRect frame = view.frame;
    view.frame = CGRectMake(1, 2, 3, 4);
    view.frame = frame;
    [view setImageFrameStyle:NSImageFrameGrayBezel];
    [view setNeedsDisplay];
}

@end
