#pragma once

#include <ostream>
#include <cmath>
#include "linear/Vector.h"
#include "linear/Matrix.h"

//#include "algebra/geometric/Bivector.h"

namespace sylvanmats::algebra::geometric{

    long double operator"" _e1234 ( long double d )
    {
        return d;
    }

    long double operator"" _e12 ( long double d )
    {
        return d;
    }

    long double operator"" _e23 ( long double d )
    {
        return d;
    }

    long double operator"" _e31 ( long double d )
    {
        return d;
    }

    long double operator"" _e41 ( long double d )
    {
        return d;
    }

    long double operator"" _e42 ( long double d )
    {
        return d;
    }

    long double operator"" _e43 ( long double d )
    {
        return d;
    }

    template<std::floating_point T> class Motor{
    protected:
        T rw=1.0_e1234;
        T uz = 0.0_e12;
        T ux = 0.0_e23;
        T uy = 0.0_e31;
        T rx = 0.0_e41;
        T ry = 0.0_e42;
        T rz = 0.0_e43;
        T uw=1.0l;

        linear::Matrix<T, 4, 4> m;
    public:
        Motor(){};
        Motor(linear::Vector<T, 3> t, linear::Vector<T, 3> a, T θ) : ux (t[0]/2), uy (t[1]/2), uz (t[2]/2), rx (a[0]*std::sin(θ/2)), ry (a[1]*std::sin(θ/2)), rz (a[2]*std::sin(θ/2)), rw (std::cos(θ/2)) {
            if(t[0]==0.0 && t[1]==0.0 && t[2]==0.0)
                uw=0.0l;
            m[0]=1.0l-2.0l*ry*ry-2.0l*rz*rz;        //[0,0]
            m[1]=2.0l*(rx*ry-rz*rw);
            m[2]=2.0l*(rz*rx+ry*rw);
            m[3]=2.0l*(rw*ux-rx*uw+ry*uz-rz*uy);
            m[4]=2.0l*(rx*ry+rz*rw);                //[1,0]
            m[5]=1.0l-2.0l*rz*rz-2.0l*rx*rx;
            m[6]=2.0l*(ry*rz-rx*rw);
            m[7]=2.0l*(rw*uy-ry*uw+rz*ux-rx*uz);
            m[8]=2.0l*(rz*rx-ry*rw);                //[2,0]
            m[9]=2.0l*(ry*rz+rx*rw);
            m[10]=1.0l-2.0l*rx*rx-2.0l*ry*ry;
            m[11]=2.0l*(rw*uz-rz*uw+rx*uy-ry*ux);
            m[12]=0.0l;                             //[3,0]
            m[13]=0.0l;
            m[14]=0.0l;
            m[15]=1.0l;
        };
        //Motor(linear::Vector<T, 3> t) : {};
        Motor(const Motor& orig) = default;
        virtual ~Motor() = default;
        public:

        template<std::floating_point U = T, signed long long V = 3> linear::Vector<U, V> operator * (const linear::Vector<U, V>& v){
            linear::Vector<T, 4> vin {v[0], v[1], v[2], 1.0l};
            linear::Vector<T, 4> vout = this->m*vin;
            linear::Vector<T, 3> v2 {vout[0], vout[1], vout[2]};
            return v2;
        };

        friend std::ostream& operator<<(std::ostream& s, Motor<T>& r) {
          s << r.ux << "(e2∧e3) + " << r.uy << "(e3∧e1) + " << r.uz << "(e1∧e2)" << " + " << r.rx << "(e4∧e1)" << " + " << r.ry << "(e4∧e2)" << " + " << r.rz << "(e4∧e3)" << " + " << r.rw << "(e1∧e2∧e3∧e4)";
          return s;
        }

    };
    typedef Motor<double> Motord;
    typedef Motor<long double> Motorl;
}

