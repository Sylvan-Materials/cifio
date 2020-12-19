#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/SVGPublisher.h"


namespace sylvanmats::publishing::st{

    SVGPublisher::SVGPublisher(std::filesystem::path& stPath) : Publisher(stPath) {
    };

}


