//
//  typedefs.h
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#ifndef Aurora_typedefs_h
#define Aurora_typedefs_h

#include "options.h"
#include <tr1/unordered_map>
#include <string.h>

namespace Aurora {
    typedef std::map<Option, double> AuroraGlobals;
    typedef std::tr1::unordered_map<std::string, std::string> StringMap;
}
#endif
