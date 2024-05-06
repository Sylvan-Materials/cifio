#pragma once

#include <numbers>
#include <cmath>
#include <tuple>

#include "publishing/st/Publisher.h"


#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

template <>
struct fmt::formatter<std::vector<std::tuple<double, double, double, std::string>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<double, double, double, std::string>>& v, FormatContext& ctx) -> decltype(ctx.out()){
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
            double i0=std::get<0>((std::tuple<double, double, double, std::string>)v[i]);
            double i1=std::get<1>((std::tuple<double, double, double, std::string>)v[i]);
            double i2=std::get<2>((std::tuple<double, double, double, std::string>)v[i]);
            std::string i3=std::get<3>((std::tuple<double, double, double, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "    <circle cx=\"{:>.4f}\" cy=\"{:>.4f}\" r=\"{:>.4f}\" stroke=\"{}\" stroke-width=\"0.2\" fill=\"none\"/>\n", fmt::make_format_args(i0, i1, i2, i3));
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
};

template <>
struct fmt::formatter<std::vector<std::tuple<double, double, double, double, double, double, double, int, int, std::string>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<double, double, double, double, double, double, double, int, int, std::string>>& v, FormatContext& ctx) -> decltype(ctx.out()){
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
            double i0=std::get<0>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i1=std::get<1>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i2=std::get<2>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i3=std::get<3>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i4=std::get<4>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i5=std::get<5>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            double i6=std::get<6>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            int i7=std::get<7>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            int i8=std::get<8>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            std::string i9=std::get<9>((std::tuple<double, double, double, double, double, double, double, int, int, std::string>)v[i]);
            fmt::vformat_to(ctx.out(), "    <g stroke=\"red\"  stroke-width=\"0.2\" fill=\"none\">\n<path d = \"M {:>.4f} {:>.4f} L {:>.4f} {:>.4f} A {:>.4f} {:>.4f} 0 {} {} {:>.4f} {:>.4f} \" stroke=\"{}\"/>\n  </g>\n"/* {:>.4f} L {:>.4f} {:>.4f} A {:>.4f} {:>.4f} 0 {} {} {:>.4f} {:>.4f} \" stroke=\"{}\"/><\n>  </g>\n)"*/, fmt::make_format_args(i0, i1, i3, i4, i2, i2, i7, i8, i5, i6, i9));
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
};

template <>
struct fmt::formatter<std::vector<std::tuple<double, double, std::string, double>>>{
    auto parse(format_parse_context& ctx){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<double, double, std::string, double>>& v, FormatContext& ctx) -> decltype(ctx.out()){
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
            double i0=std::get<0>((std::tuple<double, double, std::string, double>)v[i]);
            double i1=std::get<1>((std::tuple<double, double, std::string, double>)v[i]);
            std::string i2=std::get<2>((std::tuple<double, double, std::string, double>)v[i]);
            double i3=std::get<3>((std::tuple<double, double, std::string, double>)v[i]);
            fmt::vformat_to(ctx.out(), "    <text x=\"{:>.4f}\" y=\"{:>.4f}\" class=\"medium\">{:>.4f}\</text>\n", fmt::make_format_args(i0, i1, i3));
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
};

namespace sylvanmats::publishing::st{
    class SVGPublisher : public Publisher {
    public:
        using Publisher::render;
    protected:
        std::string svgTemplate{};
        
        size_t height=0;
        size_t width=0;
        size_t offset_x=0;
        size_t offset_y=0; 
        std::vector<std::tuple<double, double, double, std::string>> circles;
        //has_text, text_list
        std::vector<std::tuple<double, double, double, double, double, double, double, int, int, std::string>> arcs;
        std::vector<std::tuple<double, double, std::string, double>> labelText;

    public:
        SVGPublisher(std::filesystem::path& stPath);
        SVGPublisher(const SVGPublisher& orig) = delete;
        virtual ~SVGPublisher() =  default;

        std::string render() override;
        void setHeight(size_t height){this->height=height;};
        void setWidth(size_t width){this->width=width;};
        void setXOffset(size_t offset_x){this->offset_x=offset_x;};
        void setYOffset(size_t offset_y){this->offset_y=offset_y;};
        void addCircles(std::string name, std::vector<std::tuple<double, double, double, std::string>>& circles);
        void addWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string>>& wedges);
        void addLabelToWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string, double>>& wedgeText);

        protected:

        inline std::tuple<double, double> polarToCartesian(double centerX, double centerY, double radius, double angleInRadians) {
            return std::make_tuple(centerX + (radius * std::cos(angleInRadians)), centerY + (radius * std::sin(angleInRadians)));
        };

    };
}


