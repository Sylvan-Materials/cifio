#include <doctest/doctest.h>

#include <string>
#include <iostream>
#include <utility>
#include <cstring>

#define protected public

#include "reading/tcp/TCPReader.h"
#include "mio/mmap.hpp"
#include "density/mtz/Input.h"
#include "density/Space.h"
#include "domain/DiscreteFourierTransform.h"
#include "statistics/Accumulator.h"

TEST_SUITE("crystallography"){

TEST_CASE("test 1q8h mtz input"){
    std::string comp_id="1q8h";
    std::string url = "https://edmaps.rcsb.org/coefficients/"+comp_id+".mtz";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&comp_id](std::istream& is){
        char buf[32768];
        std::string path="./examples/"+comp_id+".mtz";
        std::ofstream ofs(path, std::ios::out | std::ios::trunc | std::ios::binary);
        std::size_t totalCount=0;
        std::streamsize readCount=0;
        do{
            readCount=is.readsome(buf, 32768);
//            std::cout<<"readCount "<<readCount<<std::endl;
            if(readCount>0)ofs.write(buf, readCount);
            totalCount+=readCount;
        }
        while(readCount>0);
        ofs.close();
        CHECK_EQ(totalCount, 258736);
    }));
    std::filesystem::path path="./examples/"+comp_id+".mtz";

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
    CHECK_EQ(mtzInput.getHeader().lattice_type, "P");
    CHECK_EQ(mtzInput.getHeader().space_group_number, 152);
    CHECK_EQ(mtzInput.getHeader().space_group_name, "P 31 2 1");
    CHECK_EQ(mtzInput.getHeader().point_group_name, "PG321");
    CHECK_EQ(mtzInput.getHeader().number_of_datasets, 2);
    CHECK_EQ(mtzInput.getHeader().structure_crystals.size(), 2);
    if(mtzInput.getHeader().structure_crystals.size()>0){
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].columnList.size(), 4);
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].name, "HKL_base");
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].project, "HKL_base");
    }
    if(mtzInput.getHeader().structure_crystals.size()>1){
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].columnList.size(), 13);
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].name, "cryst_1");
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].project, "sf_convert");
    }
    CHECK(mtzInput.getHeader().minimum_resolution == doctest::Approx(0.001301372423768));
    CHECK(mtzInput.getHeader().maximum_resolution == doctest::Approx(0.2497843205928802));
    CHECK(mtzInput.getHeader().cell.a == doctest::Approx(51.491));
    CHECK(mtzInput.getHeader().cell.b == doctest::Approx(51.491));
    CHECK(mtzInput.getHeader().cell.c == doctest::Approx(35.389));
    CHECK(mtzInput.getHeader().cell.α == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.β == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.γ == doctest::Approx(120.0));
    CHECK(mtzInput.getHeader().cell.volume(sylvanmats::density::Tetragonal) == doctest::Approx(93827.7));
    CHECK_EQ(mtzInput.getHeader().column_labels.size(), 17);
    if(mtzInput.getHeader().column_labels.size(), 17){
    CHECK_EQ(mtzInput.getHeader().column_labels[0].label, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].label, "K");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].label, "L");
    CHECK_EQ(mtzInput.getHeader().column_labels[3].label, "FREE");
    CHECK_EQ(mtzInput.getHeader().column_labels[4].label, "FP");
    CHECK_EQ(mtzInput.getHeader().column_labels[0].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[3].type, "I");
    CHECK_EQ(mtzInput.getHeader().column_labels[4].type, "F");
    CHECK(mtzInput.reflections[0].min() == doctest::Approx(1.0));
    CHECK(mtzInput.reflections[0].max() == doctest::Approx(22.0));
    CHECK(mtzInput.reflections[1].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[1].max() == doctest::Approx(12.0));
    CHECK(mtzInput.reflections[2].min() == doctest::Approx(-16.0));
    CHECK(mtzInput.reflections[2].max() == doctest::Approx(16.0));
    CHECK(mtzInput.reflections[3].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[3].max() == doctest::Approx(1.0));
    sylvanmats::density::Space<double> space;
    space.cell=mtzInput.getHeader().cell;
    std::cout<<"G*\n"<<space.reciprocal(sylvanmats::density::Tetragonal)<<std::endl;
    std::cout<<"G\n"<<space.G<<std::endl;
        
    }
//    for(int index=0;index<34;index++){
//        std::cout <<std::setw(4)<<index<<std::setprecision(4)<<std::dec;
//        for(int colIndex=0;colIndex<mtzInput.reflections.size();colIndex++)std::cout<<" "<<std::setw(6)<<mtzInput.reflections[colIndex][index];
//        std::cout<<std::endl;
//    }
    if(mtzInput.reflections.size()>7){
        sylvanmats::statistics::min<double> min;
        sylvanmats::statistics::max<double> max;
        sylvanmats::statistics::mean<double> mean;
        sylvanmats::statistics::median<double> median;
        sylvanmats::statistics::Accumulator<double, double, sylvanmats::statistics::min<double>, sylvanmats::statistics::max<double>, sylvanmats::statistics::mean<double>, sylvanmats::statistics::median<double>> acc(5, min, max, mean, median);
        for(size_t i=0;i<mtzInput.reflections[0].size();i++){
            acc(mtzInput.reflections[6][i]);
        }
        CHECK(min() == doctest::Approx(0.195903));
        CHECK(max() == doctest::Approx(1509.03));
        CHECK(mean() == doctest::Approx(73.1157));
        auto&& [unique, median1, median2]=median();
        CHECK(!unique);
        CHECK(median1 == doctest::Approx(38.2334));
        if(unique)CHECK(median2 == doctest::Approx(73.1157));
    }
}

TEST_CASE("test 1lri mtz input"){
    std::string comp_id="1lri";
    std::string url = "https://edmaps.rcsb.org/coefficients/"+comp_id+".mtz";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&comp_id](std::istream& is){
        char buf[32768];
        std::string path="./examples/"+comp_id+".mtz";
        std::ofstream ofs(path, std::ios::out | std::ios::trunc | std::ios::binary);
        std::size_t totalCount=0;
        std::streamsize readCount=0;
        do{
            readCount=is.readsome(buf, 32768);
//            std::cout<<"readCount "<<readCount<<std::endl;
            if(readCount>0)ofs.write(buf, readCount);
            totalCount+=readCount;
        }
        while(readCount>0);
        ofs.close();
        CHECK_EQ(totalCount, 1190496);
    }));
    std::filesystem::path path="./examples/"+comp_id+".mtz";

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
    CHECK_EQ(mtzInput.getHeader().lattice_type, "C");
    CHECK_EQ(mtzInput.getHeader().space_group_number, 20);
    CHECK_EQ(mtzInput.getHeader().space_group_name, "C 2 2 21");
    CHECK_EQ(mtzInput.getHeader().point_group_name, "PG222");
    CHECK_EQ(mtzInput.getHeader().number_of_datasets, 2);
    CHECK_EQ(mtzInput.getHeader().structure_crystals.size(), 2);
    if(mtzInput.getHeader().structure_crystals.size()>0){
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].columnList.size(), 4);
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].name, "HKL_base");
        CHECK_EQ(mtzInput.getHeader().structure_crystals[0].project, "HKL_base");
    }
    if(mtzInput.getHeader().structure_crystals.size()>1){
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].columnList.size(), 13);
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].name, "cryst_1");
        CHECK_EQ(mtzInput.getHeader().structure_crystals[1].project, "sf_convert");
    }
    CHECK(mtzInput.getHeader().minimum_resolution == doctest::Approx(0.00375273));
    CHECK(mtzInput.getHeader().maximum_resolution == doctest::Approx(0.47572));
    CHECK(mtzInput.getHeader().cell.a == doctest::Approx(30.964));
    CHECK(mtzInput.getHeader().cell.b == doctest::Approx(94.8));
    CHECK(mtzInput.getHeader().cell.c == doctest::Approx(65.296));
    CHECK(mtzInput.getHeader().cell.α == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.β == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.γ == doctest::Approx(90.0));
    CHECK(mtzInput.getHeader().cell.volume(sylvanmats::density::Orthorhombic) == doctest::Approx(191669));
    CHECK_EQ(mtzInput.getHeader().column_labels.size(), 17);
    CHECK_EQ(mtzInput.getHeader().symmetry_operations.size(), 8);
    if(mtzInput.getHeader().column_labels.size(), 17){
    CHECK_EQ(mtzInput.getHeader().column_labels[0].label, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].label, "K");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].label, "L");
    CHECK_EQ(mtzInput.getHeader().column_labels[3].label, "FREE");
    CHECK_EQ(mtzInput.getHeader().column_labels[4].label, "FP");
    CHECK_EQ(mtzInput.getHeader().column_labels[0].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[1].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[2].type, "H");
    CHECK_EQ(mtzInput.getHeader().column_labels[3].type, "I");
    CHECK_EQ(mtzInput.getHeader().column_labels[4].type, "F");
//    for(int i=5;i<mtzInput.getHeader().column_labels.size();i++)std::cout<<"L"<<i<<" "<<mtzInput.getHeader().column_labels[i].label<<std::endl;
//    for(int i=5;i<mtzInput.getHeader().column_labels.size();i++)std::cout<<"I"<<i<<" "<<mtzInput.getHeader().column_labels[i].type<<std::endl;
    CHECK(mtzInput.reflections[0].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[0].max() == doctest::Approx(21.0));
    CHECK(mtzInput.reflections[1].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[1].max() == doctest::Approx(65.0));
    CHECK(mtzInput.reflections[2].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[2].max() == doctest::Approx(45.0));
    CHECK(mtzInput.reflections[3].min() == doctest::Approx(0.0));
    CHECK(mtzInput.reflections[3].max() == doctest::Approx(1.0));
    sylvanmats::density::Space<double> space;
    space.cell=mtzInput.getHeader().cell;
    std::cout<<"GStar\n"<<space.reciprocal(sylvanmats::density::Orthorhombic)<<std::endl;
    std::cout<<"G\n"<<space.G<<std::endl;
    sylvanmats::linear::Tensor<std::complex<double>>&& F=mtzInput.getSF(6, 7);
    CHECK_EQ(F.getDimension(0), 22);
    CHECK_EQ(F.getDimension(1), 66);
    CHECK_EQ(F.getDimension(2), 46);
//    sylvanmats::domain::DiscreteFourierTransform<sylvanmats::linear::Tensor<std::complex<double>>> idft(sylvanmats::domain::INVERSE);
//    sylvanmats::linear::Tensor<std::complex<double>>&& P=idft(F);
//    CHECK_EQ(P.getDimension(0), 22);
//    CHECK_EQ(P.getDimension(1), 66);
//    CHECK_EQ(P.getDimension(2), 46);
    
    }
//    for(int index=0;index<34;index++){
//        std::cout <<std::setw(4)<<index<<std::setprecision(4)<<std::dec;
//        for(int colIndex=0;colIndex<mtzInput.reflections.size();colIndex++)std::cout<<" "<<std::setw(6)<<mtzInput.reflections[colIndex][index];
//        std::cout<<std::endl;
//    }
}

TEST_CASE("test calcite reciprocal"){
    sylvanmats::density::Space<double> space;
    space.cell.a=4.990;
    space.cell.b=4.990;
    space.cell.c=17.061;
    space.cell.α=90.0;
    space.cell.β=90.0;
    space.cell.γ=120.0;
    std::cout<<"calcite GStar\n"<<space.reciprocal(sylvanmats::density::Hexagonal)<<std::endl;
    std::cout<<"G\n"<<space.G<<std::endl;
//    CHECK(space.cStar == doctest::Approx(0.058613));
    CHECK(space.GStar[0, 0] == doctest::Approx(0.053546));
    CHECK(space.GStar[1, 1] == doctest::Approx(0.053546));
    CHECK(space.GStar[2, 2] == doctest::Approx(0.0034354));
    CHECK(space.GStar[0, 1] == doctest::Approx(0.026773));
    CHECK(space.GStar[0, 2] == doctest::Approx(0.0));
    CHECK(space.GStar[1, 2] == doctest::Approx(0.0));
    
}

}
