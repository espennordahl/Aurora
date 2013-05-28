//
//  reference.h
//  Aurora
//
//  Created by Espen Nordahl on 18/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Reference_h
#define Reference_h

#include "referenceCounted.h"

namespace Aurora {
	template <typename T> class Reference{
	public:
		Reference(T *p = NULL){
			ptr = p;
			if (ptr) AtomicAdd(&ptr->nReferences, 1);
		}
		
		Reference(const Reference<T> &r){
			ptr = r.ptr;
			if (ptr) AtomicAdd(&ptr->nReferences, 1);
		}
		
		Reference &operator=(const Reference<T> &r){
			if (r.ptr) AtomicAdd(&r.ptr->nReferences, 1);
			if (ptr && AtomicAdd(&ptr->nReferences, -1) == 0) delete ptr;
			ptr = r.ptr;
			return *this;
		}
		
		Reference &operator=(T *p){
			if (p) AtomicAdd(&p->nReferences, 1);
			if (ptr && AtomicAdd(&ptr->nReferences, -1) == 0) delete ptr;
			ptr = p;
			return *this;
		}
		
		~Reference(){
			if (ptr && AtomicAdd(&ptr->nReferences, -1) == 0)
				delete ptr;
		}
		
		T *operator->(){ return ptr; }
		const T *operator->() const { return ptr; }
		operator bool() const { return ptr != NULL; }
		const T *getPtr() const { return ptr; }
	private:
		T *ptr;
	};
}


#endif
