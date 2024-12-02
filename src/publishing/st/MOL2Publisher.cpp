#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/MOL2Publisher.h"


namespace sylvanmats::publishing::st{

    MOL2Publisher::MOL2Publisher(std::filesystem::path& stPath) : Publisher(stPath) {
        std::string templateLocation=(getenv("CIFIO_DB_LOCATION")!=NULL) ? std::string(getenv("CIFIO_DB_LOCATION"))+"/../templates/mol2": "../templates/mol2";
        std::filesystem::path path=templateLocation+"/mol2.txt";
        std::cout<<" "<<path.string()<<std::endl;
        std::ifstream file(path);
        mol2Template=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    };

    std::string MOL2Publisher::render(){
      auto idArg=fmt::arg("entry_id", entry_id);
      auto hArg=fmt::arg("num_atoms", num_atoms);
      auto wArg=fmt::arg("num_bonds", num_bonds);
      auto nsArg=fmt::arg("num_subst", substructures.size());
      auto asArg=fmt::arg("atom_sites", atom_sites);
      auto abArg=fmt::arg("atom_bonds", atom_bonds);
      auto sArg=fmt::arg("substructures", substructures);
        std::string ret=render(mol2Template, fmt::make_format_args(idArg, hArg, wArg, nsArg, asArg, abArg, sArg));
        return ret;
    }
    
}
