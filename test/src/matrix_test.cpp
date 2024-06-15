#include <doctest/doctest.h>

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <utility>
#include <charconv>
#include <valarray>
#include <complex>
#include <ranges>
#include <type_traits>

#define protected public

#include "linear/Array.h"
#include "linear/Vector.h"
#include "linear/Matrix.h"
#include "linear/Tensor.h"
#include "domain/DiscreteFourierTransform.h"

TEST_SUITE("matrix"){

TEST_CASE("Construct vectors, arrays and matrices"){
    sylvanmats::linear::ArrayXd arr(5);
    arr+=1.0;
    CHECK_EQ(arr.sum(), doctest::Approx(5.0));
    sylvanmats::linear::ArrayXd arr2(5);
    arr2+=1.0;
    CHECK_EQ(arr.dot(arr2), doctest::Approx(5.0));
    arr+=arr2;
    CHECK_EQ(arr.sum(), doctest::Approx(10.0));
    CHECK_EQ(arr.norm(), doctest::Approx(4.472135955));
    arr.normalize();
    CHECK_EQ(arr[0ul], doctest::Approx(0.4472135955));
    arr*=1.0/arr[0ul];
    CHECK_EQ(arr.sum(), doctest::Approx(5.0));

    arr2+=1.0;
    sylvanmats::linear::ArrayXd arr3=arr*arr2;
    CHECK_EQ(arr3.sum(), doctest::Approx(10.0));
    arr3/=2.0;
    CHECK_EQ(arr3.sum(), doctest::Approx(5.0));
    CHECK_EQ(arr.normalized().dot(arr3.normalized()), doctest::Approx(1.0));
}

TEST_CASE("test ways to use valarray, ranges & coroutines lazily evaluate matrix operations"){


std::valarray<double> array {7.0,  8.0, 9.0, 10.0, 11.0, 12.0};
    //std::cout<<array[std::slice(0, 3, 1)]<<std::endl;;
    //std::cout<<array[std::slice(3, 3, 1)]<<std::endl;;
//    for (const double i : array) {
//        std::cout << i << ' ';
//    }
//    std::cout << '\n';
    auto r=std::views::counted(std::begin(array), 3);
    //std::cout<<" "<<decltype(r)<<std::endl;
//    for (auto i : r){
//        std::cout << i << ' ';
//    }
//    std::cout << '\n';
}

TEST_CASE("perform 1-d dft"){
    sylvanmats::linear::ArrayXcd arr(5);
    using namespace std::complex_literals;
    arr+=(1.0+0.0i);
    CHECK_EQ(arr.sum().real(), doctest::Approx(5.0));
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::ArrayXcd> dft(sylvanmats::domain::FORWARD);
    sylvanmats::linear::ArrayXcd input(64);
    double min=0.0;
    double max=0.0;
    for(size_t i=0;i<input.size();i++){
        input[i]=std::complex<double>(std::sin(8.0*std::numbers::pi*i/input.size()), 0.0);
            if((i==0) || min>input[i].real())min=input[i].real();
            if((i==0) || max<input[i].real())max=input[i].real();
    }
    CHECK_EQ(input.size(), 64);
    CHECK_EQ(min, doctest::Approx(-1.0));
    CHECK_EQ(max, doctest::Approx(1.0));
    CHECK_EQ(input.sum().real(), doctest::Approx(0.0));
    sylvanmats::linear::ArrayXcd output=dft(input);
    for(size_t i=0;i<output.size();i++){
        output[i]/=output.size();
    }
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::ArrayXcd> idft(sylvanmats::domain::INVERSE);
    sylvanmats::linear::ArrayXcd ioutput=idft(output);
    for(size_t i=0;i<ioutput.size();i++){
    CHECK_EQ(input[i].real(), doctest::Approx(ioutput[i].real()));
    }
}

TEST_CASE("perform 2-d dft"){
    using namespace std::complex_literals;
    
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::ArrayXXcd> dft(sylvanmats::domain::FORWARD);
    sylvanmats::linear::ArrayXXcd input(64, 64);
//    CHECK_EQ(std::extent_v<decltype(input)>, 2);
    double min=0.0;
    double max=0.0;
    for(size_t j=0;j<input.cols();j++){
        for(size_t i=0;i<input.rows();i++){
            input[i, j].real(std::sin(8.0*std::numbers::pi*i/input.rows())*std::cos(8.0*std::numbers::pi*j/input.rows()));
            input[i, j].imag(0.0);
//            if(j==0)std::cout<<" "<<input[i, j]<<std::endl;
            if((j==0 && i==0) || min>input[i, j].real())min=input[i, j].real();
            if((j==0 && i==0) || max<input[i, j].real())max=input[i, j].real();
        }
    }
    CHECK_EQ(input.rows(), 64);
    CHECK_EQ(input.cols(), 64);
    CHECK_EQ(min, doctest::Approx(-1.0));
    CHECK_EQ(max, doctest::Approx(1.0));
    CHECK_EQ(input.sum().real(), doctest::Approx(0.0));
//    CHECK_EQ(input.pow(std::complex<double>(2.0, 0.0)).real(), doctest::Approx(0.0));
    min=0.0;
    max=0.0;
    sylvanmats::linear::ArrayXXcd&& output=dft(input);
    for(size_t j=0;j<output.cols();j++){
        for(size_t i=0;i<output.rows();i++){
            output[i, j]/=output.size();
//            if(j==0)std::cout<<"o "<<output[i, j]<<std::endl;
            if((j==0 && i==0) || min>output[i, j].real())min=output[i, j].real();
            if((j==0 && i==0) || max<output[i, j].real())max=output[i, j].real();
        }
    }
    CHECK_EQ(output.rows(), 64);
    CHECK_EQ(output.cols(), 64);
//    CHECK_EQ(min, doctest::Approx(-1.0));
//    CHECK_EQ(max, doctest::Approx(1.0));
    CHECK_EQ(output.sum().real(), doctest::Approx(0.0));
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::ArrayXXcd> idft(sylvanmats::domain::INVERSE);
    sylvanmats::linear::ArrayXXcd ioutput=idft(output);
    for(size_t j=0;j<ioutput.cols();j++){
        for(size_t i=0;i<ioutput.rows();i++){
//            if(j==0)std::cout<<"io "<<ioutput[i, j]<<std::endl;
        CHECK_EQ(input[i, j].real(), doctest::Approx(ioutput[i, j].real()));
        }
    }
}

TEST_CASE("perform 3-d dft"){
    using namespace std::complex_literals;
    std::cout<<"in perform 3-d dft "<<(getenv("OMP_WAIT_POLICY")!=nullptr)<<std::endl;
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::Tensor<std::complex<double>>> dft(sylvanmats::domain::FORWARD);
    std::cout<<"3-d "<<std::endl;
    size_t slices=12;
    sylvanmats::linear::Tensor<std::complex<double>> input(64, 64, slices);
    double min=0.0;
    double max=0.0;
    CHECK_EQ(input.D, 3);
    CHECK_EQ(input.size(), 64*64*slices);
    std::cout<<"i "<<input.D<<" "<<input.size()<<" "<<input.getDimension(0)<<std::endl;
    for(size_t k=0;k<input.getDimension(2);k++){
    for(size_t j=0;j<input.getDimension(1);j++){
        for(size_t i=0;i<input.getDimension(0);i++){
            input[i, j, k].real(std::sin(8.0*std::numbers::pi*i/input.getDimension(0))*std::cos(8.0*std::numbers::pi*j/input.getDimension(1)));
            input[i, j, k].imag(0.0);
//            if(j==0)std::cout<<" "<<input[i, j]<<std::endl;
//        if(i==j && j==k)std::cout<<i<<" i "<<input[i, j, k]<<std::endl;
            if((k==0 && j==0 && i==0) || min>input[i, j, k].real())min=input[i, j, k].real();
            if((k==0 && j==0 && i==0) || max<input[i, j, k].real())max=input[i, j, k].real();
        }
    }
    }
    CHECK_EQ(input.getDimension(0), 64);
    CHECK_EQ(input.getDimension(1), 64);
    CHECK_EQ(input.getDimension(2), slices);
    CHECK_EQ(min, doctest::Approx(-1.0));
    CHECK_EQ(max, doctest::Approx(1.0));
    CHECK_EQ(input.sum().real(), doctest::Approx(0.0));
    min=0.0;
    max=0.0;
    sylvanmats::linear::Tensor<std::complex<double>>&& output=dft(input);
    for(size_t k=0;k<output.getDimension(2);k++){
    for(size_t j=0;j<output.getDimension(1);j++){
        for(size_t i=0;i<output.getDimension(0);i++){
            output[i, j, k]/=output.size();
//            if(j==0)std::cout<<"o "<<output[i, j, k]<<std::endl;
            if((k==0 && j==0 && i==0) || min>output[i, j, k].real())min=output[i, j, k].real();
            if((k==0 && j==0 && i==0) || max<output[i, j, k].real())max=output[i, j, k].real();
        }
    }
    }
    CHECK_EQ(output.getDimension(0), 64);
    CHECK_EQ(output.getDimension(1), 64);
    CHECK_EQ(output.getDimension(2), slices);
//    CHECK_EQ(min, doctest::Approx(-1.0));
//    CHECK_EQ(max, doctest::Approx(1.0));
    CHECK_EQ(output.sum().real(), doctest::Approx(0.0));
    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::Tensor<std::complex<double>>> idft(sylvanmats::domain::INVERSE);
    sylvanmats::linear::Tensor<std::complex<double>>&& ioutput=idft(output);
    min=0.0;
    max=0.0;
    for(size_t k=0;k<1;k++){
    for(size_t j=0;j<1;j++){
        for(size_t i=0;i<ioutput.getDimension(0);i++){
        CHECK_EQ(input[i, j, k].real(), doctest::Approx(ioutput[i, j, k].real()));
//        if(j==0 && j==k)std::cout<<i<<" io "<<ioutput[i, j, k]<<std::endl;
            if((k==0 && j==0 && i==0) || min>ioutput[i, j, k].real())min=ioutput[i, j, k].real();
            if((k==0 && j==0 && i==0) || max<ioutput[i, j, k].real())max=ioutput[i, j, k].real();
        }
    }
    }
    CHECK_EQ(min, doctest::Approx(-1.0));
    CHECK_EQ(max, doctest::Approx(1.0));
}

}