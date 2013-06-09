//
//  objParser.cpp
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#include "objParser.h"

using namespace Aurora;


ObjParser::ObjParser(   const std::string _inputfile,
                        std::vector<Point> *_points,
                        std::vector<Vector> *_normals,
                        std::vector<uv> *_uvs,
                        std::vector<int> *_vertIndex,
                        std::vector<int> *_normIndex,
                         std::vector<int> *_uvIndex):
inputfile(_inputfile),
points(_points),
normals(_normals),
uvs(_uvs),
vertIndex(_vertIndex),
normIndex(_normIndex),
uvIndex(_uvIndex)
{
    state = idle;
    readLines = 0;
}

inline void readSymbol(char *line, objParserState *state){
    if(line[0] == *"v"){
        if(line[1] == *"n"){
            *state = readingNormal;
            return;
        }
        else if(line[1] == *"t"){
            *state = readingUV;
            return;
        }
        *state = readingPoint;
        return;
    }
    else if(line[0] == *"f"){
        *state = readingIncides;
        return;
    }
    else if(line[0] == *"#") {
        *state = waitingForLine;
        return;
    }
    *state = waitingForLine;
    return;
}

void ObjParser::run(){
    ifstream file(inputfile.c_str(), ifstream::in);
    float buffer[8];
    while (file.good()) {
        stringstream ss(stringstream::in | stringstream::out);
        ss.str(currentLine);
        std::string oper;
        ss >> oper;
        switch (state) {
            case waitingForLine:{
                file.getline(currentLine, 255);
                readLines++;
                    // read symbol
                readSymbol(currentLine, &state);
                break;
            }
            case readingPoint:{
                ss >> buffer[0] >> buffer[1] >> buffer[2];
                points->push_back(Point(buffer[0], buffer[1], buffer[2]));
                state = waitingForLine;
                break;
            }
            case readingNormal:{
                ss >> buffer[0] >> buffer[1] >> buffer[2];
                normals->push_back(Vector(buffer[0], buffer[1], buffer[2]));
                state = waitingForLine;
                break;
            }
            case readingUV:{
                ss >> buffer[0] >> buffer[1];
                uv myUVs = uv(
                    buffer[0],
                    buffer[1]);
                uvs->push_back(myUVs);
                state = waitingForLine;
                break;
            }
            case readingIncides:{
                string edge1;
                string edge2;
                string edge3;
                ss >> edge1 >> edge2 >> edge3;
                    // vert
                string stringBuffer;
                for (int i=0; i < edge1.length(); i++) {
                    if (edge1[i] == *"/") {
                        break;
                    }
                    else {
                        stringBuffer = stringBuffer + edge1[i];
                    }
                }
                vertIndex->push_back(atoi(stringBuffer.c_str()));
                stringBuffer = "";
                for (int i=0; i < edge2.length(); i++) {
                    if (edge2[i] == *"/") {
                        break;
                    }
                    else {
                        stringBuffer = stringBuffer + edge2[i];
                    }
                }
                vertIndex->push_back(atoi(stringBuffer.c_str()));
                
                stringBuffer = "";
                for (int i=0; i < edge3.length(); i++) {
                    if (edge3[i] == *"/") {
                        break;
                    }
                    else {
                        stringBuffer = stringBuffer + edge3[i];
                    }
                }
                vertIndex->push_back(atoi(stringBuffer.c_str()));
                
                    // uv
                bool begin = false;
                stringBuffer = "";
                for (int i=0; i < edge1.length(); i++) {
                    if (begin == true) {
                        if (edge1[i] == *"/") {
                            break;
                        }
                        stringBuffer = stringBuffer + edge1[i];
                    }
                    else if (edge1[i] == *"/") {
                        begin = true;
                    }
                }
                uvIndex->push_back(atoi(stringBuffer.c_str()));
                begin = false;
                stringBuffer = "";
                
                for (int i=0; i < edge2.length(); i++) {
                    if (begin == true) {
                        if (edge2[i] == *"/") {
                            break;
                        }
                        stringBuffer = stringBuffer + edge2[i];
                    }
                    else if (edge2[i] == *"/") {
                        begin = true;
                    }
                }
                uvIndex->push_back(atoi(stringBuffer.c_str()));
                begin = false;
                stringBuffer = "";
                for (int i=0; i < edge3.length(); i++) {
                    if (begin == true) {
                        if (edge3[i] == *"/") {
                            break;
                        }
                        stringBuffer = stringBuffer + edge3[i];
                    }
                    else if (edge3[i] == *"/") {
                        begin = true;
                    }
                }
                uvIndex->push_back(atoi(stringBuffer.c_str()));
                
                    // norm
                size_t found = edge1.find_last_of("/");
                edge1 = edge1.substr(found+1);
                normIndex->push_back(atoi(edge1.c_str()));
                
                found = edge2.find_last_of("/");
                edge2 = edge2.substr(found+1);
                normIndex->push_back(atoi(edge2.c_str()));
                
                found = edge3.find_last_of("/");
                edge3 = edge3.substr(found+1);
                normIndex->push_back(atoi(edge3.c_str()));
                state = waitingForLine;
                break;
            }
            case doneParsing:
                return;
            case error:
                return;
            default:
                state = error;
                break;
        }
    }
}

bool ObjParser::parseFile(){
    state = waitingForLine;
    run();
    if (state == error) {
        return false;
    }
    
    return true;
}