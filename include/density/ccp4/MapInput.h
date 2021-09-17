#pragma once

#include <stdio.h>
#include <memory>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <filesystem>
#include <ranges>
#include <vector>
#include <functional>
#include <chrono>

#include "mio/mmap.hpp"

namespace sylvanmats::density::ccp4{
    struct ccp4_header{
        int NC;        //# of Columns    (fastest changing in map)
        int NR;        //# of Rows
        int NS;        //# of Sections   (slowest changing in map)
        int MODE;      //  Data type
                        //  0 = envelope stored as signed bytes (from
                        //      -128 lowest to 127 highest)
                        //  1 = Image     stored as Integer*2
                        //  2 = Image     stored as Reals
                        //  3 = Transform stored as Complex Integer*2
                        //  4 = Transform stored as Complex Reals
                        //  5 == 0
//
                        //  Note: Mode 2 is the normal mode used in
                        //        the CCP4 programs. Other modes than 2 and 0
                        //        may NOT WORK
//
        int NCSTART;         //Number of first COLUMN  in map
        int NRSTART;         //Number of first ROW     in map
        int NSSTART;         //Number of first SECTION in map
        int NX;              //Number of intervals along X
        int NY;              //Number of intervals along Y
        int NZ;              //Number of intervals along Z
        float X_length;        //Cell Dimensions (Angstroms)
        float Y_length;        //             "
        float Z_length;        //             "
        float Alpha;          //Cell Angles     (Degrees)
        float Beta;           //             "
        float Gamma;          //             "
        int MAPC;            //Which axis corresponds to Cols.  (1,2,3 for X,Y,Z)
        int MAPR;            //Which axis corresponds to Rows   (1,2,3 for X,Y,Z)
        int MAPS;            //Which axis corresponds to Sects. (1,2,3 for X,Y,Z)
        float AMIN;           //Minimum density value
        float AMAX;           //Maximum density value
        float AMEAN;          //Mean    density value    (Average)
        int ISPG;            //Space group number
        int NSYMBT;          //Number of bytes used for storing symmetry operators
        int LSKFLG;          //Flag for skew transformation, =0 none, =1 if foll
        float SKWMAT_S11;      //Skew matrix S (in order S11, S12, S13, S21 etc) if
                        //LSKFLG .ne. 0.
        float SKWMAT_S12;
        float SKWMAT_S13;
        float SKWMAT_S21;
        float SKWMAT_S22;
        float SKWMAT_S23;
        float SKWMAT_S31;
        float SKWMAT_S32;
        float SKWMAT_S33;
        std::string EXTTYPE={0,0,0,0,0};
        int NVERSION;
//35-37   SKWTRN          //Skew translation t if LSKFLG .ne. 0.
                        //Skew transformation is from standard orthogonal
                        //coordinate frame (as used for atoms) to orthogonal
                        //map frame, as

//                                Xo(map) = S * (Xo(atoms) - t)

/*38      future use       (some of these are used by the MSUBSX routines
 .          "              in MAPBRICK, MAPCONT and FRODO)
 .          "   (all set to zero by default)
 .          "
52          "*/

        std::string MAP={0,0,0,0,0};            //Character string 'MAP ' to identify file type
        int MACHST;         //Machine stamp indicating the machine type
                        //which wrote file
        float ARMS;           //Rms deviation of map from mean density
        int NLABL;           //Number of labels being used
        std::vector<std::string> LABEL;   //10  80 character text labels (ie. A4 format)*/        
    };

    class MapInput{
    protected:
        ccp4_header ccp4Header;
    public:
        MapInput(){};
        MapInput(const MapInput& orig) = delete;
        virtual ~MapInput() = default;
    public:
        void operator()(std::filesystem::path& filePath, std::function<void(ccp4_header& ccp4Header, unsigned int sectionIndex, float*slice)> apply){
            std::uintmax_t fileSize=std::filesystem::file_size(filePath);
            mio::mmap_source mmap2nd(filePath.string(), 0, 1024);
            std::strncpy((char*)&ccp4Header.NC, &mmap2nd[0], 4);
            std::strncpy((char*)&ccp4Header.NR, &mmap2nd[4], 4);
            std::strncpy((char*)&ccp4Header.NS, &mmap2nd[8], 4);
            std::strncpy((char*)&ccp4Header.MODE, &mmap2nd[12], 4);
            std::strncpy((char*)&ccp4Header.NCSTART, &mmap2nd[16], 4);
            std::strncpy((char*)&ccp4Header.NRSTART, &mmap2nd[20], 4);
            std::strncpy((char*)&ccp4Header.NSSTART, &mmap2nd[24], 4);
            std::strncpy((char*)&ccp4Header.NX, &mmap2nd[28], 4);
            std::strncpy((char*)&ccp4Header.NY, &mmap2nd[32], 4);
            std::strncpy((char*)&ccp4Header.NZ, &mmap2nd[36], 4);
            std::strncpy((char*)&ccp4Header.X_length, &mmap2nd[40], 4);
            std::strncpy((char*)&ccp4Header.Y_length, &mmap2nd[44], 4);
            std::strncpy((char*)&ccp4Header.Z_length, &mmap2nd[48], 4);
            std::strncpy((char*)&ccp4Header.Alpha, &mmap2nd[52], 4);
            std::strncpy((char*)&ccp4Header.Beta, &mmap2nd[56], 4);
            std::strncpy((char*)&ccp4Header.Gamma, &mmap2nd[60], 4);
//std::cout<<"Gamma "<<ccp4Header.Gamma<<std::endl;
            std::strncpy((char*)&ccp4Header.MAPC, &mmap2nd[64], 4);
            std::strncpy((char*)&ccp4Header.MAPR, &mmap2nd[68], 4);
            std::strncpy((char*)&ccp4Header.MAPS, &mmap2nd[72], 4);
            std::strncpy((char*)&ccp4Header.AMIN, &mmap2nd[76], 4);
            std::strncpy((char*)&ccp4Header.AMAX, &mmap2nd[80], 4);
            std::strncpy((char*)&ccp4Header.AMEAN, &mmap2nd[84], 4);
            std::strncpy((char*)&ccp4Header.ISPG, &mmap2nd[88], 4);
            std::strncpy((char*)&ccp4Header.NSYMBT, &mmap2nd[92], 4);
            std::strncpy((char*)&ccp4Header.LSKFLG, &mmap2nd[96], 4);
            ccp4Header.EXTTYPE=std::string((char*)&mmap2nd[104], 4);
//std::cout<<"EXTTYPE "<<ccp4Header.EXTTYPE<<std::endl;
            std::strncpy((char*)&ccp4Header.NVERSION, &mmap2nd[108], 4);
            ccp4Header.MAP=std::string((char*)&mmap2nd[208], 4);
            std::strncpy((char*)&ccp4Header.MACHST, &mmap2nd[212], 4);
            std::strncpy((char*)&ccp4Header.ARMS, &mmap2nd[216], 4);
            std::strncpy((char*)&ccp4Header.NLABL, &mmap2nd[220], 4);
            for(unsigned int index=0;index<ccp4Header.NLABL;index++){
                ccp4Header.LABEL.push_back(std::string((char*)&mmap2nd[index*80+224], 80));
//std::cout<<ccp4Header.NLABL<<" LABEL "<<ccp4Header.LABEL.back()<<" "<<ccp4Header.LABEL.back().size()<<std::endl;
            }
            mmap2nd.unmap();
            unsigned int offset = 1024;//ccp4Header.NLABL*80+224;
//            std::cout<<"MACHST "<<std::hex<<" "<<ccp4Header.MACHST<<" "<<(ccp4Header.MACHST&0x0f)<<std::dec<<std::endl;
            //std::unique_ptr<float[]> slice(new float[ccp4Header.NR*ccp4Header.NC]);
            for(unsigned int sectionIndex=0;sectionIndex<ccp4Header.NS;sectionIndex++){
                mio::basic_mmap_source<std::byte> mmapSlice(filePath.string(), 4*sectionIndex*ccp4Header.NR*ccp4Header.NC+offset, 4*(1)*ccp4Header.NR*ccp4Header.NC);
                apply(ccp4Header, sectionIndex, (float*)&mmapSlice[0]);
                /*for(unsigned int index=0;index<mmapSlice.size();index+=4){
                    ((float*)&mmapSlice[index]);
                }*/
                mmapSlice.unmap();
            }
            
        };
        
        ccp4_header& getHeader(){return ccp4Header;};
    };
}