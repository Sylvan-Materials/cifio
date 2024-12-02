#include <filesystem>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
//#include <format>

#include "publishing/st/Publisher.h"
//#include "utils/JVMSingleton.h"

namespace sylvanmats::publishing::st{

    Publisher::Publisher(std::filesystem::path& stPath) : stPath (stPath){

    };

}

