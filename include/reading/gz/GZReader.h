#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "zlib.h"

namespace sylvanmats::reading{

    class GZReader{
    protected:
    public:
        GZReader() = default;
        GZReader(const GZReader& orig) = delete;
        virtual ~GZReader() = default;

        void operator()(std::string& filePath, std::function<void(std::istream& content)> apply){
            if(filePath.rfind("~/", 0)==0){
                const char* home = getenv("HOME");
                if (home) {
                    filePath=home+filePath.substr(1);
                }
            }
            std::filesystem::path path=filePath;
            if(std::filesystem::exists(path)){
                if(path.extension().compare(".gz")==0){
                    std::stringstream ss;
                    gzFile file=gzopen(path.c_str(), "rb");
                    int uncomprLen=1024;
                    char buf[1024];
                    bool eofHit=false;
                    while(!eofHit && uncomprLen>0){
                        char * ret=gzgets(file, (char*)buf, (int)uncomprLen);
                        if(ret==NULL)eofHit=true;
                        else ss<<ret;
                    }
                    ss.seekg(0, std::ios::end);
                    int size = ss.tellg();
                    ss.seekg(0, std::ios::beg);
                    gzclose(file);
                    apply(ss);
                }
                else{
                    std::ifstream file(path.c_str());
                    //std::stringstream content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    apply(file);
                    file.close();
                }
            }
        };

    };
}
