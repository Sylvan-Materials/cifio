#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>

#include "graph/container/dynamic_graph.hpp"

#include "minimization/QuasiNewton.h"

TEST_SUITE("optimization"){

TEST_CASE("outter product"){
    sylvanmats::linear::Vector2d Δx(1.0, -1.0);
    sylvanmats::linear::Vector2d Δy(0.5, -0.5);
    sylvanmats::linear::Matrix2d B;
    sylvanmats::linear::Matrix2d Bd;
    sylvanmats::linear::Matrix2d Br;
    std::cout<<"Δx: "<<Δx<<std::endl;
    std::cout<<"Δy.transpose(): "<<Δy.transpose()<<std::endl;
    B=Δx*Δy.transpose();
    std::cout<<"B: "<<B<<std::endl;
    Bd=Δy.transpose()*Δx;
    std::cout<<"Bd: "<<Bd<<std::endl; \
    Br=B/Bd;
    std::cout<<"B/Bd: "<<Br<<std::endl;
    
}

TEST_CASE("test lbfgs"){
    sylvanmats::minimization::QuasiNewton<long double> qn(2);
    sylvanmats::linear::ArrayXl x(2);
    x[0]=1.0l;
    x[1]=0.0l;
    qn.initialize(x);
    qn.objective([](sylvanmats::linear::ArrayXl& x)->long double{
        return x[0]-x[1]+2.0l*x[0]*x[1]+2.0l*std::pow(x[0], 2)+std::pow(x[1], 2);
    });
    qn.gradient([](sylvanmats::linear::ArrayXl& x, sylvanmats::linear::ArrayXl& g){
        g[0]=1.0l+2.0l*x[1]+4.0l*x[0];
        g[1]=1.0l+2.0l*x[0]+2.0l*x[1];
    });
    
    qn.monitor([](size_t i, long double f){
        std::cout<<i<<" "<<f<<std::endl;
    });
    x=qn.solve();
    std::cout<<"x= "<<x<<std::endl;
}

}
