#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/CIFPublisher.h"


namespace sylvanmats::publishing::st{

    CIFPublisher::CIFPublisher(std::filesystem::path& stPath) : Publisher(stPath){
        std::string templateLocation=(getenv("CIFIO_DB_LOCATION")!=nullptr) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../templates/cif": "../templates/cif";
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
      auto asArg=fmt::arg("atom_sites", atomSites);
      auto pArg=fmt::arg("polymers", polymers);
      auto npArg=fmt::arg("nonpolymers", nonpolymers);
        std::string ret=fmt::vformat(cifTemplate, fmt::make_format_args(idArg, asArg, pArg, npArg));
        return ret;
    }
}
