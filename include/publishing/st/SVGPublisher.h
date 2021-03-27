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

        void addWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string>>& wedges);
        void addLabelToWedges(std::string name, std::vector<std::tuple<double, double, double, double, double, int, std::string, double>>& wedgeText);

        protected:

        inline std::tuple<double, double> polarToCartesian(double centerX, double centerY, double radius, double angleInRadians) {
            return std::make_tuple(centerX + (radius * std::cos(angleInRadians)), centerY + (radius * std::sin(angleInRadians)));
        };

    };
}


