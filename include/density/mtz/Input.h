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
#include <cstdlib>
#include <cstring>

#include "mio/mmap.hpp"

namespace sylvanmats::density::mtz{
    
    struct mtz_header{
        std::string version{};
        std::string title{};
        unsigned int number_of_columns=0;
        unsigned int number_of_reflections_in_file=0;
        unsigned int number_of_batches=0;
        unsigned int number_of_symmetry_operations;
        unsigned int number_of_primitive_operations;
        std::vector<std::string> labels;       
    };

    class Input{
    protected:
        std::string mtzSignature={0,0,0,0};
        std::string machineStamp={0,0,0,0};
        int headerOffset=0;
        mtz_header mtzHeader;
    public:
        Input(){};
        Input(const Input& orig) = delete;
        virtual ~Input() = default;
    public:
        void operator()(std::filesystem::path& filePath, std::function<void(mtz_header& mtzHeader)> apply){
            std::uintmax_t fileSize=std::filesystem::file_size(filePath);
            mio::mmap_source mmap1rst(filePath.string(), 0, 20);
            mtzSignature=std::string((char*)&mmap1rst[0], 4);
            std::cout<<"Sig "<<mtzSignature<<" "<<(mtzSignature.compare("MTZ ")==0)<<std::endl;
            std::strncpy((char*)&headerOffset, &mmap1rst[4], 4);
            machineStamp=std::string((char*)&mmap1rst[8], 4);
            headerOffset=( 0x0f&machineStamp[0] )*(headerOffset-1);
            mmap1rst.unmap();
            mio::mmap_source mmap2nd(filePath.string(), headerOffset, 1760);
            std::string VERS=std::string((char*)&mmap2nd[0], 15);
            mtzHeader.version=VERS.substr(5);
            trim(mtzHeader.version);
            std::string TITLE((char*)&mmap2nd[80], 76);
            mtzHeader.title=TITLE.substr(6);
            trim(mtzHeader.title);
            std::vector<std::string>&& NCOL=tokenize(std::string((char*)&mmap2nd[160], 80));
            if(NCOL.size()>1)mtzHeader.number_of_columns=std::strtoul(NCOL[1].c_str(), nullptr, 10);
            if(NCOL.size()>2)mtzHeader.number_of_reflections_in_file=std::strtoul(NCOL[2].c_str(), nullptr, 10);
            if(NCOL.size()>3)mtzHeader.number_of_batches=std::strtoul(NCOL[3].c_str(), nullptr, 10);
            std::string CELL((char*)&mmap2nd[240], 80);
            std::cout<<CELL<<std::endl;
            std::string SORT((char*)&mmap2nd[320], 80);
            std::cout<<SORT<<std::endl;
            std::vector<std::string>&& SYMINF=tokenize(std::string((char*)&mmap2nd[400], 80));
            if(SYMINF.size()>1)mtzHeader.number_of_symmetry_operations=std::strtoul(SYMINF[1].c_str(), nullptr, 10);
            if(SYMINF.size()>2)mtzHeader.number_of_primitive_operations=std::strtoul(SYMINF[1].c_str(), nullptr, 10);
            unsigned int offset=480;
            for(unsigned int index=0;index<mtzHeader.number_of_symmetry_operations;index++){
                std::string SYMM((char*)&mmap2nd[offset], 80);
                std::cout<<SYMM<<std::endl;
                offset+=80;
            }
            std::string RESO((char*)&mmap2nd[offset], 80);
            std::cout<<"RESO "<<RESO<<std::endl;
            offset+=80;
            std::string VALM((char*)&mmap2nd[offset], 80);
            std::cout<<"VALM "<<VALM<<std::endl;
            offset+=80;
            for(unsigned int index=0;index<mtzHeader.number_of_columns;index++){
                std::string COL((char*)&mmap2nd[offset], 80);
                std::cout<<COL<<std::endl;
                offset+=80;
            }
            std::string NDIF((char*)&mmap2nd[offset], 80);
            std::cout<<"NDIF "<<NDIF<<std::endl;
            offset+=80;
//            std::string PROJECT((char*)&mmap2nd[offset], 80);
//            std::cout<<PROJECT<<std::endl;
//            offset+=80;
//            std::string CRYSTAL((char*)&mmap2nd[offset], 80);
//            std::cout<<CRYSTAL<<std::endl;
//            offset+=80;
//            std::string DATASET((char*)&mmap2nd[offset], 80);
//            std::cout<<DATASET<<std::endl;
//            offset+=80;
//            std::string DCELL((char*)&mmap2nd[offset], 80);
//            std::cout<<DCELL<<std::endl;
//            offset+=80;
//            std::string DWAVEL((char*)&mmap2nd[offset], 80);
//            std::cout<<DWAVEL<<std::endl;
//            offset+=80;
//            std::string BATCH((char*)&mmap2nd[offset], 80);
//            std::cout<<BATCH<<" "<<offset<<std::endl;
//            offset+=80;
//            for(unsigned int index=0;index<30;index++){
//            std::string history((char*)&mmap2nd[1380+80*index], 80);
//            std::cout<<index<<" "<<history<<std::endl;
//                
//            }
//            std::strncpy((char*)&mtzHeader.NC, &mmap2nd[0], 4);
//            std::strncpy((char*)&mtzHeader.NR, &mmap2nd[4], 4);
//            std::strncpy((char*)&mtzHeader.NS, &mmap2nd[8], 4);
//            std::strncpy((char*)&mtzHeader.MODE, &mmap2nd[12], 4);
//            std::strncpy((char*)&mtzHeader.NCSTART, &mmap2nd[16], 4);
//            std::strncpy((char*)&mtzHeader.NRSTART, &mmap2nd[20], 4);
//            std::strncpy((char*)&mtzHeader.NSSTART, &mmap2nd[24], 4);
//            std::strncpy((char*)&mtzHeader.NX, &mmap2nd[28], 4);
//            std::strncpy((char*)&mtzHeader.NY, &mmap2nd[32], 4);
//            std::strncpy((char*)&mtzHeader.NZ, &mmap2nd[36], 4);
//            std::strncpy((char*)&mtzHeader.X_length, &mmap2nd[40], 4);
//            std::strncpy((char*)&mtzHeader.Y_length, &mmap2nd[44], 4);
//            std::strncpy((char*)&mtzHeader.Z_length, &mmap2nd[48], 4);
//            std::strncpy((char*)&mtzHeader.Alpha, &mmap2nd[52], 4);
//            std::strncpy((char*)&mtzHeader.Beta, &mmap2nd[56], 4);
//            std::strncpy((char*)&mtzHeader.Gamma, &mmap2nd[60], 4);
//std::cout<<"Gamma "<<mtzHeader.Gamma<<std::endl;
//            std::strncpy((char*)&mtzHeader.MAPC, &mmap2nd[64], 4);
//            std::strncpy((char*)&mtzHeader.MAPR, &mmap2nd[68], 4);
//            std::strncpy((char*)&mtzHeader.MAPS, &mmap2nd[72], 4);
//            std::strncpy((char*)&mtzHeader.AMIN, &mmap2nd[76], 4);
//            std::strncpy((char*)&mtzHeader.AMAX, &mmap2nd[80], 4);
//            std::strncpy((char*)&mtzHeader.AMEAN, &mmap2nd[84], 4);
//            std::strncpy((char*)&mtzHeader.ISPG, &mmap2nd[88], 4);
//            std::strncpy((char*)&mtzHeader.NSYMBT, &mmap2nd[92], 4);
//            std::strncpy((char*)&mtzHeader.LSKFLG, &mmap2nd[96], 4);
//            mtzHeader.EXTTYPE=std::string((char*)&mmap2nd[104], 4);
//std::cout<<"EXTTYPE "<<mtzHeader.EXTTYPE<<std::endl;
//            std::strncpy((char*)&mtzHeader.NVERSION, &mmap2nd[108], 4);
//            mtzHeader.MAP=std::string((char*)&mmap2nd[208], 4);
//            std::strncpy((char*)&mtzHeader.MACHST, &mmap2nd[212], 4);
//            std::strncpy((char*)&mtzHeader.ARMS, &mmap2nd[216], 4);
//            std::strncpy((char*)&mtzHeader.NLABL, &mmap2nd[220], 4);
//            for(unsigned int index=0;index<mtzHeader.NLABL;index++){
//                mtzHeader.LABEL.push_back(std::string((char*)&mmap2nd[index*80+224], 80));
//std::cout<<mtzHeader.NLABL<<" LABEL "<<mtzHeader.LABEL.back()<<" "<<mtzHeader.LABEL.back().size()<<std::endl;
//            }
            mmap2nd.unmap();
//            unsigned int offset = 1024;//mtzHeader.NLABL*80+224;
//            std::cout<<"MACHST "<<std::hex<<" "<<mtzHeader.MACHST<<" "<<(mtzHeader.MACHST&0x0f)<<std::dec<<std::endl;
            //std::unique_ptr<float[]> slice(new float[mtzHeader.NR*mtzHeader.NC]);
//            for(unsigned int sectionIndex=0;sectionIndex<mtzHeader.NS;sectionIndex++){
//                mio::basic_mmap_source<std::byte> mmapSlice(filePath.string(), 4*sectionIndex*mtzHeader.NR*mtzHeader.NC+offset, 4*(1)*mtzHeader.NR*mtzHeader.NC);
//                apply(mtzHeader);
//                /*for(unsigned int index=0;index<mmapSlice.size();index+=4){
//                    ((float*)&mmapSlice[index]);
//                }*/
//                mmapSlice.unmap();
//            }
//            
        };
        
        mtz_header& getHeader(){return mtzHeader;};
        
    private:
        std::vector<std::string> tokenize(std::string input){
            std::vector<std::string> stack;
            const char* delimiters = " '";
            char* token = std::strtok((char*)input.c_str(), delimiters);
            while (token)
            {
                stack.push_back(std::string(token));
                token = std::strtok(nullptr, delimiters);
            }
            return stack;
        }
        
        void trim(std::string & source) {
            source.erase(0,source.find_first_not_of(" \n\r\t"));
            source.erase(source.find_last_not_of(" \n\r\t")+1);
        }
        
    };
}
