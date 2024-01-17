#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <functional>

#include "linear/Array.h"

namespace sylvanmats::minimization {
    
    template<std::floating_point T>
    class QuasiNewton {
    protected:
        sylvanmats::linear::Array<T, -1> x;
        sylvanmats::linear::Array<T, -1> g;
        size_t i=0;
        
        T α=0.5;
        T μ=1.0;
    public:
        QuasiNewton() = delete;
        QuasiNewton(size_t n) : x(n), g(n) {};
        QuasiNewton(const QuasiNewton& orig) = delete;
        virtual ~QuasiNewton() = default;
        
        void initialize(sylvanmats::linear::Array<T, -1>& x){this->x=x;};
        void objective(std::function<T(sylvanmats::linear::Array<T, -1>&)> o){this->o=o;};
        void gradient(std::function<void(sylvanmats::linear::Array<T, -1>&, sylvanmats::linear::Array<T, -1>&)> grad){this->grad=grad;};
        void monitor(std::function<void(size_t i, long double f)> m){this->m=m;};
        void solve(){
            T Φ0=o(x);
            grad(x, g);
            T dΦ=g.norm();
            for(;i<5;++i){
                T Φ=o(x);
                grad(x, g);
                lineSearch(Φ0, dΦ, Φ);
                m(i, Φ);
            }
        };
        
    protected:
        std::function<T(sylvanmats::linear::Array<T, -1>&)> o;
        std::function<void(sylvanmats::linear::Array<T, -1>&, sylvanmats::linear::Array<T, -1>&)> grad;
        std::function<void(size_t i, long double f)> m;
        
        void lineSearch(T Φ0, T dΦ, T Φ){
            T Ψ=Φ-Φ0-μ*dΦ*α;
            
        };
        
    };
}
