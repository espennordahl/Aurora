//
//  referenceCounted.h
//  Aurora
//
//  Created by Espen Nordahl on 18/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ReferenceCounted_h
#define ReferenceCounted_h

#include "pbrtParalallel.h"

namespace Aurora {
	class ReferenceCounted{
	public:
		ReferenceCounted(){ nReferences = 0; }
		AtomicInt32 nReferences;
	};
}


#endif
