#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

namespace sylvanmats::density {

    template<typename T>
    struct _refln {
    unsigned int wavelength_id;
    unsigned int crystal_id;
    unsigned int scale_group_code;
    unsigned int index_h;
    unsigned int index_k;
    unsigned int index_l;
    std::string status;
    T F_meas_au;
    T F_meas_sigma_au;
    T F_calc;
    T phase_calc;
    T fom;
    };

    class Populator{
    protected:
        std::vector<_refln<double>> structureFactors;
    public:
        Populator() = default;
        Populator(const Populator& orig) = delete;
        virtual ~Populator() = default;

        void operator()(std::string& filePath);
        std::vector<_refln<double>>& getStructureFactors(){return structureFactors;};
    };
}

