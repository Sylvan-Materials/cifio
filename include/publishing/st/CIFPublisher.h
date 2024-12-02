#pragma once

#include "publishing/st/Publisher.h"

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
        for (int i= 0; i < v.size(); ++i){
            std::string i0=std::get<0>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            size_t i1=std::get<1>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i2=std::get<2>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i3=std::get<3>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i4=std::get<4>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i5=std::get<5>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i6=std::get<6>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            long long i7=std::get<7>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            long long i8=std::get<8>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i9=std::get<9>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            double i10=std::get<10>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            double i11=std::get<11>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            double i12=std::get<12>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            double i13=std::get<13>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            double i14=std::get<14>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            short i15=std::get<15>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            int i16=std::get<16>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i17=std::get<17>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i18=std::get<18>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            std::string i19=std::get<19>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            int i20=std::get<20>((std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<4s} {:<5d} {:<4s} {:<4s} {:<4s} {:<4s} {:<4s} {:<5d} {:<5d} {:<4s} {:>10.6f} {:>10.6f} {:>10.6f} {:>10.6f} {:>10.6f} {:>5d} {:>5d} {:<5s} {:<5s} {:<5s} {:>5d}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12 ,i13, i14, i15, i16, i17, i18, i19, i20));
        }
        constexpr char* fmt={""};
        return fmt::format_to(ctx.out(), fmt);
    }
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
        for (int i= 0; i < v.size(); ++i){
            std::string i0=std::get<0>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            long long i1=std::get<1>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            long long i2=std::get<2>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i3=std::get<3>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            long long i4=std::get<4>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            long long i5=std::get<5>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            long long i6=std::get<6>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i7=std::get<7>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i8=std::get<8>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i9=std::get<9>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i10=std::get<10>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            std::string i11=std::get<11>((std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<4s} {:<5d} {:<5d} {:<4s} {:<5d} {:<5d} {:<5d} {:<4s} {:<4s} {:<4s} {:<4s} {:<4s}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11));
        }
        constexpr char* fmt={""};
        return fmt::format_to(ctx.out(), fmt);
    }
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
        for (int i= 0; i < v.size(); ++i){
            std::string i0=std::get<0>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            long long i1=std::get<1>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            std::string i2=std::get<2>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            long long i3=std::get<3>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            long long i4=std::get<4>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            long long i5=std::get<5>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            std::string i6=std::get<6>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            std::string i7=std::get<7>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            std::string i8=std::get<8>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            std::string i9=std::get<9>((std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "{:<4s} {:<5d} {:<4s} {:<5d} {:<5d} {:<5d} {:<4s} {:<4s} {:<4s} {:<4s}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));
        }
        constexpr char* fmt={""};
        return fmt::format_to(ctx.out(), fmt);
    }
};

namespace sylvanmats::publishing::st{
    class CIFPublisher : public Publisher {
    public:
        using Publisher::render;

    protected:
        std::string cifTemplate{};
        
        std::string entry_id;
        std::vector<std::tuple<std::string, unsigned long long, std::string, std::string, std::string, std::string, std::string, long long, long long, std::string, double, double, double, double, double, short, int, std::string, std::string, std::string, int>> atomSites;
        std::vector<std::tuple<std::string, long long, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string, std::string>> polymers;
        std::vector<std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string>> nonpolymers;
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
        void insertNonpolymers(std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string> nps){nonpolymers.insert(nonpolymers.begin(), nps);};
        void appendNonpolymers(std::tuple<std::string, long long, std::string, long long, long long, long long, std::string, std::string, std::string, std::string> nps){nonpolymers.push_back(nps);};
        protected:


    };
}
