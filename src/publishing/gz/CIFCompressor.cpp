#include <fstream>
#include <string>
#include <cstring>

#include "zlib.h"

#include "publishing/gz/CIFCompressor.h"

namespace sylvanmats::publishing{

    void CIFCompressor::operator()(std::string& name, std::istream& is){
        is.seekg(0); // rewind
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        std::string path="./examples/"+name+".cif.gz";
        gzFile file=gzopen(path.c_str(), "wb");
        int uncomprLen=1024;
        char buf[uncomprLen+1];
        std::memset (buf, 0, uncomprLen+1);
        bool eofHit=false;
        unsigned long long count=0;
        while(!eofHit && uncomprLen>0){
            std::memset (buf, 0, uncomprLen+1);
            std:strncpy(buf, &content[count], uncomprLen);
            buf[uncomprLen+1]=0;
            int ret=gzputs(file, (char*)buf);
            if(ret<=0)eofHit=true;
            else count+=ret;
            if(uncomprLen>content.size()-count)uncomprLen=content.size()-count;
            //std::cout<<ret<<" "<<eofHit<<" "<<count<<" "<<content.size()<<" "<<uncomprLen<<std::endl;
        }
        gzclose(file);
    }

}