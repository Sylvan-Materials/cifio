#include "caching/FileSteward.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace sylvanmats::caching {
    bool FileSteward::operator ()(std::filesystem::path& path, std::function<void(std::string& content)> apply){
        std::filesystem::path cacheLocation=(getenv("CIFIO_DB_LOCATION")!=nullptr) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../cache/raw": "../cache/raw";
        if(!std::filesystem::exists(cacheLocation))std::filesystem::create_directories(cacheLocation);
        std::filesystem::path rawPath=cacheLocation/path;
        std::filesystem::directory_entry entry{rawPath.string()};
        bool ret=entry.exists();
        if(ret){
            std::ifstream is(rawPath);
            std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            apply(content);
        }
        return ret;
    }
    
    void FileSteward::operator ()(std::filesystem::path& path, std::string& content){
        std::filesystem::path cacheLocation=(getenv("CIFIO_DB_LOCATION")!=nullptr) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../cache/raw": "../cache/raw";
        if(!std::filesystem::exists(cacheLocation))std::filesystem::create_directories(cacheLocation);
        std::filesystem::path rawPath=cacheLocation/path;
        std::filesystem::directory_entry entry{rawPath.string()};
        bool ret=entry.exists();
        if(!ret){
            std::ofstream os(rawPath);
            os<<content;
            os.close();
        }
    }
}