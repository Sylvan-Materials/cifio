#pragma once

#include <stdlib.h>

#include <filesystem>
#include <functional>
#include <cstring>

namespace sylvanmats::caching {

    class FileSteward{
    public:
        FileSteward() = default;
        FileSteward(const FileSteward& orig) = delete;
        virtual ~FileSteward() = default;
        void operator ()(std::filesystem::path& path, std::function<void(std::string& content)> apply);
    };
}
