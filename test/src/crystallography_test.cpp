#include <doctest/doctest.h>

#include <string>
#include <iostream>
#include <utility>
#include <cstring>

#define protected public

#include "mio/mmap.hpp"
#include "density/mtz/Input.h"

TEST_SUITE("lattice"){

TEST_CASE("test mtz input"){
    std::filesystem::path path="../../1q8h_phases.mtz";

    CHECK(std::filesystem::exists(path));
    sylvanmats::density::mtz::Input mtzInput;
    mtzInput(path, [](sylvanmats::density::mtz::mtz_header& ccp4Header){
//        CHECK_EQ(ccp4Header.LABEL[0], "::::EMDATABANK.org::::EMD-8194::::                                              ");
    });
    CHECK_EQ(mtzInput.getHeader().version, "MTZ:V1.1");
    CHECK_EQ(mtzInput.getHeader().title, "Output mtz file from refmac");
    CHECK_EQ(mtzInput.getHeader().number_of_columns, 17);
    CHECK_EQ(mtzInput.getHeader().number_of_reflections_in_file, 3732);
    CHECK_EQ(mtzInput.getHeader().number_of_batches, 0);
    CHECK_EQ(mtzInput.getHeader().number_of_symmetry_operations, 6);
    CHECK_EQ(mtzInput.getHeader().number_of_primitive_operations, 6);
}
}
