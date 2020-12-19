#pragma once

#include "publishing/st/Publisher.h"


namespace sylvanmats::publishing::st{
    class OBJPublisher : public Publisher {
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
        OBJPublisher(std::filesystem::path& stPath);
        OBJPublisher(const OBJPublisher& orig) = delete;
        virtual ~OBJPublisher() =  default;

        protected:


    };
}

