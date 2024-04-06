#pragma once

#include <ostream>
#include <numbers>
#include "linear/Vector.h"

namespace sylvanmats::algebra::geometric{
    template<typename Type> class Bivector{
        Type _B01;
        Type _B12;
        Type _B20;
    public:
        Bivector(Type _B01, Type _B12, Type _B20): _B01 (_B01), _B12 (_B12), _B20(_B20) {};
        Bivector(sylvanmats::linear::Vector<Type, 3>& v1, sylvanmats::linear::Vector<Type, 3>& v2){
            _B01 = v1[0]*v2[1]-v1[1]*v2[0];
            _B12 = v1[1]*v2[2]-v1[2]*v2[1];
            _B20 = v1[2]*v2[0]-v1[0]*v2[2];
        };
        
        void operator+(){
        };
        
        Bivector<Type> operator*(Bivector<Type>& otherB){
            return Bivector(this->B01()*otherB.B01(), this->B12()*otherB.B12(), this->B20()*otherB.B20());
        };
        
        Bivector<Type> cross(Bivector<Type>& otherB){
            return Bivector(this->B12()*otherB.B20()-this->B20()*otherB.B12(), this->B20()*otherB.B01()-this->B01()*otherB.B12(), this->B12()*otherB.B01());
        };
        
        Type norm(){
            Type sum=_B01*_B01+_B12*_B12+_B20*_B20;
             return std::sqrt(sum);
        };
        
        Type dot(Bivector<Type>& otherB){
            Type sum=_B01*otherB.B01()+_B12*otherB.B12()+_B20*otherB.B20();
            return sum;
        };
        
        Type dot(sylvanmats::linear::Vector<Type, 3>& v){
            Type sum=_B01*v[0]+_B12*v[1]+_B20*v[2];
            return sum;
        };
        
        Type angle(Bivector<Type>& otherB){
            Type dot=this->dot(otherB);
            return std::acos(dot/(this->norm()*otherB.norm()))-std::numbers::pi;
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
