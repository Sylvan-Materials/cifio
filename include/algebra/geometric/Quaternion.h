#pragma once

#include <ostream>
#include <cmath>
#include "linear/Vector.h"

//#include "algebra/geometric/Bivector.h"

namespace sylvanmats::algebra::geometric{
    template<std::floating_point T> class Quaternion{
    protected:
        T _e1234=1;
        T _e12 = 0;
        T _e23 = 0;
        T _e31 = 0;
        T _e41 = 0;
        T _e42 = 0;
        T _e43 = 0;
    public:
        Quaternion(){};
        Quaternion(linear::Vector<T, 3> a, T θ) : _e41 (a[0]*std::sin(θ/2)), _e42 (a[1]*std::sin(θ/2)), _e43 (a[2]*std::sin(θ/2)), _e1234 (std::cos(θ/2)) {};
        Quaternion(linear::Vector<T, 3> t) : _e23 (t[0]/2), _e31 (t[1]/2), _e12 (t[2]/2) {};
        Quaternion(const Quaternion& orig) = default;
        virtual ~Quaternion() = default;

    };
}
