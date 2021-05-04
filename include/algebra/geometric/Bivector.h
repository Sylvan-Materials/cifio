#pragma once

#include <ostream>
#include "linear/Vector.h"

namespace sylvanmats::algebra::geometric{
    template<typename Type> class Bivector{
        Type _B01;
        Type _B12;
        Type _B20;
    public:
        Bivector(sylvanmats::linear::Vector<Type, 3>& v1, sylvanmats::linear::Vector<Type, 3>& v2){
            _B01 = v1[0]*v2[1]-v1[1]*v2[0];
            _B12 = v1[1]*v2[2]-v1[2]*v2[1];
            _B20 = v1[2]*v2[0]-v1[0]*v2[2];
        };
        
        void operator+(){
        };

        Type B01(){return _B01;};
        Type B12(){return _B12;};
        Type B20(){return _B20;};

        friend std::ostream& operator<<(std::ostream& s, Bivector<Type>& b) {
          s << b.B01() << "(e1∧e2) + " << b.B12() << "(e2∧e3) + " << b.B20() << "(e3∧e1)";// << " + " << b.w();
          return s;
        }

    };
}
