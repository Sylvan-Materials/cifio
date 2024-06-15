#pragma once

#include <cmath>

namespace sylvanmats::density {
    
    enum CRYSTAL_SYSTEM{
        Triclinic,
        Monoclinic,
        Orthorhombic,
        Tetragonal,
        Rhombohedral,
        Hexagonal,
        Cubic
    };
    template<typename T>
    struct cell{
        T a;
        T b;
        T c;
        T α;
        T β;
        T γ;
        
        T volume(CRYSTAL_SYSTEM crystal_system=Cubic){
            T αRadian=std::numbers::pi*α/180.0;
            T βRadian=std::numbers::pi*β/180.0;
            T γRadian=std::numbers::pi*γ/180.0;
            switch(crystal_system){
                case Triclinic:
                    return a*b*c*std::sqrt(1.0-std::pow(std::cos(αRadian),2)-std::pow(std::cos(βRadian),2)-std::pow(std::cos(γRadian),2)+2.0*std::cos(αRadian)*std::cos(βRadian)*std::cos(γRadian));
                    break;
                case Monoclinic:
                    return a*b*c*std::sin(βRadian);
                    break;
                case Orthorhombic:
                    return a*b*c;
                    break;
                case Tetragonal:
                    return std::pow(a, 2)*c;
                    break;
                case Rhombohedral:
                    return std::pow(a, 3)*std::sqrt(1.0-3.0*std::pow(std::cos(αRadian),2)+2.0*std::pow(std::cos(αRadian),3));
                    break;
                case Hexagonal:
                    return (std::sqrt(3.0)/2.0)*std::pow(a, 2)*c;
                    break;
                default:
                    return std::pow(a, 3);
                    break;
            }
        }
    };
}
