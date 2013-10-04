//
//  AppDelegate.m
//  AuroraUI
//
//  Created by Espen Nordahl on 14/08/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "AppDelegate.h"

#include "session.h"

class SessionBridge : public Aurora::Session{
public:
    SessionBridge(const std::string &scenefile):
    Aurora::Session(scenefile),
    m_updateCount(0)
    {};
    
    void imageDidUpdate(){
        [delegate performSelectorOnMainThread:@selector(imageDidUpdate:) withObject:Nil waitUntilDone:NO];
    }

    void start(){
        m_updateCount = 0;
        Session::start();
    }
    
    char *imageFile(){
        ++m_updateCount;
        if (shouldReturnThumbnail()) {
            return Session::proxyFile((4 - m_updateCount)*2);
        } else {
            return Session::imageFile();
        }
    }
    
    AppDelegate *delegate;

private:
    int m_updateCount;
    
    bool shouldReturnThumbnail(){
        return m_updateCount < 4;
    }
};

@interface AppDelegate ()

@property SessionBridge *m_session;

@end

@implementation AppDelegate

@synthesize m_session;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self.renderViewController.view addObserver:self forKeyPath:@"frame" options:Nil context:Nil];
    self.window.delegate = self;
    [self _setupSession];
    [self _setupLightEdit];
    m_session->start();
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (object == self.renderViewController.view) {
        if ([keyPath isEqualToString:@"frame"]) {
            m_session->setResolution(self.renderViewController.view.frame.size.width, self.renderViewController.view.frame.size.height);
        }
    }
}

-(void)_setupLightEdit
{
    self.lightEditController.session = m_session;
}

-(void)_setupSession
{
    m_session = new SessionBridge("/Users/espennordahl/Documents/Aurora/pyAurora/tmp.asc");
    m_session->setResolution(self.renderViewController.view.frame.size.width, self.renderViewController.view.frame.size.height);
    m_session->delegate = self;
}

-(void)imageDidUpdate:(NSNotification*)notification
{
    void *pixeldata = m_session->imageFile();
    int width = m_session->width();
    int height = m_session->height();
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
                                                              pixeldata,
                                                              width*height*4,
                                                              NULL);
    
    int bitsPerComponent = 8;
    int bitsPerPixel = 4*bitsPerComponent;
    int bytesPerRow = bitsPerPixel * width / 8;

    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGImageAlphaLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    CGImageRef imageRef = CGImageCreate(width,
                                        height,
                                        bitsPerComponent,
                                        bitsPerPixel,
                                        bytesPerRow,
                                        colorSpaceRef,
                                        bitmapInfo,
                                        provider,
                                        NULL,
                                        NO,
                                        renderingIntent);

    NSImage *render = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
    
    [self.renderViewController imageDidChange:render];
}

@end
