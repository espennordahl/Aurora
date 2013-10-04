//
//  RenderView.m
//  Aurora
//
//  Created by Espen Nordahl on 24/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "RenderView.h"

@implementation RenderView

@synthesize image;
@synthesize color;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self.image drawInRect:CGRectMake(0, 0, self.image.size.width, self.image.size.height)];
}

@end
