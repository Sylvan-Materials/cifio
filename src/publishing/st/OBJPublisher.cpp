#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/OBJPublisher.h"


namespace sylvanmats::publishing::st{

    OBJPublisher::OBJPublisher(std::filesystem::path& stPath) : Publisher(stPath) {
    };

}

