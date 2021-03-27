#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/SVGPublisher.h"


namespace sylvanmats::publishing::st{

    SVGPublisher::SVGPublisher(std::filesystem::path& stPath) : Publisher(stPath) {
        add("has_arcs", false);
        add("has_text", false);
    };

    void SVGPublisher::addWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string>>& wedges){
        std::vector<std::tuple<double, double, double, double, double, double, double, int, int, std::string>> arcs;
        for(std::tuple<double, double, double, double, double, int, std::string> wedge : wedges){
            int direction = std::get<5>(wedge);
            int large_arc_flag = (std::abs(std::get<4>(wedge)-std::get<3>(wedge))<=std::numbers::pi) ? 0: 1;
            auto [startX, startY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<3>(wedge) : std::get<4>(wedge));
            auto [endX, endY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<4>(wedge) : std::get<3>(wedge));
            //std::cout<<direction<<" "<<large_arc_flag<<" alpha "<<std::get<3>(wedge)<<" "<<std::get<4>(wedge)<<" "<<(std::get<4>(wedge)-std::get<3>(wedge))<<std::endl;
            arcs.push_back(std::make_tuple(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), startX, startY, endX, endY, large_arc_flag, direction, std::get<6>(wedge)));
        }
        add(name, arcs);
    };
    void SVGPublisher::addLabelToWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string, double>>& wedgeText){
        std::vector<std::tuple<double, double, double>> labelText;
        for(std::tuple<double, double, double, double, double, int, std::string, double> wedge : wedgeText){
            int direction = std::get<5>(wedge);
            int large_arc_flag = (std::abs(std::get<4>(wedge)-std::get<3>(wedge))<=std::numbers::pi) ? 0: 1;
            //auto [startX, startY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<3>(wedge) : std::get<4>(wedge));
            //auto [endX, endY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), (direction==1 ) ? std::get<4>(wedge) : std::get<3>(wedge));
            double aveAngle = (std::get<3>(wedge)+std::get<4>(wedge))/2.0;
            auto [startX, startY] = polarToCartesian(std::get<0>(wedge), std::get<1>(wedge), std::get<2>(wedge), aveAngle);
            labelText.push_back(std::make_tuple(startX, startY, std::get<7>(wedge)));
        }
        add(name, labelText);
    };
}


