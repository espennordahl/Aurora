//
//  attributes.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_attributes_h
#define Aurora_attributes_h

#include <tr1/unordered_map>
#include <string.h>

namespace Aurora {
	class Attributes : public ReferenceCounted {
	public:
		Attributes();
		Attributes(std::tr1::unordered_map<std::string, std::string> _attributes);
		
		std::string getAttribute(std::string key);
		std::tr1::unordered_map<std::string, std::string> getAttributes();
		
		void addAttribute(std::string key, std::string value);
		void addAttributes(std::tr1::unordered_map<std::string, std::string> _attributes);
		
		int size();
		
	private:
		std::tr1::unordered_map<std::string, std::string> attributes;
	};
}

#endif
