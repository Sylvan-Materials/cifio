#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/MOL2Publisher.h"


namespace sylvanmats::publishing::st{

    MOL2Publisher::MOL2Publisher(std::filesystem::path& stPath) : Publisher(stPath) {
    };

}
