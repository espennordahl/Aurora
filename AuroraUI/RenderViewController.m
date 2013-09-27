//
//  RenderViewController.m
//  Aurora
//
//  Created by Espen Nordahl on 14/08/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "RenderViewController.h"
#import "RenderView.h"

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

//
//-(void)awakeFromNib
//{
//    [self.view setWantsLayer:YES];
//    [self.view setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawOnSetNeedsDisplay];    
//}

-(void)imageDidChange:(NSImage*)image
{
    NSLog(@"Received image update. Updating view!");
    NSLog(@"Image size: %f %f", image.size.width, image.size.height);
    RenderView* view = (RenderView*)self.view;
    view.image = image;
    self.view = view;
    [self.view setNeedsDisplay:YES];
}

@end
