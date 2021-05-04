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
#include <ranges>
#include <type_traits>

#include "linear/Array.h"
#include "linear/Vector.h"
#include "linear/Matrix.h"

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
    CHECK_EQ(arr[0], doctest::Approx(0.4472135955));
    arr*=1.0/arr[0];
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
    for (const double i : array) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    auto r=std::views::counted(std::begin(array), 3);
    //std::cout<<" "<<decltype(r)<<std::endl;
    for (auto i : r){
        std::cout << i << ' ';
    }
    std::cout << '\n';
}

}