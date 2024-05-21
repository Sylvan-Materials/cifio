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

#include "density/cell.h"
#include "linear/Array.h"

#include "mio/mmap.hpp"

namespace sylvanmats::density::mtz{
    
    struct labels{
        std::string label{};
        std::string type{};
        double minimum;
        double maximum;
        unsigned int dataset_id=0;
    };
    
    struct mtz_header{
        std::string version{};
        std::string title{};
        unsigned int number_of_columns=0;
        unsigned int number_of_reflections_in_file=0;
        unsigned int number_of_batches=0;
        unsigned int number_of_symmetry_operations;
        unsigned int number_of_primitive_operations;
        unsigned int number_of_datasets;
        sylvanmats::density::cell<double> cell;
        double minimum_resolution;
        double maximum_resolution;
        std::vector<labels> column_labels;
        std::vector<std::string> symmetry_operations;
    };

    class Input{
    protected:
        std::string mtzSignature={0,0,0,0};
        std::string machineStamp={0,0,0,0};
        int headerOffset=0;
        mtz_header mtzHeader;
        std::vector<std::string> headerKeys={"VERS", "TITLE", "NCOL", "CELL", "SORT", "SYMINF", "SYMM", "RESO", "VALM", "COL", "NDIF", "PROJECT", "CRYSTAL", "DATASET", "DCELL", "DWAVEL", "BATCH"};
        std::vector<sylvanmats::linear::ArrayXf> reflections;
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
            mio::mmap_source mmap2nd(filePath.string(), headerOffset, (fileSize-headerOffset));
            int headerPosition=0;
            for(int index=0;index<std::min(1, (int)headerKeys.size());index++){
                for(int keyIndex=0;keyIndex<headerKeys.size();keyIndex++){
                    if(std::strncmp((char*)&mmap2nd[headerPosition], headerKeys[keyIndex].c_str(), headerKeys[keyIndex].size())==0){
                        if(headerKeys[keyIndex].compare("VERS")==0){
                            std::string VERS=std::string((char*)&mmap2nd[headerPosition], 15);
                            mtzHeader.version=VERS.substr(5);
                            trim(mtzHeader.version);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("TITLE")==0){
                            std::string TITLE((char*)&mmap2nd[headerPosition], 76);
                            mtzHeader.title=TITLE.substr(6);
                            trim(mtzHeader.title);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("NCOL")==0){
                            std::vector<std::string>&& NCOL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(NCOL.size()>1)mtzHeader.number_of_columns=std::strtoul(NCOL[1].c_str(), nullptr, 10);
                            if(NCOL.size()>2)mtzHeader.number_of_reflections_in_file=std::strtoul(NCOL[2].c_str(), nullptr, 10);
                            if(NCOL.size()>3)mtzHeader.number_of_batches=std::strtoul(NCOL[3].c_str(), nullptr, 10);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("CELL")==0){
                            std::vector<std::string>&& CELL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(CELL.size()>1)mtzHeader.cell.a=std::strtod(CELL[1].c_str(), nullptr);
                            if(CELL.size()>2)mtzHeader.cell.b=std::strtod(CELL[2].c_str(), nullptr);
                            if(CELL.size()>3)mtzHeader.cell.c=std::strtod(CELL[3].c_str(), nullptr);
                            if(CELL.size()>4)mtzHeader.cell.α=std::strtod(CELL[4].c_str(), nullptr);
                            if(CELL.size()>5)mtzHeader.cell.β=std::strtod(CELL[5].c_str(), nullptr);
                            if(CELL.size()>6)mtzHeader.cell.γ=std::strtod(CELL[6].c_str(), nullptr);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("SORT")==0){
                            std::string SORT((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<SORT<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("SYMINF")==0){
                            std::vector<std::string>&& SYMINF=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(SYMINF.size()>1)mtzHeader.number_of_symmetry_operations=std::strtoul(SYMINF[1].c_str(), nullptr, 10);
                            if(SYMINF.size()>2)mtzHeader.number_of_primitive_operations=std::strtoul(SYMINF[2].c_str(), nullptr, 10);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("SYMM")==0){
                            for(unsigned int symIndex=0;symIndex<mtzHeader.number_of_symmetry_operations;symIndex++){
                                std::string SYMM((char*)&mmap2nd[headerPosition+4], 76);
                                trim(SYMM);
                                mtzHeader.symmetry_operations.push_back(SYMM);
//                                std::cout<<SYMM<<std::endl;
                                headerPosition+=80;
                            }
                        }
                        else if(headerKeys[keyIndex].compare("RESO")==0){
                            std::vector<std::string>&& RESO=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(RESO.size()>1)mtzHeader.minimum_resolution=std::strtod(RESO[1].c_str(), nullptr);
                            if(RESO.size()>2)mtzHeader.maximum_resolution=std::strtod(RESO[2].c_str(), nullptr);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("VALM")==0){
                            std::string VALM((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<VALM<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("COL")==0){
                            for(unsigned int index=0;index<mtzHeader.number_of_columns;index++){
                                mtzHeader.column_labels.push_back(labels{});
                                std::vector<std::string>&& COL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                                if(COL.size()>1)mtzHeader.column_labels.back().label=COL[1];
                                if(COL.size()>2)mtzHeader.column_labels.back().type=COL[2];
                                if(COL.size()>3)mtzHeader.column_labels.back().minimum=std::strtod(COL[3].c_str(), nullptr);
                                if(COL.size()>4)mtzHeader.column_labels.back().maximum=std::strtod(COL[4].c_str(), nullptr);
                                if(COL.size()>5)mtzHeader.column_labels.back().dataset_id=std::strtoul(COL[5].c_str(), nullptr, 10);
                                headerPosition+=80;
                                std::string COLSRC((char*)&mmap2nd[headerPosition], 80);
                                if(COLSRC.substr(0, 6).compare("COLSRC")==0){
                                    std::cout<<COLSRC<<std::endl;
                                    headerPosition+=80;
                                }
                            }
                        }
                        else if(headerKeys[keyIndex].compare("NDIF")==0){
                            std::vector<std::string>&& NDIF=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(NDIF.size()>1)mtzHeader.number_of_datasets=std::strtoul(NDIF[1].c_str(), nullptr, 10);
                            std::cout<<"NDIF "<<mtzHeader.number_of_datasets<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("PROJECT")==0){
                            std::string PROJECT((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<PROJECT<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("CRYSTAL")==0){
                            std::string CRYSTAL((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<CRYSTAL<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("DATASET")==0){
                            std::string DATASET((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<DATASET<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("DCELL")==0){
                            std::vector<std::string>&& DCELL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(DCELL.size()>2)mtzHeader.cell.a=std::strtod(DCELL[2].c_str(), nullptr);
                            if(DCELL.size()>3)mtzHeader.cell.b=std::strtod(DCELL[3].c_str(), nullptr);
                            if(DCELL.size()>4)mtzHeader.cell.c=std::strtod(DCELL[4].c_str(), nullptr);
                            if(DCELL.size()>5)mtzHeader.cell.α=std::strtod(DCELL[5].c_str(), nullptr);
                            if(DCELL.size()>6)mtzHeader.cell.β=std::strtod(DCELL[6].c_str(), nullptr);
                            if(DCELL.size()>7)mtzHeader.cell.γ=std::strtod(DCELL[7].c_str(), nullptr);
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("DWAVEL")==0){
                            std::string DWAVEL((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<DWAVEL<<std::endl;
                            headerPosition+=80;
                        }
                        else if(headerKeys[keyIndex].compare("BATCH")==0){
                            std::string BATCH((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<BATCH<<" "<<headerPosition<<std::endl;
                            headerPosition+=80;
                        }
                    }
                }
            }
//            for(unsigned int index=0;index<30;index++){
//            std::string history((char*)&mmap2nd[1380+80*index], 80);
//            std::cout<<index<<" "<<history<<std::endl;
//                
//            }
////            std::strncpy((char*)&mtzHeader.NC, &mmap2nd[0], 4);
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
            mio::mmap_source mmapReflections(filePath.string(), 20, (headerOffset-20));
            for(unsigned int colIndex=0;colIndex<mtzHeader.number_of_columns;colIndex++){
                reflections.push_back(sylvanmats::linear::ArrayXf(mtzHeader.number_of_reflections_in_file));
            }
            std::cout<<"refl "<<(headerOffset-20)<<" "<<(4*mtzHeader.number_of_columns*mtzHeader.number_of_reflections_in_file)<<std::endl;
            int reflectionOffset=0;
            for(unsigned int rowIndex=0;rowIndex<mtzHeader.number_of_reflections_in_file;rowIndex++){
            for(unsigned int colIndex=0;colIndex<3;colIndex++){
                reflections[colIndex][rowIndex]=((float*)&mmapReflections[0])[reflectionOffset];
//                std::cout<<std::setw(5)<<std::setprecision(2)<<reflections[colIndex][rowIndex];
                reflectionOffset++;
            }
            for(unsigned int colIndex=3;colIndex<mtzHeader.number_of_columns;colIndex++){
                reflections[colIndex][rowIndex]=((float*)&mmapReflections[0])[reflectionOffset];
//                std::cout<<std::setw(5)<<std::setprecision(2)<<reflections[colIndex][rowIndex];
                reflectionOffset++;
            }
//            std::cout<<std::endl;
            }
            mmapReflections.unmap();
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
