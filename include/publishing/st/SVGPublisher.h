#pragma once

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

        protected:


    };
}


