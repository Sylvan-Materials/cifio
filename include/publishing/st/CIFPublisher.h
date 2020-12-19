#pragma once

#include "publishing/st/Publisher.h"


namespace sylvanmats::publishing::st{
    class CIFPublisher : public Publisher {
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
        CIFPublisher(std::filesystem::path& stPath);
        CIFPublisher(const CIFPublisher& orig) = delete;
        virtual ~CIFPublisher() =  default;


        protected:


    };
}
