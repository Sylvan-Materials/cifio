#pragma once

#include "publishing/st/Publisher.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

template <>
struct fmt::formatter<std::vector<std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>>& v, FormatContext& ctx) -> decltype(ctx.out()){
        for (int i= 0; i < v.size(); ++i){
            size_t i0=std::get<0>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            std::string i1=std::get<1>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            double i2=std::get<2>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            double i3=std::get<3>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            double i4=std::get<4>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            std::string i5=std::get<5>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            long long i6=std::get<6>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            std::string i7=std::get<7>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            double i8=std::get<8>((std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<5d} {:<4s} {:>10.6f} {:>10.6f} {:>10.6f} {:<5s} {:>5d} {:<5s} {:>9.6f}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7, i8));
        }
        constexpr char* fmt={""};
        return fmt::format_to(ctx.out(), fmt);
    }
};

template <>
struct fmt::formatter<std::vector<std::tuple<size_t, size_t, size_t, std::string>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<size_t, size_t, size_t, std::string>>& v, FormatContext& ctx) -> decltype(ctx.out()){
        for (int i= 0; i < v.size(); ++i){
            size_t i0=std::get<0>((std::tuple<size_t, size_t, size_t, std::string>)v[i]);
            size_t i1=std::get<1>((std::tuple<size_t, size_t, size_t, std::string>)v[i]);
            size_t i2=std::get<2>((std::tuple<size_t, size_t, size_t, std::string>)v[i]);
            std::string i3=std::get<3>((std::tuple<size_t, size_t, size_t, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<5d} {:>5d} {:>5d} {:<5s}\n", fmt::make_format_args(i0, i1, i2, i3));
        }
        constexpr char* fmt={"\n"};
        return fmt::format_to(ctx.out(), fmt);
    }
};

template <>
struct fmt::formatter<std::vector<std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>>& v, FormatContext& ctx) -> decltype(ctx.out()){
//        const auto&& buf=ctx.out();
//        if(curly){
//            constexpr char* fmt={"{{"};
//            std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt{"["};
//            std::format_to(ctx.out(), fmt);            
//        }
 //        if (v.size() > 0){
//            const std::string_view vf=value_format;
//            size_t d=v[0];
//            std::format_to(ctx.out(), vf, d);
//        }
        for (int i= 0; i < v.size(); ++i){
            size_t i0=std::get<0>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            std::string i1=std::get<1>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            size_t i2=std::get<2>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            std::string i3=std::get<3>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            size_t i4=std::get<4>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            std::string i5=std::get<5>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            std::string i6=std::get<6>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            size_t  i7=std::get<7>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            std::string i8=std::get<8>((std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<5d} {:<5s} {:>5d} {} {:>5d} {:<2s} {:<7s} {:>5d} {:<s}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7, i8));
        }
        constexpr char* fmt={"\n"};
        return fmt::format_to(ctx.out(), fmt);
//        if(curly){
//            constexpr char* fmt={"}}"};
//            return std::format_to(ctx.out(), fmt);
//        }
//        else{
//            constexpr char* fmt={"]"};
//            return std::format_to(ctx.out(), fmt);
//        }
    }
    // ...
//    bool        curly{false};
//    std::string value_format;
};

namespace sylvanmats::publishing::st{
    class MOL2Publisher : public Publisher {
    public:
        using Publisher::render;
    protected:
        std::string mol2Template{};

        std::string entry_id;
        size_t num_atoms=0;
        size_t num_bonds=0;
        size_t num_subst=1;
        std::vector<std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double>> atom_sites;
        std::vector<std::tuple<size_t, size_t, size_t, std::string>> atom_bonds;
        std::vector<std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string>> substructures;
    public:
        MOL2Publisher(std::filesystem::path& stPath);
        MOL2Publisher(const MOL2Publisher& orig) = delete;
        virtual ~MOL2Publisher() =  default;

        std::string render() override;
        void setEntryID(std::string entry_id){this->entry_id=entry_id;};
        void setNumberOfAtoms(size_t num_atoms){this->num_atoms=num_atoms;};
        void setNumberOfBonds(size_t num_bonds){this->num_bonds=num_bonds;};
        void insertAtomSites(std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double> as){atom_sites.insert(atom_sites.begin(), as);};
        void appendAtomSites(std::tuple<size_t, std::string, double, double, double, std::string, long long, std::string, double> as){atom_sites.push_back(as);};
        void insertAtomBonds(std::tuple<size_t, size_t, size_t, std::string> atom_bonds){this->atom_bonds.insert(this->atom_bonds.begin(), atom_bonds);};
        void appendAtomBonds(std::tuple<size_t, size_t, size_t, std::string> atom_bonds){this->atom_bonds.push_back(atom_bonds);};
        void insertSubstructures(std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string> substructures){this->substructures.insert(this->substructures.begin(), substructures);};
        void appendSubstructures(std::tuple<size_t, std::string, size_t, std::string, size_t, std::string, std::string, size_t, std::string> substructures){this->substructures.push_back(substructures);};
        
        protected:


    };
}


