//
//  session.h
//  Aurora
//
//  Created by Espen Nordahl on 7/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__session__
#define __Aurora__session__

#include <iostream>

#include "auroraObject.h"
#include "renderer.h"

namespace Aurora {
    
    class Session{
    public:
        Session();
        Session(const std::string scenefile);
        
        void start();
        void stop();
        
        void addObject(ObjectPtr object);
        void removeObject(ObjectPtr object);

        enum ChangeType{
            kAttributeChange,
            kGeometryChange
        };
        
        void objectChanged(ObjectPtr object, ChangeType change);
        
        void setResolution(int width, int height);
        
        int width();
        int height();
        
        const std::vector<ObjectPtr> &objects() const;
        
        void *imageFile();
        
        virtual void imageDidUpdate() = 0;

    private:
        std::vector<ObjectPtr> m_objects;
        Renderer m_renderer;
    };
}

#endif /* defined(__Aurora__session__) */
