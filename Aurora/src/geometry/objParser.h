//
//  objParser.h
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#ifndef Aurora_objParser_h
#define Aurora_objParser_h

#include "core.h"

#include <string.h>
#include <vector>

namespace Aurora {
    enum objParserState{
        idle,
        waitingForLine,
        readingPoint,
        readingNormal,
        readingUV,
        readingIncides,
        doneParsing,
        error,
    };
    
    class ObjParser{
    public:
        ObjParser(const std::string inputfile,
                  std::vector<Point> *points,
                  std::vector<Vector> *normals,
                  std::vector<uv> *uvs,
                  std::vector<int> *vertIndex,
                  std::vector<int> *normIndex,
                  std::vector<int> *uvIndex);
        
        bool parseFile();
        
    private:
        
        void run();
        
        objParserState state;
        char currentLine[256];
        uint32_t readLines;
        
        std::string inputfile;
        std::vector<Point> *points;
        std::vector<Vector> *normals;
        std::vector<uv> *uvs;
        std::vector<int> *vertIndex;
        std::vector<int> *normIndex;
        std::vector<int> *uvIndex;
    };
}

#endif
