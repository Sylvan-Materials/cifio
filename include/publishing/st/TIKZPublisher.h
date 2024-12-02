#pragma once

#include "publishing/st/Publisher.h"

#include "linear/Vector.h"

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
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
         std::cout<<"vertices "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string label=std::get<0>((std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>)v[i]);
            std::string name=std::get<1>((std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>)v[i]);
            double x=std::get<2>((std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>)v[i]).x();
            double y=std::get<2>((std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>)v[i]).y();
            double z=std::get<2>((std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>)v[i]).z();
            if(label.empty())
                fmt::vformat_to(ctx.out(), "\\coordinate[] ({})  at ({},{},{});\n", fmt::make_format_args(name, x, y, z));
            else
                fmt::vformat_to(ctx.out(), "\\coordinate[label=above:${}$] ({})  at ({},{},{});\n", fmt::make_format_args(label, name, x, y, z));
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
    bool        curly{false};
    std::string value_format;
};

template <>
struct fmt::formatter<std::vector<std::tuple<std::string, std::string>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<std::string, std::string>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
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
         std::cout<<"vertices "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            std::string iA=std::get<0>((std::tuple<std::string, std::string>)v[i]);
            std::string iB=std::get<1>((std::tuple<std::string, std::string>)v[i]);
            //if(iA)
            fmt::vformat_to(ctx.out(), "\\draw[->,ultra thin] ({}) -- ({});\n", fmt::make_format_args(iA, iB));
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
    bool        curly{false};
    std::string value_format;
};

namespace sylvanmats::publishing::st{
    class TIKZPublisher : public Publisher {
    public:
        using Publisher::render;

    protected:
        std::string tikzTemplate{};

        std::vector<std::tuple<std::string, std::string, sylvanmats::linear::Vector3d>> vertices;
        std::vector<std::tuple<std::string, std::string>> indices;

    public:
        TIKZPublisher(std::filesystem::path& stPath);
        TIKZPublisher(const TIKZPublisher& orig) = delete;
        virtual ~TIKZPublisher() =  default;

        std::string render() override;
        void append(std::string label, std::string name, sylvanmats::linear::Vector3d v){vertices.push_back(std::make_tuple(label, name, v));};
        void draw(std::string label, std::string name){indices.push_back(std::make_tuple(label, name));};
    protected:


    };
}
