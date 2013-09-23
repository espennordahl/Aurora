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

#include <tbb/mutex.h>

#include "auroraObject.h"
#include "renderer.h"
#include "attributeChange.h"

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
        
        void addAttributeChange(const AttributeChange &change);

    private:
        std::vector<AttributeChange> m_changes;
        tbb::mutex m_changeMutex;
        std::vector<ObjectPtr> m_objects;
        Renderer m_renderer;
        
        void applyAttributeChanges();
    };
}

#endif /* defined(__Aurora__session__) */
