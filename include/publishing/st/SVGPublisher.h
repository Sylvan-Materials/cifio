#pragma once

#include <numbers>
#include <cmath>

#include "publishing/st/Publisher.h"


#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace sylvanmats::publishing::st{
    class SVGPublisher : public Publisher {
    public:
        using Publisher::render;
    protected:
        std::string svgTemplate{};
        
        size_t height=0;
        size_t width=0;
        //offset_x, offset_y, circles, has_arcs, arcs, has_text, text_list

    public:
        SVGPublisher(std::filesystem::path& stPath);
        SVGPublisher(const SVGPublisher& orig) = delete;
        virtual ~SVGPublisher() =  default;

        std::string render() override;
        void setHeight(size_t height){this->height=height;};
        void setWidth(size_t width){this->width=width;};
        void addWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string>>& wedges);
        void addLabelToWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string, double>>& wedgeText);

        protected:

        inline std::tuple<double, double> polarToCartesian(double centerX, double centerY, double radius, double angleInRadians) {
            return std::make_tuple(centerX + (radius * std::cos(angleInRadians)), centerY + (radius * std::sin(angleInRadians)));
        };

    };
}


