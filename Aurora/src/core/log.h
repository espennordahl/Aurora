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

#define LOG_Main                lc(Log::Debug,	"Main")
#define LOG_BBox                lc(Log::Debug,	"BBox")
#define LOG_Point               lc(Log::Debug,	"Point")
#define LOG_Vector              lc(Log::Debug,	"Point")
#define LOG_Sampler2D           lc(Log::Debug,	"Sampler2D")
#define LOG_Sampler3D           lc(Log::Debug,	"Sampler3D")
#define LOG_Renderer            lc(Log::Debug,	"Renderer")
#define LOG_AdaptiveRndSampler  lc(Log::Debug,	"AdaptiveRndSampler")
#define LOG_Embree              lc(Log::Debug,	"Embree")
#define LOG_Parser              lc(Log::Debug,	"Parser")
#define LOG_KelemenMaterial     lc(Log::Debug,	"Kelemen")
#define LOG_MatteMaterial       lc(Log::Debug,	"Matte")
#define LOG_MetalMaterial       lc(Log::Debug,	"Metal")
#define LOG_Lights              lc(Log::Debug,	"Lights")
#define LOG_Matrix              lc(Log::Debug,	"Matrix")
#define LOG_Texture2D           lc(Log::Debug,	"Texture2D")
#define LOG_Lambert             lc(Log::Debug,	"Lambert")
#define LOG_ObjMesh             lc(Log::Debug,	"ObjMesh")
#define LOG_KelemenShader       lc(Log::Debug,  "KelemenShader")
#define LOG_ShaderCache         lc(Log::Debug,  "ShaderCache")
#define LOG_JsonParser          lc(Log::Debug,  "JsonParser")
#define LOG_AuroraObj           lc(Log::Debug,  "AuroraObject")
#define LOG_Texture2DShd        lc(Log::Debug,  "Texture2dShader")
#define LOG_ShaderAttr          lc(Log::Debug,  "ShaderAttribute")
#define LOG_ExrDisplay          lc(Log::Debug,  "ExrDisplay")
#define LOG_StringUtils         lc(Log::Debug,  "StringUtils")
#define LOG_Subdiv              lc(Log::Debug,  "Subdiv")
#define LOG_Material            lc(Log::Debug,  "Material")
#define LOG_Integrator          lc(Log::Debug,  "Integrator")

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
