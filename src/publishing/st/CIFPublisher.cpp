#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/st/CIFPublisher.h"


namespace sylvanmats::publishing::st{

    CIFPublisher::CIFPublisher(std::filesystem::path& stPath) : Publisher(stPath){
    };

}
