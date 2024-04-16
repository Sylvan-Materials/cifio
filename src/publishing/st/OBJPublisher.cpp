#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "publishing/st/OBJPublisher.h"


namespace sylvanmats::publishing::st{

    OBJPublisher::OBJPublisher(std::filesystem::path& stPath) : Publisher(stPath) {
        std::string templateLocation=(getenv("CIFIO_DB_LOCATION")!=NULL) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../templates/obj": "../templates/obj";
        std::filesystem::path path=templateLocation+"/obj.txt";
        std::cout<<" "<<path.string()<<std::endl;
        std::ifstream file(path);
        objTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    
    std::string OBJPublisher::render(){
      auto vcArg=fmt::arg("vertex_count", vertex_count);
      auto fcArg=fmt::arg("face_count", face_count);
      auto mcArg=fmt::arg("material_count", material_count);
      auto vArg=fmt::arg("vertices", vertices);
        std::string ret=fmt::vformat(objTemplate, fmt::make_format_args(vcArg, fcArg, mcArg, vArg));
        return ret;
    }

}

