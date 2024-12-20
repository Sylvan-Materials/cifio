#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>
#include <valarray>
#include <ranges>

#define protected public
#include "zlib.h"
#include "mio/mmap.hpp"

#include "io/json/Binder.h"

#include "asynchronous/Generator.h"

#include "publishing/st/MOL2Publisher.h"
#include "standards/ComponentStandards.h"
#include "standards/AminoStandards.h"
#include "constitution/Graph.h"
#include "constitution/Populator.h"
#include "constitution/Selection.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "density/ccp4/MapInput.h"
#include "linear/Vector.h"
#include "linear/Matrix.h"
#include "algebra/geometric/Quaternion.h"
#include "algebra/geometric/Motor.h"
#include "publishing/jgf/JGFPublisher.h"

#include "antlr4-runtime.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"

// row-major
template<std::floating_point T>
using Matrix = std::vector<std::vector<T>>;
// column vector
template<std::floating_point T>
using Vector = std::vector<T>;

 
template<std::integral T>
sylvanmats::asynchronous::Generator<T> range(T first, const T last) {
    while (first < last) {
        co_yield first++;
    }
}
 
template<std::floating_point T>
sylvanmats::asynchronous::Generator<T> transposed(const Matrix<T>& mat)
{
    assert(!mat.empty());
    size_t ncols = mat[0].size();
    for(size_t j = 0; j < ncols; ++j)
        for(size_t i = 0; i < mat.size(); ++i)
            co_yield mat[i][j];
}

/*namespace sylvanmats{
    template<std::floating_point T>
    class Matrix : public std::valarray<T>{
        public:
        unsigned int rows=0;
        unsigned int cols=0;
    };
}*/

/*template<std::floating_point Tt>
void sylvanmats::Matrix<Tt>::operator""_^(sylvanmats::Matrix<Tt>& mat){
auto transposed_b = transposed<Tt>(mat);
    //return mat;
}*/

TEST_SUITE("symmetry"){

TEST_CASE("test a matrix based on valarray"){

Matrix<double> B = {{7,  8},
           {9,  10},
           {11, 12}};
    
auto transposed_b = transposed<double>(B);
//  {{7, 9, 11},
//  {8, 10, 12}};
//auto result =co_await transposed_b;
//std::cout << (result);

std::valarray<double> array {7.0,  8.0, 9.0, 10.0, 11.0, 12.0};
    //std::cout<<array[std::slice(0, 3, 1)]<<std::endl;;
    //std::cout<<array[std::slice(3, 3, 1)]<<std::endl;;
//    for (const char i : range(65, 91)) {
//        std::cout << i << ' ';
//    }
//    std::cout << '\n';

    sylvanmats::linear::Matrix3l m {2.0l, 0.0l, 0.0l, 0.0l, 2.0l, 0.0l, 0.0l, 0.0l, 2.0l};
//    std::cout<<"m "<<std::setprecision(12)<<std::endl<<m<<std::endl;
//    std::cout<<"m det "<<std::endl<<m.det()<<std::endl;
//    std::cout<<"m inverse "<<std::endl<<m.inverse()<<std::endl;
    CHECK_EQ(m.rows(), 3);
    CHECK_EQ(m.cols(), 3);
    CHECK_EQ(m.size(), 9);
    sylvanmats::linear::Vector3l v {3.0l, 4.0l, -2.0l};
    CHECK_EQ(v.rows(), 3);
    CHECK_EQ(v.cols(), 1);
    CHECK_EQ(v.size(), 3);
    sylvanmats::linear::Vector3l v2=m*v;//+v3+v4/5 +(m*v).pow(2);
    CHECK_EQ(v2.rows(), 3);
    CHECK_EQ(v2.cols(), 1);
    CHECK_EQ(v2.size(), 3);
    CHECK(v2[0] == doctest::Approx(6.0l));
    CHECK(v2[1] == doctest::Approx(8.0l));
    CHECK(v2[2] == doctest::Approx(-4.0l));
    CHECK(v2.norm() == doctest::Approx(10.77033l));
   CHECK(v2.normalized()[0] == doctest::Approx(0.557086014531156l));
}

TEST_CASE("test a dual quaternion or 4d motor"){
    long double θ = 2.0l*std::numbers::pi/3.0l;
//    std::cout<<(180.0l*θ/2/std::numbers::pi)<<" "<<std::sin(θ/2)<<" "<<std::cos(θ/2)<<std::endl;
    sylvanmats::linear::Vector3l a {0.0l, 1.0l, 0.0l};
    sylvanmats::linear::Vector3l p1 {1.0l, 1.0l, 1.0l};
    sylvanmats::linear::Vector3l p2 {1.0l, 2.0l, 1.0l};
    sylvanmats::linear::Vector3l t = {0.0l, 0.0l, 0.0l};//p1 - p2;
    CHECK(t[0] == doctest::Approx(0.0l));
    CHECK(t[1] == doctest::Approx(0.0l));
    CHECK(t[2] == doctest::Approx(0.0l));
    sylvanmats::linear::Vector3l p {0.0l, 0.0l, 1.0l};
    sylvanmats::algebra::geometric::Quaternion<long double> R(a, θ);
    sylvanmats::algebra::geometric::Quaternion<long double> T(t);
    sylvanmats::algebra::geometric::Motor<long double> M(t, a, θ);
//    std::cout<<"M: "<<M<<std::endl;
//    std::cout<<"M.m: "<<std::endl<<M.m<<std::endl;
    sylvanmats::linear::Vector3l pt = M*p;
//    std::cout<<"pt[0] "<<pt[0]<<std::endl;
//    std::cout<<"pt[1] "<<pt[1]<<std::endl;
//    std::cout<<"pt[2] "<<pt[2]<<std::endl;
    sylvanmats::linear::Vector3l pt2 = M*pt;
    sylvanmats::linear::Vector3l pt3 = M*pt2;
//    std::cout<<"pt3[0] "<<pt3[0]<<std::endl;
//    std::cout<<"pt3[1] "<<pt3[1]<<std::endl;
//    std::cout<<"pt3[2] "<<pt3[2]<<std::endl;

}

TEST_CASE("test a versor based on valarray"){

}

}