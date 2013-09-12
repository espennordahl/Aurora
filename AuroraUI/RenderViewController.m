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
    [image recache];
    NSLog(@"Image size: %f %f", image.size.width, image.size.height);
    NSImageView *view = (NSImageView *)self.view;
    view.image = image;
    [view setNeedsDisplay:YES];
        // setNeedsDisplay doesn't work for some reason, so forcing
        // an update by changing the frame instead...
    CGRect oldFrame = view.frame;
    view.frame = CGRectMake(0, 0, 10, 10);
    view.frame = oldFrame;
}

@end
