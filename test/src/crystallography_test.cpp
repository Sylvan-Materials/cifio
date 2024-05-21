#include <doctest/doctest.h>

#include <string>
#include <iostream>
#include <utility>
#include <cstring>

#define protected public

#include "mio/mmap.hpp"
#include "density/mtz/Input.h"

TEST_SUITE("crystallography"){

TEST_CASE("test 1q8h mtz input"){
    std::filesystem::path path="../../1q8h_phases.mtz";

    CHECK(std::filesystem::exists(path));
    sylvanmats::density::mtz::Input mtzInput;
    mtzInput(path, [](sylvanmats::density::mtz::mtz_header& ccp4Header){
//        CHECK_EQ(ccp4Header.LABEL[0], "::::EMDATABANK.org::::EMD-8194::::                                              ");
    });
    CHECK_EQ(mtzInput.getHeader().version, "MTZ:V1.1");
    CHECK_EQ(mtzInput.getHeader().title, "Output mtz file from refmac");
    CHECK_EQ(mtzInput.getHeader().number_of_columns, 17);
    CHECK_EQ(mtzInput.reflections.size(), 17);
    CHECK_EQ(mtzInput.getHeader().number_of_reflections_in_file, 3732);
    CHECK_EQ(mtzInput.getHeader().number_of_batches, 0);
    CHECK_EQ(mtzInput.getHeader().number_of_symmetry_operations, 6);
    CHECK_EQ(mtzInput.getHeader().number_of_primitive_operations, 6);
    CHECK_EQ(mtzInput.getHeader().number_of_datasets, 2);
    CHECK(mtzInput.getHeader().minimum_resolution == doctest::Approx(0.001301372423768));
    CHECK(mtzInput.getHeader().maximum_resolution == doctest::Approx(0.2497843205928802));
    CHECK(mtzInput.getHeader().cell.a == doctest::Approx(51.491));
    CHECK(mtzInput.getHeader().cell.b == doctest::Approx(51.491));
    CHECK(mtzInput.getHeader().cell.c == doctest::Approx(35.389));
    CHECK(mtzInput.getHeader().cell.α == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.β == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.γ == doctest::Approx(120.0));
    CHECK_EQ(mtzInput.getHeader().column_labels.size(), 17);
    if(mtzInput.getHeader().column_labels.size(), 17){
    CHECK_EQ(mtzInput.getHeader().column_labels[0].label, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].label, "K");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].label, "L");
    CHECK_EQ(mtzInput.getHeader().column_labels[0].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].type, "H");
        
    }
    for(int index=0;index<34;index++){
        std::cout <<std::setw(4)<<index<<std::setprecision(4)<<std::dec;
        for(int colIndex=0;colIndex<mtzInput.reflections.size();colIndex++)std::cout<<" "<<std::setw(6)<<mtzInput.reflections[colIndex][index];
        std::cout<<std::endl;
    }
}

TEST_CASE("test 1lri mtz input"){
    std::filesystem::path path="../../1lri_phases.mtz";

    CHECK(std::filesystem::exists(path));
    sylvanmats::density::mtz::Input mtzInput;
    mtzInput(path, [](sylvanmats::density::mtz::mtz_header& ccp4Header){
//        CHECK_EQ(ccp4Header.LABEL[0], "::::EMDATABANK.org::::EMD-8194::::                                              ");
    });
    CHECK_EQ(mtzInput.getHeader().version, "MTZ:V1.1");
    CHECK_EQ(mtzInput.getHeader().title, "Output mtz file from refmac");
    CHECK_EQ(mtzInput.getHeader().number_of_columns, 17);
    CHECK_EQ(mtzInput.reflections.size(), 17);
    CHECK_EQ(mtzInput.getHeader().number_of_reflections_in_file, 17432);
    CHECK_EQ(mtzInput.getHeader().number_of_batches, 0);
    CHECK_EQ(mtzInput.getHeader().number_of_symmetry_operations, 8);
    CHECK_EQ(mtzInput.getHeader().number_of_primitive_operations, 4);
    CHECK_EQ(mtzInput.getHeader().number_of_datasets, 2);
    CHECK(mtzInput.getHeader().minimum_resolution == doctest::Approx(0.00375273));
    CHECK(mtzInput.getHeader().maximum_resolution == doctest::Approx(0.47572));
    CHECK(mtzInput.getHeader().cell.a == doctest::Approx(30.964));
    CHECK(mtzInput.getHeader().cell.b == doctest::Approx(94.8));
    CHECK(mtzInput.getHeader().cell.c == doctest::Approx(65.296));
    CHECK(mtzInput.getHeader().cell.α == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.β == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.γ == doctest::Approx(90.0));
    CHECK_EQ(mtzInput.getHeader().column_labels.size(), 17);
    CHECK_EQ(mtzInput.getHeader().symmetry_operations.size(), 8);
    if(mtzInput.getHeader().column_labels.size(), 17){
    CHECK_EQ(mtzInput.getHeader().column_labels[0].label, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].label, "K");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].label, "L");
    CHECK_EQ(mtzInput.getHeader().column_labels[0].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].type, "H");
        
    }
    for(int index=0;index<34;index++){
        std::cout <<std::setw(4)<<index<<std::setprecision(4)<<std::dec;
        for(int colIndex=0;colIndex<mtzInput.reflections.size();colIndex++)std::cout<<" "<<std::setw(6)<<mtzInput.reflections[colIndex][index];
        std::cout<<std::endl;
    }
}

}
