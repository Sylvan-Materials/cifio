#pragma once

#include "publishing/st/Publisher.h"

#include "linear/Vector.h"

template <>
struct fmt::formatter<std::vector<sylvanmats::linear::Vector3d>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        value_format= "{:";        
        for (auto it= std::begin(ctx); it != std::end(ctx); ++it) {
            char c= *it;
            if (c == 'c')
                curly= true;
            else
                value_format+= c;
            if (c == '}')
                return it;
        }
        return std::end(ctx);
    }
    
     template <typename FormatContext>
     auto format(const std::vector<sylvanmats::linear::Vector3d>& v, FormatContext& ctx) const -> decltype(ctx.out()){
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
            double x=((sylvanmats::linear::Vector3d)v[i]).x();
            double y=((sylvanmats::linear::Vector3d)v[i]).y();
            double z=((sylvanmats::linear::Vector3d)v[i]).z();
            fmt::vformat_to(ctx.out(), "v "+value_format+" "+value_format+" "+value_format+"\n", fmt::make_format_args(x, y, z));
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
struct fmt::formatter<std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>>>{
    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){
        return ctx.begin();
    }
//    auto parse(format_parse_context& ctx)
//    {
//        value_format= "{:";        
//        for (auto it= std::begin(ctx); it != std::end(ctx); ++it) {
//            char c= *it;
//            if (c == 'c')
//                curly= true;
//            else
//                value_format+= c;
//            if (c == '}')
//                return it;
//        }
//        return std::end(ctx);
//    }
    
     template <typename FormatContext>
     auto format(const std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>>& v, FormatContext& ctx) const -> decltype(ctx.out()){
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
         std::cout<<"indices "<<v.size()<<std::endl;
        for (int i= 0; i < v.size(); ++i){
            size_t i0=std::get<0>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i1=std::get<1>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i2=std::get<2>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i3=std::get<3>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i4=std::get<4>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i5=std::get<5>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i6=std::get<6>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            size_t i7=std::get<7>((std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>)v[i]);
            fmt::vformat_to(ctx.out(), "f {} {} {} {} {} {} {} {}\n", fmt::make_format_args(i0, i1, i2, i3, i4, i5, i6, i7));
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
    class OBJPublisher : public Publisher {
    public:
        using Publisher::render;
    protected:
        std::string objTemplate{};

        size_t vertex_count;
        size_t face_count;
        size_t material_count;
        std::vector<sylvanmats::linear::Vector3d> vertices;
        std::vector<sylvanmats::linear::Vector3d> texture_vertices;
        //normals
        std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>> indices;
    public:
        OBJPublisher(std::filesystem::path& stPath);
        OBJPublisher(const OBJPublisher& orig) = delete;
        virtual ~OBJPublisher() =  default;

        std::string render() override;
        void setVertexCount(size_t vertex_count){this->vertex_count=vertex_count;};
        void setFaceCount(size_t face_count){this->face_count=face_count;};
        void setMaterialCount(size_t material_count){this->material_count=material_count;};
        void append(sylvanmats::linear::Vector3d v){vertices.push_back(v);};
        void appendIndices(std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t> i){indices.push_back(i);};

    protected:


    };
}

