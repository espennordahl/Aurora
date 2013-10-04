//
//  ValueWheelView.m
//  Aurora
//
//  Created by Espen Nordahl on 14/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "ValueWheelView.h"

@interface ValueWheelView ()

@property NSMutableSet *m_targets;

@end

@implementation ValueWheelView

@synthesize floatValue = _floatValue;
@synthesize m_targets;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        m_targets = [[NSMutableSet alloc] init];
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self _drawBackground];
    [self _drawText];
    [self _drawHandle];
    [self _drawUpButton];
    [self _drawDownButton];
}

-(void)_drawBackground
{
    NSBezierPath *rect = [NSBezierPath bezierPathWithOvalInRect:CGRectMake(0, 0, self.frame.size.width, self.frame.size.height)];
    [[NSColor colorWithCalibratedRed:0.18 green:0.18 blue:0.18 alpha:0.5] setFill];
    [rect fill];
}

-(void)_drawText
{
    CGPoint centerPoint = CGPointMake(self.frame.size.width/2., self.frame.size.height/2.);
    CGPoint drawPoint = CGPointMake(centerPoint.x-12, centerPoint.y-7);
    [[NSString stringWithFormat:@"%.2f", self.floatValue] drawAtPoint:drawPoint withAttributes:Nil];
}

-(void)_drawHandle
{
    CGPoint centerPoint = CGPointMake(self.frame.size.width/2., self.frame.size.height/2.);
    float angle = -(M_PI * 2 * (self.floatValue - floorf(self.floatValue))) - M_PI/2;
    float handleLength = MIN(self.frame.size.width, self.frame.size.height) * 0.4;
    CGPoint innerPoint = CGPointMake(cos(angle)*handleLength/2 + centerPoint.x,
                                     sin(angle)*handleLength/2 + centerPoint.y);
    CGPoint outerPoint = CGPointMake(cos(angle)*handleLength + centerPoint.x,
                                     sin(angle)*handleLength + centerPoint.y);
    NSBezierPath *handlePath = [NSBezierPath bezierPath];
    [handlePath moveToPoint:innerPoint];
    [handlePath lineToPoint:outerPoint];
    [handlePath stroke];
}

-(void)_drawUpButton
{
    NSBezierPath *buttonPath = [NSBezierPath bezierPathWithOvalInRect:[self _upButtonRect]];
    [buttonPath fill];
}

-(void)_drawDownButton
{
    NSBezierPath *buttonPath = [NSBezierPath bezierPathWithOvalInRect:[self _downButtonRect]];
    [buttonPath fill];
}

-(CGRect)_downButtonRect
{
    CGPoint centerPoint = CGPointMake(self.frame.size.width/2., self.frame.size.height/2.);
    float buttonSize = 20;
    
    return CGRectMake(centerPoint.x - buttonSize/2, centerPoint.y - 15 - buttonSize, buttonSize, buttonSize);
}

-(CGRect)_upButtonRect
{
    CGPoint centerPoint = CGPointMake(self.frame.size.width/2., self.frame.size.height/2.);
    float buttonSize = 20;
    
    return CGRectMake(centerPoint.x - buttonSize/2, centerPoint.y + 15, buttonSize, buttonSize);
}

-(void)setTarget:(id)target
{
    [m_targets addObject:target];
}

-(void)removeTarget:(id)target
{
    [m_targets removeObject:target];
}

-(void)setFloatValue:(float)floatValue
{
    if (floatValue != _floatValue) {
        _floatValue = floatValue;
        [self setNeedsDisplay:YES];
        for (id <ValueViewDelegate> target in m_targets) {
            [target valueChanged:self];
        }
    }
}

-(float)floatValue
{
    return _floatValue;
}

-(void)mouseDown:(NSEvent *)theEvent
{
    self.floatValue = [self _valueForMousePos:[self convertPoint:theEvent.locationInWindow fromView:self.superview]];
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    self.floatValue = [self _valueForMousePos:[self convertPoint:theEvent.locationInWindow fromView:self.superview]];
}

-(float)_valueForMousePos:(CGPoint)mousePos
{
    if (NSPointInRect(mousePos, [self _downButtonRect])) {
        return self.floatValue - 1;
    } else if(NSPointInRect(mousePos, [self _upButtonRect])){
        return self.floatValue + 1;
    }
    
    CGPoint center = CGPointMake(self.frame.size.width/2., self.frame.size.height/2.);
    float angle = atan2f(mousePos.x - center.x, mousePos.y - center.y) + M_PI;
    int integerValue = floor(self.floatValue);
    return integerValue + angle / (2.f * M_PI);
}

@end
