//
//  ItemPickedView.m
//  Aurora
//
//  Created by Espen Nordahl on 30/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "ItemPickerView.h"

@implementation ItemPickerView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSMutableParagraphStyle *style = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
    [style setAlignment:NSCenterTextAlignment];

    const float fontSize = 16;
    NSFont *font = [NSFont boldSystemFontOfSize:fontSize];
    
    NSDictionary *attributes = @{NSParagraphStyleAttributeName: style,
                                 NSFontAttributeName: font};
    [self.name drawInRect:CGRectMake(0, self.frame.size.height/2, self.frame.size.width, self.frame.size.height/2) withAttributes:attributes];
    
    NSBezierPath *rightButton = [NSBezierPath bezierPathWithOvalInRect:[self _rightButtonRect]];
    [rightButton fill];
    [rightButton stroke];

    NSBezierPath *leftButton = [NSBezierPath bezierPathWithOvalInRect:[self _leftButtonRect]];
    [leftButton fill];
    [leftButton stroke];
}

-(CGRect)_leftButtonRect
{
    float buttonSize = self.frame.size.height/2;
    float erode = 5;

    CGRect leftButtonRect = CGRectMake(0, self.frame.size.height/2 - buttonSize/2, buttonSize, buttonSize);
    return CGRectInset(leftButtonRect, erode, erode);
}

-(CGRect)_rightButtonRect
{
    float buttonSize = self.frame.size.height/2;
    float erode = 5;

    CGRect rightButtonRect = CGRectMake(self.frame.size.width-buttonSize, self.frame.size.height/2 - buttonSize/2, buttonSize, buttonSize);
    return CGRectInset(rightButtonRect, erode, erode);
}

-(void)mouseDown:(NSEvent *)theEvent
{
    NSPoint localPoint = [self convertPoint:theEvent.locationInWindow fromView:self.superview];
    if (NSPointInRect(localPoint, [self _leftButtonRect])) {
        [self.delegate previousItemSelected:self];
    } else if (NSPointInRect(localPoint, [self _rightButtonRect])) {
        [self.delegate nextItemSelected:self];
    }
}

@end
