//
//  logger.h
//  Aurora
//
//  Created by Espen Nordahl on 19/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef logger_h
#define logger_h

#include <iostream>
#include <assert.h>

using namespace std;

struct lc{
	int i;
	string s;
	lc(int _i, string _s){
		i = _i; s = _s;
	}
};

// Contexts

#define LOG_Main				lc(Log::Debug,	"Main")
#define LOG_BBox				lc(Log::Info,	"BBox")
#define LOG_Point				lc(Log::Info,	"Point")
#define LOG_Vector				lc(Log::Info,	"Point")
#define LOG_Sampler2D			lc(Log::Info,	"Sampler2D")
#define LOG_Sampler3D			lc(Log::Info,	"Sampler3D")
#define LOG_Renderer			lc(Log::Info,	"Renderer")
#define LOG_AdaptiveRndSampler  lc(Log::Info,	"AdaptiveRndSampler")
#define LOG_Gridaccel			lc(Log::Info,	"GridAccel")
#define LOG_Parser              lc(Log::Info,	"Parser")
#define LOG_KelemenMaterial     lc(Log::Info,	"Kelemen")
#define LOG_Lights              lc(Log::Info,	"Lights")
#define LOG_Matrix              lc(Log::Info,	"Matrix")
#define LOG_Texture2D           lc(Log::Info,	"Texture2D")
#define LOG_Lambert             lc(Log::Info,	"Lambert")
#define LOG_ObjMesh             lc(Log::Info,	"ObjMesh")
#define LOG_KelemenShader       lc(Log::Info,   "KelemenShader")
#define LOG_ShaderCache         lc(Log::Debug,   "ShaderCache")

class Log
{
public:
	
    enum Level { Error, Warning, Info, Debug };
	
	
    static ostream& GetStream() { return cout; }
    static bool IsLevelActive(int c, Level l) {
        if (l > c)
            return false;
        return true;
    }
};

#ifndef NO_LOG
#define LOG_ERROR(M)   do { if (Log::IsLevelActive(lcontext.i, Log::Error))   (Log::GetStream() << \
"ERROR: \t\t" << lcontext.s << ": \t" << M << endl), assert(false); } while (false)
#define LOG_INFO(M)    do { if (Log::IsLevelActive(lcontext.i, Log::Info))    (Log::GetStream() << \
"INFO: \t\t" << lcontext.s << ": \t" << M << endl); } while (false)
#define LOG_DEBUG(M)    do { if (Log::IsLevelActive(lcontext.i, Log::Debug))    (Log::GetStream() << \
"DEUBG: \t\t" << lcontext.s << ": \t" << M << endl); } while (false)
#define LOG_WARNING(M) do { if (Log::IsLevelActive(lcontext.i, Log::Warning)) (Log::GetStream() << \
"WARNING: \t" << lcontext.s << ": \t" << M << endl); } while (false)

#else
#define LOG_ERROR(M)
#define LOG_INFO(M)
#define LOG_DEBUG(M)
#define LOG_WARNING(M)
#endif

#endif
