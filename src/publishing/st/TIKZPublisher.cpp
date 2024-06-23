#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "publishing/st/TIKZPublisher.h"


namespace sylvanmats::publishing::st{

    TIKZPublisher::TIKZPublisher(std::filesystem::path& stPath) : Publisher(stPath) {
        std::string templateLocation=(getenv("CIFIO_DB_LOCATION")!=NULL) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../templates/tikz": "../templates/tikz";
        std::filesystem::path path=templateLocation+"/tikz.txt";
        std::cout<<" "<<path.string()<<std::endl;
        std::ifstream file(path);
        tikzTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    
    std::string TIKZPublisher::render(){
      //auto vcArg=fmt::arg("vertex_count", vertex_count);
      //auto fcArg=fmt::arg("face_count", face_count);
      //auto mcArg=fmt::arg("material_count", material_count);
      auto cArg=fmt::arg("coordinates", vertices);
      auto dArg=fmt::arg("draws", indices);
        std::string ret=fmt::vformat(tikzTemplate, fmt::make_format_args(cArg, dArg));
        return ret;
    }

}

