#pragma once

#include <string>
#include <istream>

namespace sylvanmats::publishing{
    class CIFCompressor{
    protected:
    public:
        CIFCompressor() =  default;
        CIFCompressor(const CIFCompressor& orig) = delete;
        virtual ~CIFCompressor() = default;

        void operator()(std::string& name, std::istream& content);
    };
}
