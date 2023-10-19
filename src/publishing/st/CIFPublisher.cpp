#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/CIFPublisher.h"


namespace sylvanmats::publishing::st{

    CIFPublisher::CIFPublisher(std::filesystem::path& stPath) : Publisher(stPath){
        std::string templateLocation=(getenv("CIFIO_DB_LOCATION")!=NULL) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../templates/cif": "../templates/cif";
        std::filesystem::path path=templateLocation+"/cif.txt";
        std::cout<<" "<<path.string()<<std::endl;
        std::ifstream file(path);
        cifTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    std::string CIFPublisher::render(){
      auto idArg=fmt::arg("entry_id", entry_id);
//      auto wArg=fmt::arg("num_bonds", num_bonds);
//      auto mcArg=fmt::arg("material_count", material_count);
//      auto vArg=fmt::arg("vertices", vertices);
        std::string ret=fmt::vformat(cifTemplate, fmt::make_format_args(idArg));
        return ret;
    }
}
