#include "caching/FileSteward.h"

namespace sylvanmats::caching {
    void FileSteward::operator ()(std::filesystem::path& path, std::function<void(std::string& content)> apply){
        std::filesystem::path cacheLocation=(getenv("CIFIO_DB_LOCATION")!=nullptr) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../cache/raw": "../cache/raw";
        
    }
}