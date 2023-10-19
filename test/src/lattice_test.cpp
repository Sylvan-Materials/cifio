#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>

#include "algebra/geometric/Bivector.h"
#include "algebra/geometric/Rotor.h"
#include "algebra/geometric/Motor.h"

#include "antlr4-runtime.h"
#include "parsing/CIFLexer.h"
#include "parsing/CIFParser.h"
#include "reading/gz/GZReader.h"
#include "reading/tcp/TCPReader.h"
#include "constitution/Populator.h"
#include "density/Populator.h"
#include "lattice/Populator.h"
#include "publishing/st/OBJPublisher.h"
#include "symmetry/LatticeSites.h"
#include "LuaException.h"

#include "density/ccp4/MapInput.h"
#include "png.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

TEST_SUITE("lattice"){

TEST_CASE("test bivector"){
    sylvanmats::linear::Vector3d v1(2.0, 3.0, 4.0);
    sylvanmats::linear::Vector3d v2(7.0, 1.0, 5.0);
    sylvanmats::algebra::geometric::Bivector<double> bivector(v1, v2);
//    std::cout<<bivector<<std::endl;
    CHECK_EQ(bivector.B01(), -19.0);
    CHECK_EQ(bivector.B12(), 11.0);
    CHECK_EQ(bivector.B20(), 18.0);
}

TEST_CASE("test rotor"){
    sylvanmats::linear::Vector3d v1(1.0, 0.0, 0.0);
    sylvanmats::linear::Vector3d v2(0.0, 1.0, 0.0);
    sylvanmats::linear::Vector3d v3(0.0, 0.0, 1.0);
    sylvanmats::algebra::geometric::Rotor<double> rotor12(v1, v2);
    sylvanmats::algebra::geometric::Rotor<double> rotor23(v2, v3);
    sylvanmats::algebra::geometric::Rotor<double> rotor31(v3, v1);
    sylvanmats::algebra::geometric::Rotor<double> identityRotor12;
    sylvanmats::algebra::geometric::Rotor<double> identityRotor23;
    sylvanmats::algebra::geometric::Rotor<double> identityRotor31;
    sylvanmats::algebra::geometric::Rotor<double> transformedRotor12 = identityRotor12*rotor12;
    sylvanmats::algebra::geometric::Rotor<double> transformedRotor23 = identityRotor23*rotor23;
    sylvanmats::algebra::geometric::Rotor<double> transformedRotor31 = identityRotor31*rotor31;
//    std::cout<<transformedRotor12<<std::endl;
    sylvanmats::linear::Vector3d x(0.0, 0.0, 1.0);
    sylvanmats::linear::Vector3d y(0.0, 1.0, 0.0);
    sylvanmats::linear::Vector3d z(1.0, 0.0, 0.0);
    x = rotor12.rotate(x);
    y = rotor23.rotate(y);
    z = rotor31.rotate(z);
    REQUIRE(x[0] == doctest::Approx(0.0));
    REQUIRE(x[1] == doctest::Approx(0.0));
    REQUIRE(x[2] == doctest::Approx(1.0));
    REQUIRE(y[0] == doctest::Approx(0.0));
    REQUIRE(y[1] == doctest::Approx(1.0));
    REQUIRE(y[2] == doctest::Approx(0.0));
    REQUIRE(z[0] == doctest::Approx(1.0));
    REQUIRE(z[1] == doctest::Approx(0.0));
    REQUIRE(z[2] == doctest::Approx(0.0));
    CHECK_EQ(transformedRotor12.a(), rotor12.a());
    CHECK_EQ(transformedRotor12.B01(), rotor12.B01());
    CHECK_EQ(transformedRotor12.B12(), rotor12.B12());
    CHECK_EQ(transformedRotor12.B20(), rotor12.B20());
    sylvanmats::algebra::geometric::Rotor<double> reverseTransformedRotor12 = transformedRotor12.reverse();
    sylvanmats::algebra::geometric::Rotor<double> reverseTransformedRotor23 = transformedRotor23.reverse();
    sylvanmats::algebra::geometric::Rotor<double> reverseTransformedRotor31 = transformedRotor31.reverse();
//    std::cout<<"transformedRotor: "<<transformedRotor23<<" reverseTransformedRotor23 "<<reverseTransformedRotor23<<std::endl;
    x = reverseTransformedRotor12.rotate(x);
    y = reverseTransformedRotor23.rotate(y);
    z = reverseTransformedRotor31.rotate(z);
    REQUIRE(x[0] == doctest::Approx(0.0));
    REQUIRE(x[1] == doctest::Approx(0.0));
    REQUIRE(x[2] == doctest::Approx(1.0));
    REQUIRE(y[0] == doctest::Approx(0.0));
    REQUIRE(y[1] == doctest::Approx(1.0));
    REQUIRE(y[2] == doctest::Approx(0.0));
    REQUIRE(z[0] == doctest::Approx(1.0));
    REQUIRE(z[1] == doctest::Approx(0.0));
    REQUIRE(z[2] == doctest::Approx(0.0));
    sylvanmats::linear::Vector3d vXAxis=sylvanmats::linear::Vector3d::UnitX();
    sylvanmats::linear::Vector3d vYAxis=sylvanmats::linear::Vector3d::UnitY();
    sylvanmats::linear::Vector3d vZAxis=sylvanmats::linear::Vector3d::UnitZ();
    sylvanmats::algebra::geometric::Rotor<double> e1e2Rotor(std::numbers::pi*(90.0)/180.0, vXAxis, vYAxis);
    sylvanmats::algebra::geometric::Rotor<double> e2e3Rotor(std::numbers::pi*(90.0)/180.0, vYAxis, vZAxis);
    sylvanmats::algebra::geometric::Rotor<double> e3e1Rotor(std::numbers::pi*(90.0)/180.0, vZAxis, vXAxis);
    sylvanmats::algebra::geometric::Rotor<double> transformedRotor = e1e2Rotor*e2e3Rotor*e3e1Rotor;
//    std::cout<<"transformedRotor "<<transformedRotor<<std::endl;
//    std::cout<<"transformedRotor length "<<transformedRotor.length()<<std::endl;
    sylvanmats::linear::Vector3d vXAxisT=transformedRotor.rotate(vXAxis);
    //std::cout<<"vXAxisT "<<vXAxisT.transpose()<<std::endl;
}

TEST_CASE("test copper unit cell"){
    std::string url = "https://www.crystallography.net/cod/result*?text=copper&format=urls&el1=Cu&formula=Cu";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        std::vector<std::string> urls;
        std::string delim="\n";
        char *token = strtok(const_cast<char*>(content.c_str()), delim.c_str());
        while (token != nullptr)
        {
                urls.push_back(std::string(token));
                token = strtok(nullptr, delim.c_str());
        }
        CHECK_EQ(urls.size(), 18);
        url=urls[0];
        CHECK(tcpReader(url, [&](std::istream& isr){
            sylvanmats::lattice::Graph graph;

            sylvanmats::lattice::Populator populator;
            populator(isr, graph, [](sylvanmats::lattice::Graph& graph){
                CHECK_EQ(lemon::countNodes(graph), 1);

            });
            CHECK_EQ(lemon::countNodes(graph), 1);
            CHECK_EQ(lemon::countEdges(graph), 0);
//std::cout<<"cell "<<graph.cell.length_a<<" "<<graph.cell.length_b<<" "<<graph.cell.length_c<<std::endl;
             sylvanmats::linear::Vector3d vXAxis(graph.cell.length_a, 0.0, 0.0);
             sylvanmats::linear::Vector3d vYAxis(0.0, graph.cell.length_b, 0.0);
             sylvanmats::linear::Vector3d vZAxis(0.0, 0.0, graph.cell.length_c);
             sylvanmats::algebra::geometric::Rotor<double> e1e2Rotor(std::numbers::pi*(graph.cell.angle_alpha-90.0)/180.0, vXAxis, vYAxis);
             sylvanmats::algebra::geometric::Rotor<double> e2e3Rotor(std::numbers::pi*(graph.cell.angle_beta-90.0)/180.0, vYAxis, vZAxis);
             sylvanmats::algebra::geometric::Rotor<double> e3e1Rotor(-std::numbers::pi*(graph.cell.angle_gamma-90.0)/180.0, vZAxis, vXAxis);
             sylvanmats::algebra::geometric::Rotor<double> transformedRotor = e1e2Rotor*e2e3Rotor*e3e1Rotor;
//             std::cout<<transformedRotor<<std::endl;
            sylvanmats::linear::Vector3d x(graph.cell.length_a/2.0, graph.cell.length_b/2.0, graph.cell.length_c/2.0);
            x = transformedRotor.rotate(x);
            //std::cout<<x.transpose()<<std::endl;
            auto [functionNames, ssLua] = graph.equivalentPositionsAsLua();
            sylvanmats::linear::Matrix3d opMatrix=sylvanmats::linear::Matrix3d::Identity();//((double*)graph.getFractionalAtomSites().fract_transf_matrix, 3, 3);
            sylvanmats::linear::Vector3d opVector=sylvanmats::linear::Vector3d::Zero();//(graph.getFractionalAtomSites().fract_transf_vector);
            sylvanmats::linear::Vector3d p1(1.0, -1.0/std::sqrt(3.0), -1.0/std::sqrt(6.0));
            sylvanmats::linear::Vector3d p2(-1.0, -1.0/std::sqrt(3.0), -1.0/std::sqrt(6.0));
            sylvanmats::linear::Vector3d p3(0.0, 2.0/std::sqrt(3.0), -1.0/std::sqrt(6.0));
            sylvanmats::linear::Vector3d p4(0.0, 0.0, 3.0/std::sqrt(6.0));
            try{
                //ssLua+="function f-2(x,y,z) return x,z+1/2,y end\n";
                //std::cout<<" "<<ssLua<<std::endl;
                std::filesystem::path path="../../cifio/templates/obj";
                sylvanmats::publishing::st::OBJPublisher objPublisher(path);
                objPublisher.setVertexCount(4*functionNames.size());
                objPublisher.setFaceCount(6*functionNames.size());
                objPublisher.setMaterialCount(1);
                std::vector<std::tuple<double, double, double>> normalLoop;
                std::vector<std::tuple<unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long>> indexLoop;
                sylvanmats::linear::Vector3d c(0.722, 0.451, 0.2);
                unsigned int faceIndex=1;
                sylvanmats::symmetry::LatticeSites latticeSites(functionNames, ssLua, opMatrix, opVector);
                latticeSites(x, [&faceIndex, &p1, &p2, &p3, &p4, &c, &objPublisher, &normalLoop, &indexLoop](sylvanmats::linear::Vector3d& xPrime){
                    //std::cout<<"xPrime "<<xPrime<<std::endl;
                    sylvanmats::linear::Vector3d v=p1/3.0+xPrime;
                    objPublisher.append(v);
                    v=p2/3.0+xPrime;
                    objPublisher.append(v);
                    v=p3/3.0+xPrime;
                    objPublisher.append(v);
                    v=p4/3.0+xPrime;
                    objPublisher.append(v);
                    sylvanmats::linear::Vector3d vn=(-(p2+p3+p4)/3.0).normalized();
                    normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
                    vn=(-(p1+p3+p4)/3.0).normalized();
                    normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
                    vn=(-(p1+p2+p4)/3.0).normalized();
                    normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
                    vn=(-(p1+p2+p3)/3.0).normalized();
                    normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
                    indexLoop.insert(indexLoop.begin(), std::make_tuple(faceIndex, faceIndex+1, faceIndex+2, faceIndex, faceIndex+2, faceIndex+3, faceIndex+1, faceIndex));
                    indexLoop.insert(indexLoop.begin(), std::make_tuple(faceIndex+2, faceIndex+3, faceIndex+1, faceIndex+2, faceIndex+3, faceIndex+1, faceIndex+3, faceIndex+2));
                    faceIndex+=4;
                });

//                objPublisher.add("texture_vertices", vertexLoop);
//                objPublisher.add("normals", normalLoop);
//                objPublisher.add("indices", indexLoop);
                std::string&& content2 = objPublisher.render();
                std::ofstream ofs2("../../server/public/molecule.obj");
                ofs2<<content2<<std::endl;
            }
            catch(sylvanmats::LuaException& ex){
                std::cout<<" "<<ex.what()<<std::endl;
            }
            catch(std::exception& ex){
                std::cout<<" "<<ex.what()<<std::endl;
            }
        }));
        std::string hklUrl=url.substr(0, url.size()-4)+".hkl";
//std::cout<<"hklUrl "<<hklUrl<<std::endl;
        CHECK(tcpReader(hklUrl, [&](std::istream& is){
            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            //std::cout<<"cifContent: "<<cifContent<<std::endl;
            CHECK_EQ(cifContent.size(), 203938);

        }));
        
    }));

}

TEST_CASE("test graphite unit cell"){
    std::string url = "https://www.crystallography.net/cod/result*?text=graphite&format=urls&el1=C";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        std::vector<std::string> urls;
        std::string delim="\n";
        char *token = strtok(const_cast<char*>(content.c_str()), delim.c_str());
        while (token != nullptr)
        {
                urls.push_back(std::string(token));
                token = strtok(nullptr, delim.c_str());
        }
        CHECK_EQ(urls.size(), 42);
        url=urls[0];
//        std::cout<<"url "<<url<<std::endl;
        CHECK(tcpReader(url, [&](std::istream& isr){
            sylvanmats::lattice::Graph graph;

            sylvanmats::lattice::Populator populator;
            populator(isr, graph, [](sylvanmats::lattice::Graph& graph){
                CHECK_EQ(lemon::countNodes(graph), 2);

            });
            CHECK_EQ(lemon::countNodes(graph), 2);
            CHECK_EQ(lemon::countEdges(graph), 0);
//std::cout<<"cell "<<graph.cell.length_a<<" "<<graph.cell.length_b<<" "<<graph.cell.length_c<<" "<<graph.cell.angle_gamma<<std::endl;
            sylvanmats::linear::Vector3d vXAxis=sylvanmats::linear::Vector3d::UnitX();
            sylvanmats::linear::Vector3d vYAxis=sylvanmats::linear::Vector3d::UnitY();
            sylvanmats::linear::Vector3d vZAxis=sylvanmats::linear::Vector3d::UnitZ();
            sylvanmats::algebra::geometric::Rotor<double> e1e2Rotor(std::numbers::pi*(graph.cell.angle_alpha-90.0)/180.0, vXAxis, vYAxis);
            sylvanmats::algebra::geometric::Rotor<double> e2e3Rotor(std::numbers::pi*(graph.cell.angle_beta-90.0)/180.0, vYAxis, vZAxis);
            sylvanmats::algebra::geometric::Rotor<double> e3e1Rotor(-std::numbers::pi*(graph.cell.angle_gamma-90.0)/180.0, vZAxis, vXAxis);
            sylvanmats::algebra::geometric::Rotor<double> transformedRotor = e1e2Rotor*e2e3Rotor*e3e1Rotor;
//            std::cout<<"transformedRotor "<<transformedRotor<<std::endl;
//            std::cout<<"transformedRotor length "<<transformedRotor.length()<<std::endl;
            sylvanmats::linear::Vector3d x(graph.cell.length_a/2.0, graph.cell.length_b/2.0, graph.cell.length_c/2.0);
            x = transformedRotor.rotate(x);
            //std::cout<<x.transpose()<<std::endl;
            sylvanmats::algebra::geometric::Rotor<double> reverseTransformedRotor = transformedRotor.reverse();
            x = reverseTransformedRotor.rotate(x);
            //std::cout<<x.transpose()<<std::endl;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                sylvanmats::linear::Vector3d xfrac(graph.atomSites[nSiteA].fract_x, graph.atomSites[nSiteA].fract_y, graph.atomSites[nSiteA].fract_z);
//                std::cout<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[nSiteA].label<<" "<<graph.atomSites[nSiteA].fract_x<<" "<<graph.atomSites[nSiteA].fract_y<<" "<<graph.atomSites[nSiteA].fract_z<<std::endl;
                xfrac = reverseTransformedRotor.rotate(xfrac);
                xfrac[0]*=graph.cell.length_a;
                xfrac[1]*=graph.cell.length_b;
                xfrac[2]*=graph.cell.length_c;
                //std::cout<<"\t"<<xfrac.transpose()<<std::endl;
            };
        }));
        /*std::string hklUrl=url.substr(0, url.size()-4)+".hkl";
//std::cout<<"hklUrl "<<hklUrl<<std::endl;
        CHECK(tcpReader(hklUrl, [&](std::istream& is){
            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            //std::cout<<"cifContent: "<<cifContent<<std::endl;
            CHECK_EQ(cifContent.size(), 203938);

        }));*/
        
    }));

}

TEST_CASE("test gypsum unit cell"){
    std::string url = "https://www.crystallography.net/cod/result*?text=gypsum&format=urls";
    sylvanmats::reading::TCPReader tcpReader;
    CHECK(tcpReader(url, [&](std::istream& is){
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        std::vector<std::string> urls;
        std::string delim="\n";
        char *token = strtok(const_cast<char*>(content.c_str()), delim.c_str());
        while (token != nullptr)
        {
                urls.push_back(std::string(token));
                token = strtok(nullptr, delim.c_str());
        }
        CHECK_EQ(urls.size(), 28);
        url=urls[0];
//        std::cout<<"url "<<url<<std::endl;
        CHECK(tcpReader(url, [&](std::istream& isr){
            sylvanmats::lattice::Graph graph;

            sylvanmats::lattice::Populator populator;
            populator(isr, graph, [](sylvanmats::lattice::Graph& graph){
                CHECK_EQ(lemon::countNodes(graph), 5);

            });
            CHECK_EQ(lemon::countNodes(graph), 5);
            CHECK_EQ(lemon::countEdges(graph), 0);
//std::cout<<"cell "<<graph.cell.length_a<<" "<<graph.cell.length_b<<" "<<graph.cell.length_c<<" "<<graph.cell.angle_gamma<<std::endl;
            sylvanmats::linear::Vector3d vXAxis=sylvanmats::linear::Vector3d::UnitX();
            sylvanmats::linear::Vector3d vYAxis=sylvanmats::linear::Vector3d::UnitY();
            sylvanmats::linear::Vector3d vZAxis=sylvanmats::linear::Vector3d::UnitZ();
            sylvanmats::algebra::geometric::Rotor<double> e1e2Rotor(std::numbers::pi*(graph.cell.angle_alpha-90.0)/180.0, vXAxis, vYAxis);
            sylvanmats::algebra::geometric::Rotor<double> e2e3Rotor(std::numbers::pi*(graph.cell.angle_beta-90.0)/180.0, vYAxis, vZAxis);
            sylvanmats::algebra::geometric::Rotor<double> e3e1Rotor(-std::numbers::pi*(graph.cell.angle_gamma-90.0)/180.0, vZAxis, vXAxis);
            sylvanmats::algebra::geometric::Rotor<double> transformedRotor = e1e2Rotor*e2e3Rotor*e3e1Rotor;
//            std::cout<<"transformedRotor "<<transformedRotor<<std::endl;
//            std::cout<<"transformedRotor length "<<transformedRotor.length()<<std::endl;
            sylvanmats::linear::Vector3d x(graph.cell.length_a/2.0, graph.cell.length_b/2.0, graph.cell.length_c/2.0);
            x = transformedRotor.rotate(x);
            //std::cout<<x.transpose()<<std::endl;
            sylvanmats::algebra::geometric::Rotor<double> reverseTransformedRotor = transformedRotor.reverse();
            x = reverseTransformedRotor.rotate(x);
            //std::cout<<x.transpose()<<std::endl;
            for(lemon::ListGraph::NodeIt nSiteA(graph); nSiteA!=lemon::INVALID; ++nSiteA){
                sylvanmats::linear::Vector3d xfrac(graph.atomSites[nSiteA].fract_x, graph.atomSites[nSiteA].fract_y, graph.atomSites[nSiteA].fract_z);
                std::cout<<" "<<graph.atomSites[nSiteA].type_symbol<<" "<<graph.atomSites[nSiteA].label<<" "<<graph.atomSites[nSiteA].fract_x<<" "<<graph.atomSites[nSiteA].fract_y<<" "<<graph.atomSites[nSiteA].fract_z<<std::endl;
                xfrac = reverseTransformedRotor.rotate(xfrac);
                xfrac[0]*=graph.cell.length_a;
                xfrac[1]*=graph.cell.length_b;
                xfrac[2]*=graph.cell.length_c;
                //std::cout<<"\t"<<xfrac.transpose()<<std::endl;
            };
        }));
        /*std::string hklUrl=url.substr(0, url.size()-4)+".hkl";
//std::cout<<"hklUrl "<<hklUrl<<std::endl;
        CHECK(tcpReader(hklUrl, [&](std::istream& is){
            std::string cifContent((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            //std::cout<<"cifContent: "<<cifContent<<std::endl;
            CHECK_EQ(cifContent.size(), 203938);

        }));*/
        
    }));

}

TEST_CASE("test EM map to png slices"){
    std::filesystem::path path="./examples/emd_8194.map";

    CHECK(std::filesystem::exists(path));
    if(std::filesystem::exists(path)){
        float min=0.0f;
        float max=0.0f;
        sylvanmats::density::ccp4::MapInput mapInput;
        mapInput(path, [&min, &max](sylvanmats::density::ccp4::ccp4_header& ccp4Header, unsigned int sectionIndex, float* slice){
            std::string pngPath="section"+std::to_string(sectionIndex)+".png";
            FILE *fp = std::fopen(pngPath.c_str(), "wb");
            if (!fp)
            {
                return;// (ERROR);
            }
            png_structp png_ptr = png_create_write_struct
            (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (!png_ptr)
                return;// (ERROR);
            png_infop info_ptr = png_create_info_struct(png_ptr);
            if (!info_ptr)
            {
//                std::cout<<"setjmp 2 "<<std::endl;
                png_destroy_write_struct(&png_ptr,
                (png_infopp)NULL);
                return;// (ERROR);
            }
            if (setjmp(png_jmpbuf(png_ptr)))
            {
//                std::cout<<"setjmp 3 "<<std::endl;
                png_destroy_write_struct(&png_ptr, &info_ptr);
                fclose(fp);
                return;// (ERROR);
            }
            png_init_io(png_ptr, fp);
            /*png_set_filter(png_ptr, 0,
            PNG_FILTER_NONE | PNG_FILTER_VALUE_NONE |
            PNG_FILTER_SUB | PNG_FILTER_VALUE_SUB |
            PNG_FILTER_UP | PNG_FILTER_VALUE_UP |
            PNG_FILTER_AVG | PNG_FILTER_VALUE_AVG |
            PNG_FILTER_PAETH | PNG_FILTER_VALUE_PAETH|
            PNG_ALL_FILTERS);*/
            /* set the zlib compression level */
            png_set_compression_level(png_ptr,
            Z_BEST_COMPRESSION);
            if (setjmp (png_jmpbuf (png_ptr))) {
//                std::cout<<"setjmp 4 "<<std::endl;
                return;
            }
            png_set_IHDR(png_ptr, info_ptr, ccp4Header.NR, ccp4Header.NC,
                16, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            if (setjmp (png_jmpbuf (png_ptr))) {
//                std::cout<<"setjmp 5 "<<std::endl;
                return;
            }
            //png_set_gray_to_rgb(png_ptr);
            png_write_info(png_ptr, info_ptr);
            if (setjmp (png_jmpbuf (png_ptr))) {
//                std::cout<<"setjmp 6 "<<std::endl;
                return;
            }
            png_set_swap(png_ptr);
//void write_row_callback(png_ptr, png_uint_32 row, int pass){std::cout<<"row "<<row<<" "<<pass<<std::endl;};
            //png_set_write_status_fn(png_ptr, [](png_structp png_ptr, png_uint_32 row, int pass){std::cout<<"row "<<row<<" "<<pass<<std::endl;});
            //png_set_write_status_fn(png_ptr, write_row_callback);
            std::unique_ptr<unsigned short[]> buffer(new unsigned short[ccp4Header.NR*ccp4Header.NC]);
            std::unique_ptr<png_bytep[]> row_pointers(new png_bytep[ccp4Header.NC]);
            unsigned int rowIndex=0;
            for(unsigned int index=0;index<ccp4Header.NR*ccp4Header.NC;index++){
                min=std::min(min, slice[index]);
                max=std::max(max, slice[index]);
                buffer[index]=static_cast<unsigned short>(65535*(slice[index]-ccp4Header.AMIN)/(ccp4Header.AMAX-ccp4Header.AMIN));
                //buffer[index]=static_cast<unsigned short>(255*(index)/(ccp4Header.NR*ccp4Header.NC));
                if(0 == index % ccp4Header.NC)row_pointers[rowIndex++]=(png_bytep)(&(buffer.get()[index]));
            }
                //std::cout<<(ccp4Header.NR*ccp4Header.NC)<<" rowIndex "<<rowIndex<<" "<<min<<" "<<max<<std::endl;
            CHECK_EQ(rowIndex, ccp4Header.NC);

            png_write_image(png_ptr, &row_pointers.get()[0]);
            //png_write_rows(png_ptr, row_pointers.get(), rowIndex);
            if (setjmp (png_jmpbuf (png_ptr))) {
//                std::cout<<"setjmp 6 "<<std::endl;
                return;
            }
            png_write_end(png_ptr, info_ptr);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            fclose(fp);
        });
        CHECK_EQ(mapInput.getHeader().NC, 234);
        CHECK_EQ(mapInput.getHeader().NR, 234);
        CHECK_EQ(mapInput.getHeader().NS, 234);
        CHECK_EQ(mapInput.getHeader().MODE, 2);
        CHECK(mapInput.getHeader().X_length == doctest::Approx(149.058f));
        CHECK(mapInput.getHeader().Y_length == doctest::Approx(149.058f));
        CHECK(mapInput.getHeader().Z_length == doctest::Approx(149.058f));
        //CHECK(mapInput.getHeader().Alpha == doctest::Approx(3.0f));
        //CHECK(mapInput.getHeader().Beta == doctest::Approx(3.0f));
        //CHECK(mapInput.getHeader().Gamma == doctest::Approx(3.0f));
        CHECK(mapInput.getHeader().AMIN == doctest::Approx(-0.01429f));
        CHECK(mapInput.getHeader().AMAX == doctest::Approx(0.03361f));
        CHECK(mapInput.getHeader().AMEAN == doctest::Approx(-0.00011f));
        CHECK_EQ(mapInput.getHeader().ISPG, 1);
        CHECK_EQ(mapInput.getHeader().NSYMBT, 0);
        CHECK_EQ(mapInput.getHeader().LSKFLG, 0);
        CHECK_EQ(mapInput.getHeader().NVERSION, 0);
         CHECK(mapInput.getHeader().ARMS == doctest::Approx(0.0029f));
       CHECK_EQ(mapInput.getHeader().NLABL, 1);
        CHECK_EQ(mapInput.getHeader().LABEL[0], "::::EMDATABANK.org::::EMD-8194::::                                              ");
        
    }
}

TEST_CASE("test symmetry operations on 1q8h"){
    std::string comp_id="1q8h";
    std::string url = "https://files.rcsb.org/download/"+comp_id+".cif";
    sylvanmats::constitution::Graph graph;
    std::filesystem::path  filePath="./"+comp_id+".cif.gz";
    sylvanmats::reading::TCPReader tcpReader;
    tcpReader(url, [&graph, &filePath, &comp_id](std::istream& is){
        sylvanmats::constitution::Populator populator;
        populator(is, graph, [&filePath](sylvanmats::constitution::Graph& graph){
//        std::cout<<"cb populator "<<std::endl;
       CHECK_EQ(graph.getNumberOfAtomSites(), 378);
       CHECK_EQ(graph.getCell().length_a, 51.491);
       CHECK_EQ(graph.getCell().length_b, 51.491);
       CHECK_EQ(graph.getCell().length_c, 35.389);
       CHECK_EQ(graph.getCell().angle_alpha, 90.000);
       CHECK_EQ(graph.getCell().angle_beta, 90.0);
       CHECK_EQ(graph.getCell().angle_gamma, 120.0);
       CHECK_EQ(graph.getSymmetry().space_group_name_H_M, "P 31 2 1");
       CHECK_EQ(graph.getSymmetry().Int_Tables_number, 152);

            std::array<unsigned int, 8> terminals{3, 3, 2, 0, 0, 0, 0, 1};
            unsigned int index=0;
            for(lemon::ListGraph::NodeIt nCompA(graph.componentGraph); nCompA!=lemon::INVALID; ++nCompA){
//                std::cout<<graph.componentProperties[nCompA].auth_mon_id<<" "<<graph.componentProperties[nCompA].termination<<" "<<graph.getComponentMaximumDiameter(nCompA)<<std::endl;
                //CHECK_EQ(graph.componentProperties[nCompA].termination, terminals[index++]);
            }    
       });
       CHECK_EQ(graph.getNumberOfAtomSites(), 378);
       CHECK_EQ(lemon::countEdges(graph), 316);
       CHECK_EQ(lemon::countNodes(graph.componentGraph), 113);
       CHECK_EQ(lemon::countEdges(graph.componentGraph), 47);

    });
    CHECK_EQ(graph.getOperationList().size(), 2);
    if(graph.getOperationList().size()>1){
        sylvanmats::linear::Matrix3d opMatrix((double*)&graph.getFractionalAtomSites().fract_transf_matrix);
        sylvanmats::linear::Vector3d opVector((double*)&graph.getFractionalAtomSites().fract_transf_vector);
        std::stringstream&& ssLua = graph.symmetryAsLua();
//        std::cout<<graph.getFractionalAtomSites().entry_id<<"\n"<<opMatrix<<"\n"<<"\n"<<opVector.transpose()<<std::endl;

        std::filesystem::path path="../templates/obj";
        sylvanmats::publishing::st::OBJPublisher objPublisher(path);
        std::string vertexCount=std::to_string(2*2*12*lemon::countEdges(graph));
//        std::cout<<"vertexCount "<<std::endl;
        objPublisher.add("vertex_count", vertexCount);
        std::string faceCount=std::to_string(2*12*lemon::countEdges(graph));
        objPublisher.add("face_count", faceCount);
        objPublisher.add("material_count", "1");
        std::vector<std::tuple<double, double, double, double, double, double>> vertexLoop;
        std::vector<std::tuple<double, double, double>> normalLoop;
//        std::vector<std::tuple<unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long, unsigned long long>> indexLoop;
        unsigned int faceIndex=1;
        unsigned int connectionIndex=0;
        lua_State *L = luaL_newstate();
        int err=luaL_dostring (L, ssLua.str().c_str());
        CHECK_EQ(err, LUA_OK);
//        std::cout<<err<<" "<<LUA_OK<<" luaL_pcall "<<std::endl;
        //err=lua_pcall(L, 0, 0, 0);
        //CHECK_EQ(err, LUA_ERRRUN);
//        std::cout<<"eSiteA "<<std::endl;
        for(lemon::ListGraph::EdgeIt eSiteA(graph); eSiteA!=lemon::INVALID; ++eSiteA){
            lemon::ListGraph::Node nSiteA=graph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=graph.v(eSiteA);
            sylvanmats::linear::Vector3d pA(graph.atomSites[nSiteA].Cartn_x, graph.atomSites[nSiteA].Cartn_y, graph.atomSites[nSiteA].Cartn_z);
            sylvanmats::linear::Vector3d pB(graph.atomSites[nSiteB].Cartn_x, graph.atomSites[nSiteB].Cartn_y, graph.atomSites[nSiteB].Cartn_z);
            sylvanmats::linear::Vector3d pAPrime=pA;
            sylvanmats::linear::Vector3d pBPrime=pB;
            for(unsigned int symIndex=0;symIndex<2;symIndex++){
                if(symIndex>=1){
                std::string fName="f"+std::to_string(symIndex+1);
                    pAPrime=opMatrix.transpose()*(pA-opVector);//+opVector;
                    pBPrime=opMatrix.transpose()*(pB-opVector);//+opVector;
                    //std::cout<<"pA "<<pA.transpose()<<" <--> "<<pB.transpose()<<" pB "<<std::endl;
                    lua_getglobal(L, fName.c_str());
                    lua_pushnumber(L, pAPrime[0]);
                    lua_pushnumber(L, pAPrime[1]);
                    lua_pushnumber(L, pAPrime[2]);
                    if (lua_pcall(L, 3, 3, 0) != LUA_OK) {
                            printf("Error calling sym.f2");
                            lua_error(L);
                            return;
                    }
                    else{
                        pAPrime[0]=lua_tonumber(L, -3);
                        pAPrime[1]=lua_tonumber(L, -2);
                        pAPrime[2]=lua_tonumber(L, -1);
                        lua_pop(L, 3);
                    }
                    lua_getglobal(L, fName.c_str());
                    lua_pushnumber(L, pBPrime[0]);
                    lua_pushnumber(L, pBPrime[1]);
                    lua_pushnumber(L, pBPrime[2]);
                    if (lua_pcall(L, 3, 3, 0) != LUA_OK) {
                            printf("Error calling sym.f2");
                            lua_error(L);
                            return;
                    }
                    else{
                        pBPrime[0]=lua_tonumber(L, -3);
                        pBPrime[1]=lua_tonumber(L, -2);
                        pBPrime[2]=lua_tonumber(L, -1);
                        lua_pop(L, 3);
                    }
                    //std::cout<<"\tpA "<<pA.transpose()<<" <--> "<<pB.transpose()<<" pB "<<std::endl;
                    pAPrime=opMatrix.transpose()*(pAPrime);//+opVector;
                    pBPrime=opMatrix.transpose()*(pBPrime);//+opVector;
                    pAPrime+=opVector;
                    pBPrime+=opVector;
                }
            sylvanmats::linear::Vector3d rotationVector=pBPrime-pAPrime;
            sylvanmats::linear::Vector3d averageVector=(pBPrime+pAPrime)/2.0;
            double d=rotationVector.norm();
            sylvanmats::linear::Vector3d ek=sylvanmats::linear::Vector3d::UnitX();
            double dot=std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitX()));
            if(std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitY()))<dot){ek=sylvanmats::linear::Vector3d::UnitY();dot=std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitY()));}
            if(std::abs(rotationVector.dot(sylvanmats::linear::Vector3d::UnitZ()))<dot)ek=sylvanmats::linear::Vector3d::UnitZ();
            ek.normalize();
            //ek=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            sylvanmats::linear::Vector3d vk=rotationVector.normalized().cross(ek);
            sylvanmats::linear::Vector3d vn=rotationVector.normalized().cross(vk);
            vn.normalize();
            double cylinderRadius=0.075;
            sylvanmats::linear::Vector3d t = {0.0, 0.0, 0.0};//p1 - p2;
            //std::cout<<rotationVector.transpose()<<" "<<averageVector.transpose()<<" vk "<<vk<<" vn "<<vn.transpose()<<" "<<d<<" angle "<<(180.0*std::atan(cylinderRadius/(d/2.0))/std::numbers::pi)<<std::endl;
            sylvanmats::algebra::geometric::Motor<double> quatA(t, (vk).normalized(), std::atan(cylinderRadius/(d/2.0)));
            sylvanmats::algebra::geometric::Motor<double> quatB(t, (vk).normalized(), -std::atan(cylinderRadius/(d/2.0)));
            sylvanmats::linear::Vector3d vA=quatA*(pAPrime-averageVector);
            sylvanmats::linear::Vector3d vB=quatB*(pBPrime-averageVector);
            //std::cout<<"vA "<<vA.transpose()<<"vB "<<vB.transpose()<<std::endl;
            sylvanmats::algebra::geometric::Motor<double> quat(t, rotationVector.normalized(), std::numbers::pi/6.0);
            sylvanmats::algebra::geometric::Motor<double> quatHalf(t, rotationVector.normalized(), std::numbers::pi/6.0/2.0);
            vn=quatHalf*vn;
            sylvanmats::linear::Vector3d v=vA+averageVector;
            sylvanmats::linear::Vector3d c(0.8, 0.8, 0.8);
            if(graph.atomSites[nSiteA].type_symbol.compare("C")==0)c=sylvanmats::linear::Vector3d(1.0, 0.0, 0.0);
            else if(graph.atomSites[nSiteA].type_symbol.compare("O")==0)c=sylvanmats::linear::Vector3d(0.0, 0.0, 1.0);
            else if(graph.atomSites[nSiteA].type_symbol.compare("N")==0)c=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            for(unsigned int index=0;index<11;index++){
                vA=quat*(vA);
                v=vA+averageVector;
                vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            }
            v=vB+averageVector;
            if(graph.atomSites[nSiteB].type_symbol.compare("C")==0)c=sylvanmats::linear::Vector3d(1.0, 0.0, 0.0);
            else if(graph.atomSites[nSiteB].type_symbol.compare("O")==0)c=sylvanmats::linear::Vector3d(0.0, 0.0, 1.0);
            else if(graph.atomSites[nSiteB].type_symbol.compare("N")==0)c=sylvanmats::linear::Vector3d(0.0, 1.0, 0.0);
            else c=sylvanmats::linear::Vector3d(0.7, 0.7, 0.8);
            vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            for(unsigned int index=0;index<11;index++){
                vB=quat*(vB);
                v=vB+averageVector;
                vertexLoop.insert(vertexLoop.begin(), std::make_tuple(v[0], v[1], v[2], c[0], c[1], c[2]));
            }
            normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
            for(unsigned int index=0;index<11;index++){
                vn=quat*vn;
                normalLoop.insert(normalLoop.begin(), std::make_tuple(vn[0], vn[1], vn[2]));
            }
            for(unsigned long long index=1+connectionIndex;index<=12-1+connectionIndex;index++){//[ a, c, b, c, b, d ]
                objPublisher.appendIndices(std::make_tuple(index, faceIndex, index+1, faceIndex, index+12, faceIndex, index+12+1, faceIndex));
//                indexLoop.insert(indexLoop.begin(), std::make_tuple(index, faceIndex, index+1, faceIndex, index+12, faceIndex, index+12+1, faceIndex));
                //indexLoop.insert(indexLoop.begin(), std::make_tuple(index+12, 2*index, index, 2*index, index+12+1, 2*index));
                faceIndex++;
            }
            unsigned long long index=1+connectionIndex;
            objPublisher.appendIndices(std::make_tuple(index-1+12, faceIndex, index, faceIndex, index-1+2*12, faceIndex, index+12, faceIndex));
            //indexLoop.insert(indexLoop.begin(), std::make_tuple(index-1+12, faceIndex, index, faceIndex, index-1+2*12, faceIndex, index+12, faceIndex));
            //indexLoop.insert(indexLoop.begin(), std::make_tuple(2*12, 2*index, 12, 2*index, 12+1, 2*index));
            connectionIndex+=24;
            faceIndex++;
            }
        }
//        objPublisher.add("vertices", vertexLoop);
//        objPublisher.add("texture_vertices", vertexLoop);
//        objPublisher.add("normals", normalLoop);
//        objPublisher.add("indices", indexLoop);
        std::string&& content2 = objPublisher.render();
        std::ofstream ofs2("../../server/public/molecule2.obj");
        ofs2<<content2<<std::endl;

        //std::cout<<graph.getOperationList()[1].type<<" "<<graph.getOperationList()[1].name<<" "<<graph.getOperationList()[1].symmetry_operation<<"\n"<<opMatrix<<"\n"<<"\n"<<opVector.transpose()<<std::endl;
    }
}

}

