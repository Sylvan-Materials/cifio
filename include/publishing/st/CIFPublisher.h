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
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSites;
        std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>> polymers;
        std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>> nonpolymers;
    public:
        CIFPublisher(std::filesystem::path& stPath);
        CIFPublisher(const CIFPublisher& orig) = delete;
        virtual ~CIFPublisher() =  default;

        std::string render() override;
        void setEntryID(std::string entry_id){this->entry_id=entry_id;};
        void insertAtomSites(std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int> as){atomSites.insert(atomSites.begin(), as);};
        void appendAtomSites(std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int> as){atomSites.push_back(as);};
        void insertPolymers(std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string> ps){polymers.insert(polymers.begin(), ps);};
        void appendPolymers(std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string> ps){polymers.push_back(ps);};
        void insertNonpolymers(std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string> nps){nonpolymers.insert(nonpolymers.begin(), nps);};
        void appendNonpolymers(std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string> nps){nonpolymers.push_back(nps);};
        protected:


    };
}
