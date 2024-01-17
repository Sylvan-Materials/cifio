#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>


#include "minimization/QuasiNewton.h"

TEST_SUITE("optimization"){

TEST_CASE("test lbfgs"){
    sylvanmats::minimization::QuasiNewton<long double> qn(2);
    sylvanmats::linear::ArrayXl x(2);
    x[0]=0.0l;
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
    qn.solve();
}

}
