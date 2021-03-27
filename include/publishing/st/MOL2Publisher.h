#pragma once

#include "publishing/st/Publisher.h"


namespace sylvanmats::publishing::st{
    class MOL2Publisher : public Publisher {
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
        MOL2Publisher(std::filesystem::path& stPath);
        MOL2Publisher(const MOL2Publisher& orig) = delete;
        virtual ~MOL2Publisher() =  default;

        protected:


    };
}


