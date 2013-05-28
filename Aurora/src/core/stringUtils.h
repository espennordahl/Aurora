//
//  stringUtils.h
//  Aurora
//
//  Created by Espen Nordahl on 25/12/12.
//
//

#ifndef Aurora_stringUtils_h
#define Aurora_stringUtils_h

#include "constants.h"
#include "log.h"
#define lcontext LOG_StringUtils

#include "json.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unordered_map>



namespace Aurora {
    
    typedef std::unordered_map<std::string, std::string> StringMap;
    
    inline std::string intToString(int number)
    {
        std::stringstream ss;
        ss << number;
        return ss.str();
    }
        
    inline std::string stringTemplate(std::string inputString){

            // build hash map
        StringMap replacementMap;

        Json::Value deserializeRoot;
        Json::Reader reader;
        std::string filename = CONFIGPATH;
        std::ifstream sceneDescription( filename.c_str() );
        if ( !reader.parse(sceneDescription, deserializeRoot) ){
            LOG_ERROR("Couldn't parse file: " << filename);
            return "stringUtilsError";
        }
        else{
            LOG_DEBUG("Parsing json file: " << filename);
            if( deserializeRoot.size() <= 0 ) {
                LOG_ERROR("No keys in json object: " << filename);
                return "stringUtilsError";
            }
            else {
                replacementMap["$BASE"] = deserializeRoot["basepath"].asString();
                replacementMap["$CACHE"] = deserializeRoot["cachepath"].asString();
                replacementMap["$RENDER"] = deserializeRoot["renderpath"].asString();
                replacementMap["$TEXTURE"] = deserializeRoot["texturepath"].asString();
                replacementMap["$HDRI"] = deserializeRoot["hdripath"].asString();
            }
        }
        // loop through keywords and replace
        std::string outputString = inputString;
        bool hasKeys = true;
        while (hasKeys) {
            bool didWork = false;
                // replace string if it's there
            for (StringMap::iterator it = replacementMap.begin();
                 it != replacementMap.end(); ++it){
                size_t pos = outputString.find(it->first);
                if (pos != -1) {
                    outputString.replace(pos, it->first.size(), it->second);
                    didWork = true;
                }
            }
                // check if we're done
            if (outputString.find("$") == -1) {
                hasKeys = false;
            }
                // if we're not done, but nothing changed, error
            else if (!didWork) {
                LOG_ERROR("Can't replace unknown keywords in string: " << outputString);
                return "stringUtilsError";
            }
        }
        
        
            // cleanup: remove double /'s
        size_t doubleSlashPos = outputString.find("//");
        while (doubleSlashPos != -1) {
            outputString.replace(doubleSlashPos, 2, "/");
            doubleSlashPos = outputString.find("//");
        }
        
            // frame number replace
        size_t hashPos = outputString.find("####");
        int currentFrame = 1;
        if (hashPos != -1) {
            bool fileExists = true;
            while (fileExists) {
                std::string tmpFileName = outputString;
                std::string number = intToString(currentFrame);
                while (number.length() < 4) {
                    number = "0" + number;
                }
                tmpFileName.replace(hashPos, 4, number);
                    // find file on disk
                ifstream file(tmpFileName.c_str());
                if (file.good()) {
                    currentFrame++;
                }
                else{
                    fileExists = false;
                    outputString = tmpFileName;
                }
            }
        }
        
        return outputString;
    }
}

#undef lcontext

#endif
