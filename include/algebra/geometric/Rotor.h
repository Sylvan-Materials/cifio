#pragma once

#include <ostream>
#include "linear/Vector.h"

#include "algebra/geometric/Bivector.h"

namespace sylvanmats::algebra::geometric{
    template<typename Type> class Rotor{
        Type _a=1;
        Type _B01 = 0;
        Type _B12 = 0;
        Type _B20 = 0;
    public:
        /**
        * "Let's remove Quaternions from every 3D Engine" Marc ten Bosch (https://marctenbosch.com/quaternions/)
        **/
        Rotor(){};
        Rotor(Type _a, Type _B01, Type _B12, Type _B20) : _a (_a), _B01 (_B01), _B12 (_B12), _B20 (_B20) {};
        //Rotor(Type _a, Bivector<Type>& bv) : _a (_a), _B01 (bv.B01()), _B12 (bv.B12()), _B20 (bv.B20()) {};
        Rotor(sylvanmats::linear::Vector<Type, 3>& vFrom, sylvanmats::linear::Vector<Type, 3>& vTo){
            _a = 1 + (vTo*vFrom).sum();
            Bivector<Type> minusb(vTo, vFrom);
            _B01 = minusb.B01();
            _B12 = minusb.B12();
            _B20 = minusb.B20();
            normalize();
        };
        Rotor(Type angle, sylvanmats::linear::Vector<Type, 3>& vFrom, sylvanmats::linear::Vector<Type, 3>& vTo){
            Bivector<Type> bvPlane(Bivector<Type>(vFrom, vTo));
            Type sina = std::sin(angle/2.0);
            _a = std::cos(angle/2.0);
            _B01 = -sina * bvPlane.B01();
            _B12 = -sina * bvPlane.B12();
            _B20 = -sina * bvPlane.B20();
        };
        Rotor(Type angle, Bivector<Type>& bvPlane){
            Type sina = std::sin(angle/2.0);
            _a = std::cos(angle/2.0);
            _B01 = -sina * bvPlane.B01();
            _B12 = -sina * bvPlane.B12();
            _B20 = -sina * bvPlane.B20();
        };
        
        Rotor<Type> operator*(Rotor<Type>& q){
            Type __a = _a*q.a() - _B01*q.B01() - _B12*q.B12() - _B20*q.B20();
            Type b01 = _B01*q.a() + _a*q.B01() + _B20*q.B12() - _B12*q.B20();
            Type b12 = _B12*q.a() + _a*q.B12() - _B20*q.B01() + _B01*q.B20();
            Type b20 = _B20*q.a() + _a*q.B20() + _B12*q.B01() - _B01*q.B12();
            return Rotor<Type>(__a, b01, b12, b20);
        };
        
        sylvanmats::linear::Vector<Type, 3> rotate(sylvanmats::linear::Vector<Type, 3>& x){
            sylvanmats::linear::Vector<Type, 3> q;
            q[0] = _a*x[0] + x[1]*_B01 + x[2]*_B12;
            q[1] = _a*x[1] - x[0]*_B01 + x[2]*_B20;
            q[2] = _a*x[2] - x[0]*_B12 - x[1]*_B20;
            Type q012 = -x[0]*_B20 + x[1]*_B12 - x[2]*_B01;
            //std::cout<<"q012 "<<q012<<std::endl;
            sylvanmats::linear::Vector<Type, 3> r;
            r[0] = _a*q[0] + q[1]*_B01 + q[2]*_B12 - q012*_B20;
            r[1] = _a*q[1] - q[0]*_B01 + q012*_B12 + q[2]*_B20;
            r[2] = _a*q[2] - q012*_B01 - q[0]*_B12 - q[1]*_B20;
            return r;
        };

        Rotor<Type> operator*=(const Rotor<Type>& r){
            (*this) = (*this) * r;
            return *this;
        };

        Rotor<Type> rotate(const Rotor<Type>& r){
            return (*this) * r * (*this).reverse();
        };

        Rotor<Type> reverse(){
            return Rotor<Type>(_a, -_B01, -_B12, -_B20);
        };

        Type a(){return _a;};
        Type B01(){return _B01;};
        Type B12(){return _B12;};
        Type B20(){return _B20;};

        Type lengthsqrd(){
            return std::pow(_a, 2) + std::pow(_B01, 2) + std::pow(_B12, 2) + std::pow(_B20, 2);
        };

        Type length(){
            return std::sqrt(lengthsqrd());
        };

        void normalize(){
            Type l = length();
            _a/=l;
            _B01/=l;
            _B12/=l;
            _B20/=l;
        };

        Rotor<Type> normal(){
            Rotor<Type> r = *this; 
            r.normalize();
            return r;
        };

        sylvanmats::linear::Matrix<Type, 3, 3> toMatrix(){
            sylvanmats::linear::Vector<Type, 3> v0 = rotate(sylvanmats::linear::Vector<Type, 3>(1, 0, 0));
            sylvanmats::linear::Vector<Type, 3> v1 = rotate(sylvanmats::linear::Vector<Type, 3>(0, 1, 0));
            sylvanmats::linear::Vector<Type, 3> v2 = rotate(sylvanmats::linear::Vector<Type, 3>(0, 0, 1));
            return sylvanmats::linear::Matrix<Type, 3, 3>(v0, v1, v2);
        };

        friend std::ostream& operator<<(std::ostream& s, Rotor<Type>& r) {
          s << r.B01() << "(e1∧e2) + " << r.B12() << "(e2∧e3) + " << r.B20() << "(e3∧e1)" << " + " << r.a();
          return s;
        }
        
    };
}