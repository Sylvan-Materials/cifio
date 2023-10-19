#pragma once

#include "publishing/st/Publisher.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace sylvanmats::publishing::st{
    class CIFPublisher : public Publisher {
    public:
        using Publisher::render;

    protected:
        std::string cifTemplate{};
        
        std::string entry_id;
        //atom_sites, pdbx_poly_seq_schemes, pdbx_nonpoly_schemes
    public:
        CIFPublisher(std::filesystem::path& stPath);
        CIFPublisher(const CIFPublisher& orig) = delete;
        virtual ~CIFPublisher() =  default;

        std::string render() override;
        protected:


    };
}
