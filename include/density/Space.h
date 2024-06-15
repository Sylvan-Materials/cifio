#pragma once

#include "density/cell.h"
#include "linear/Matrix.h"

namespace sylvanmats::density {
    template<typename T>
    struct Space{
        sylvanmats::density::cell<T> cell;
        sylvanmats::linear::Matrix<T, 3, 3> GStar;
        sylvanmats::linear::Matrix<T, 3, 3> G;
        
        sylvanmats::linear::Matrix<T, 3, 3>& reciprocal(CRYSTAL_SYSTEM crystal_system=Cubic){
            T αRadian=std::numbers::pi*cell.α/180.0;
            T βRadian=std::numbers::pi*cell.β/180.0;
            T γRadian=std::numbers::pi*cell.γ/180.0;
            T V=cell.volume(crystal_system);
            T aStar=cell.b*cell.c*std::sin(αRadian)/V;
            T bStar=cell.c*cell.a*std::sin(βRadian)/V;
            T cStar=cell.a*cell.b*std::sin(γRadian)/V;
            std::cout<<"cStar "<<cStar<<std::endl;
            T cosαStar=(std::cos(βRadian)*std::cos(γRadian)-std::cos(αRadian))/(std::sin(βRadian)*std::sin(γRadian));
            T cosβStar=(std::cos(γRadian)*std::cos(αRadian)-std::cos(βRadian))/(std::sin(γRadian)*std::sin(αRadian));
            T cosγStar=(std::cos(αRadian)*std::cos(βRadian)-std::cos(γRadian))/(std::sin(αRadian)*std::sin(βRadian));
            GStar[0, 0]=std::pow(aStar, 2);
            GStar[1, 1]=std::pow(bStar, 2);
            GStar[2, 2]=std::pow(cStar, 2);
            GStar[0, 1]=GStar[1, 0]=aStar*bStar*cosγStar;
            GStar[0, 2]=GStar[2, 0]=aStar*cStar*cosβStar;
            GStar[1, 2]=GStar[2, 1]=bStar*cStar*cosαStar;
            G=GStar.inverse();
            return GStar;
        }
    };
}
