#pragma once

#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include <cstdarg>

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"


namespace sylvanmats::publishing::st{
    class Publisher{
    protected:
        std::filesystem::path stPath;

        std::unordered_map<short, std::string> tupleMap = {{2, "Pair"}, {3, "Triplet"},{4, "Quartet"},{5, "Quintet"},{6, "Sextet"},{7, "Septet"},{8, "Octet"},{9, "Ennead"},{10, "Decade"},{11, "Hendecad"},{12, "Dodecad"},{21, "TwentyOne"}};

    public:
        Publisher() = delete;
        Publisher(std::filesystem::path& stPath);
        Publisher(const Publisher& orig) = delete;
        virtual ~Publisher() =  default;

//        virtual void add(std::string name, const char* value);
//        virtual void add(std::string name, std::string value);
//        virtual void add(std::string name, bool value);
//        virtual void add(std::string name, long long value);
//        virtual void add(std::string name, unsigned long long value);
//        virtual void add(std::string name, double value);
//        virtual void rawSetAttribute(std::string name, bool value);

        virtual std::string render() = 0;

        protected:

        std::string render(std::string_view users_fmt, fmt::format_args&& args){
           return fmt::vformat(users_fmt, args);
        }
    };
}

