#pragma once

#include <numbers>
#include <cmath>

#include "publishing/st/Publisher.h"


namespace sylvanmats::publishing::st{
    class SVGPublisher : public Publisher {
    protected:
        using Publisher::jniEnv;
        using Publisher::jcls;
        using Publisher::jshortcls;
        using Publisher::jintcls;
        using Publisher::jlongcls;
        using Publisher::jdoublecls;
        using Publisher::stObject;
    public:
        using Publisher::add;

    public:
        SVGPublisher(std::filesystem::path& stPath);
        SVGPublisher(const SVGPublisher& orig) = delete;
        virtual ~SVGPublisher() =  default;

        void addWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string>>& wedges){
            std::vector<std::tuple<double, double, double, double, double, double, double, int, int, std::string>> arcs;
            for(std::tuple<double, double, double, double, double, int, std::string> wedge : wedges){
                int direction = std::get<5>(wedge);
                int large_arc_flag = (std::abs(std::get<4>(wedge)-std::get<3>(wedge))<=std::numbers::pi) ? 0: 1;
                auto [startX, startY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<3>(wedge) : std::get<4>(wedge));
                auto [endX, endY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<4>(wedge) : std::get<3>(wedge));
                std::cout<<direction<<" "<<large_arc_flag<<" alpha "<<std::get<3>(wedge)<<" "<<std::get<4>(wedge)<<" "<<(std::get<4>(wedge)-std::get<3>(wedge))<<std::endl;
                arcs.push_back(std::make_tuple(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), startX, startY, endX, endY, large_arc_flag, direction, std::get<6>(wedge)));
            }
            add(name, arcs);
        };
        protected:

        inline std::tuple<double, double> polarToCartesian(double centerX, double centerY, double radius, double angleInRadians) {
            return std::make_tuple(centerX + (radius * std::cos(angleInRadians)), centerY + (radius * std::sin(angleInRadians)));
        };

    };
}


