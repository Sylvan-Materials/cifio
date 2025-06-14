#pragma once

#include <stdlib.h>
#include <string>
#include <memory>
#include <functional>

#include "linear/Vector.h"
#include "linear/Array.h"

namespace sylvanmats::minimization {
    
    /**
     * 
     */
    template<std::floating_point T>
    class QuasiNewton {
    protected:
        sylvanmats::linear::Array<T, -1> x;
        sylvanmats::linear::Array<T, -1> g;
        sylvanmats::linear::Array<T, -1> d;
        size_t i=0;
        
        T α=0.5;
        T μ=1.0;
        T γ=0.8;
        T c=0.1;
    public:
        QuasiNewton() = delete;
        /**
         * 
         */
        QuasiNewton(size_t n) : x(n), g(n), d(n) {};
        QuasiNewton(const QuasiNewton& orig) = delete;
        virtual ~QuasiNewton() = default;
        
        void initialize(sylvanmats::linear::Array<T, -1>& x){this->x=x;};
        void objective(std::function<T(sylvanmats::linear::Array<T, -1>&)> o){this->o=o;};
        void gradient(std::function<void(sylvanmats::linear::Array<T, -1>&, sylvanmats::linear::Array<T, -1>&)> grad){this->grad=grad;};
        void monitor(std::function<void(size_t i, long double f)> m){this->m=m;};
        sylvanmats::linear::Array<T, -1>& solve(){
            T Φ=o(x);
            T t=backtrackingLineSearch(Φ);
            sylvanmats::linear::Array<T, -1> xminus1(x.rows());
            xminus1=x;
            sylvanmats::linear::Array<T, -1> gkminus1(g.rows());
            grad(xminus1, gkminus1);
            x=xminus1+t*d;
            m(i, o(x));
            sylvanmats::linear::Array<T, -1> xplus1(x.rows());
            i++;
            sylvanmats::linear::Matrix<T, -1 , -1> B=typename sylvanmats::linear::Matrix<T, -1, -1>::Identity(0.0, x.rows(), x.rows());
            sylvanmats::linear::Matrix<T, -1 , -1> Bplus=typename sylvanmats::linear::Matrix<T, -1, -1>::Identity(0.0, x.rows(), x.rows());
            for(;i<4;++i){
                grad(x, g);
                sylvanmats::linear::Matrix<T, -1 , -1> I=typename sylvanmats::linear::Matrix<T, -1, -1>::Identity(0.0, x.rows(), x.rows());
                sylvanmats::linear::Vector<T, -1> Δx=(x-xminus1).vector();
                sylvanmats::linear::Vector<T, -1> Δy=(g-gkminus1).vector();
                //std::cout<<"I "<<I<<std::endl;
                //std::cout<<"Δx "<<Δx<<" Δy "<<Δy<<std::endl<<B<<std::endl;
                //sylvanmats::linear::Vector<T, -1 , -1> vv=Δx*Δy.transpose();
                //std::cout<<"left vv "<<vv<<std::endl;
                //Bplus=Δx*Δy.transpose();
                //std::cout<<"left numerator "<<Bplus<<std::endl;
                //Bplus=Δy.transpose()*Δx;
                //std::cout<<"left denominator "<<Bplus<<std::endl;
                //Bplus=(I-(Δx*Δy.transpose())/(Δy.transpose()*Δx));
                //std::cout<<"left "<<Bplus<<std::endl;
                //Bplus=(I-(Δy*Δx.transpose())/(Δy.transpose()*Δx));
                //std::cout<<"right "<<Bplus<<std::endl;
                //Bplus=(Δx*Δx.transpose())/(Δy.transpose()*Δx);
                //std::cout<<"term2 "<<Bplus<<std::endl;
                Bplus=(I-(Δx*Δy.transpose())/(Δy.transpose()*Δx))*B*(I-(Δy*Δx.transpose())/(Δy.transpose()*Δx))+(Δx*Δx.transpose())/(Δy.transpose()*Δx);
                std::cout<<"Bplus "<<Bplus<<std::endl;
                xplus1=x-(Bplus*g.vector()).array();
                m(i, o(xplus1));
                xminus1=x;
                gkminus1=g;
                x=xplus1;
                B=Bplus;
                /*T Φ=o(x);
                grad(x, g);
                lineSearch(Φ0, dΦ, Φ);
                m(i, Φ);*/
            }
            return x;
        };
        
    protected:
        std::function<T(sylvanmats::linear::Array<T, -1>&)> o;
        std::function<void(sylvanmats::linear::Array<T, -1>&, sylvanmats::linear::Array<T, -1>&)> grad;
        std::function<void(size_t i, long double f)> m;
        
        void lineSearch(T Φ0, T dΦ, T Φ){
            T αl=0.0;
            T αu=1.0;
            T Ψl=Ψ(Φ0, dΦ, Φ, αl);
            T Ψu=Ψ(Φ0, dΦ, Φ, αu);
            if(Ψl<=Ψu && Ψl<=0 && Ψprime(dΦ, Φ)*(αu-αl)<0){

            }
        };

        T backtrackingLineSearch(T Φ){
            T fc=o(x);
            grad(x, g);
            T df=c*g.norm();
            d=-g;
            sylvanmats::linear::Array<T, -1> newx(g.rows());
            newx=x+d;
            T newf=o(newx);
            T t=1.0;
            while(newf>(fc+t*df)){
                t=γ*t;
                newx=x+t*d;
                newf=o(newx);
            }
            return t;
        };

        inline T Ψ(T Φ0, T dΦ, T Φ, T α){return Φ-Φ0-μ*dΦ*α;}
        inline T Ψprime(T dΦ, T Φ){return Φ-μ*dΦ;}
        
    };
}
