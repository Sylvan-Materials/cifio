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
#include <regex>
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
    
    struct crystals{
        unsigned int id=0;
        std::string name{};
        std::string project{};
        std::string dataset_name{};
        unsigned int dataset_id;
        unsigned int number_of_columns=0;
        sylvanmats::density::cell<double> cell;
        double wave_length=0.0;
    };
    
    struct mtz_header{
        std::string version{};
        std::string title{};
        unsigned int number_of_columns=0;
        unsigned int number_of_reflections_in_file=0;
        unsigned int number_of_batches=0;
        unsigned int number_of_symmetry_operations;
        unsigned int number_of_primitive_operations;
        std::string lattice_type{};
        unsigned int space_group_number;
        std::string space_group_name{};
        std::string point_group_name{};
        unsigned int number_of_datasets;
        sylvanmats::density::cell<double> cell;
        double minimum_resolution;
        double maximum_resolution;
        std::vector<labels> column_labels;
        std::vector<std::string> symmetry_operations;
        std::vector<crystals> structure_crystals;
    };

    class Input{
    protected:
        std::string mtzSignature={0,0,0,0};
        std::string machineStamp={0,0,0,0};
        int headerOffset=0;
        mtz_header mtzHeader;
        std::vector<std::string> headerKeys={"VERS", "TITLE", "NCOL", "CELL", "SORT", "SYMINF", "SYMM", "RESO", "VALM", "COL", "NDIF"};
        std::vector<std::string> crystalKeys={"PROJECT", "CRYSTAL", "DATASET", "DCELL", "DWAVEL", "BATCH"};
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
                            std::string syminf((char*)&mmap2nd[headerPosition+6], 74);
                            std::cout<<syminf<<std::endl;
                            std::regex syminfRegex(R"(\s*(\d+)\s*(\d+)\s*(\w)\s*(\d+)\s*'((\w| )+)'\s*(\w+)\s*)");
                            std::smatch syminfMatch;
                            if (std::regex_match(syminf, syminfMatch, syminfRegex)){
                                if (syminfMatch.size()>1){
                                    mtzHeader.number_of_symmetry_operations=std::strtoul(syminfMatch[1].str().c_str(), nullptr, 10);
                                }
                                if (syminfMatch.size()>2){
                                    mtzHeader.number_of_primitive_operations=std::strtoul(syminfMatch[2].str().c_str(), nullptr, 10);
                                }
                                if (syminfMatch.size()>3){
                                    mtzHeader.lattice_type=syminfMatch[3].str();
                                }
                                if (syminfMatch.size()>4){
                                    mtzHeader.space_group_number=std::strtoul(syminfMatch[4].str().c_str(), nullptr, 10);
                                }
                                if (syminfMatch.size()>5){
                                    mtzHeader.space_group_name=syminfMatch[5].str();
                                }
                                if (syminfMatch.size()>7){
                                    mtzHeader.point_group_name=syminfMatch[7].str();
                                }
                            }
//                            std::vector<std::string>&& SYMINF=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
//                            if(SYMINF.size()>1)mtzHeader.number_of_symmetry_operations=std::strtoul(SYMINF[1].c_str(), nullptr, 10);
//                            if(SYMINF.size()>2)mtzHeader.number_of_primitive_operations=std::strtoul(SYMINF[2].c_str(), nullptr, 10);
//                            if(SYMINF.size()>3)mtzHeader.lattice_type=SYMINF[3];
//                            if(SYMINF.size()>4)mtzHeader.space_group_number=std::strtoul(SYMINF[4].c_str(), nullptr, 10);
//                            if(SYMINF.size()>5)mtzHeader.space_group_name=SYMINF[5];
//                            if(SYMINF.size()>6)mtzHeader.point_group_name=SYMINF[6];
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
                                std::string tCOL((char*)&mmap2nd[headerPosition], 80);
//                                    std::cout<<tCOL<<std::endl;
                                std::vector<std::string>&& COL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                                if(COL.size()>1)mtzHeader.column_labels.back().label=COL[1];
                                if(COL.size()>2)mtzHeader.column_labels.back().type=COL[2];
                                if(COL.size()>3)mtzHeader.column_labels.back().minimum=std::strtod(COL[3].c_str(), nullptr);
                                if(COL.size()>4)mtzHeader.column_labels.back().maximum=std::strtod(COL[4].c_str(), nullptr);
                                if(COL.size()>5)mtzHeader.column_labels.back().dataset_id=std::strtoul(COL[5].c_str(), nullptr, 10);
                                headerPosition+=80;
                                std::string COLSRC((char*)&mmap2nd[headerPosition], 80);
                                if(COLSRC.substr(0, 6).compare("COLSRC")==0){
//                                    std::cout<<COLSRC<<std::endl;
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
                    }
                }
            }
            for(int index=0;index<mtzHeader.number_of_datasets;index++){
                for(int keyIndex=0;keyIndex<crystalKeys.size();keyIndex++){
                    if(std::strncmp((char*)&mmap2nd[headerPosition], crystalKeys[keyIndex].c_str(), crystalKeys[keyIndex].size())==0){
                        if(crystalKeys[keyIndex].compare("PROJECT")==0){
                            std::vector<std::string>&& PROJECT=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
//                            std::string sPROJECT((char*)&mmap2nd[headerPosition], 80);
//                            std::cout<<sPROJECT<<std::endl;
                            mtzHeader.structure_crystals.push_back(crystals{});
//                            if(PROJECT.size()>1)mtzHeader.structure_crystals.back().project=std::strtod(PROJECT[1].c_str(), nullptr);
                            if(PROJECT.size()>2)mtzHeader.structure_crystals.back().project=PROJECT[2];
                            headerPosition+=80;
                        }
                        else if(crystalKeys[keyIndex].compare("CRYSTAL")==0){
                            std::vector<std::string>&& CRYSTAL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
//                            std::string sCRYSTAL((char*)&mmap2nd[headerPosition], 80);
//                            std::cout<<sCRYSTAL<<std::endl;
                            if(CRYSTAL.size()>1)mtzHeader.structure_crystals.back().id=std::strtoul(CRYSTAL[1].c_str(), nullptr, 10);
                            if(CRYSTAL.size()>2)mtzHeader.structure_crystals.back().name=CRYSTAL[2];
                            headerPosition+=80;
                        }
                        else if(crystalKeys[keyIndex].compare("DATASET")==0){
                            std::vector<std::string>&& DATASET=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
//                            std::string sDATASET((char*)&mmap2nd[headerPosition], 80);
//                            std::cout<<sDATASET<<std::endl;
                            if(DATASET.size()>1)mtzHeader.structure_crystals.back().dataset_id=std::strtoul(DATASET[1].c_str(), nullptr, 10);
                            if(DATASET.size()>2)mtzHeader.structure_crystals.back().dataset_name=DATASET[2];
                            headerPosition+=80;
                        }
                        else if(crystalKeys[keyIndex].compare("DCELL")==0){
                            std::vector<std::string>&& DCELL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            if(DCELL.size()>2)mtzHeader.structure_crystals.back().cell.a=std::strtod(DCELL[2].c_str(), nullptr);
                            if(DCELL.size()>3)mtzHeader.structure_crystals.back().cell.b=std::strtod(DCELL[3].c_str(), nullptr);
                            if(DCELL.size()>4)mtzHeader.structure_crystals.back().cell.c=std::strtod(DCELL[4].c_str(), nullptr);
                            if(DCELL.size()>5)mtzHeader.structure_crystals.back().cell.α=std::strtod(DCELL[5].c_str(), nullptr);
                            if(DCELL.size()>6)mtzHeader.structure_crystals.back().cell.β=std::strtod(DCELL[6].c_str(), nullptr);
                            if(DCELL.size()>7)mtzHeader.structure_crystals.back().cell.γ=std::strtod(DCELL[7].c_str(), nullptr);
                            headerPosition+=80;
                        }
                        else if(crystalKeys[keyIndex].compare("DWAVEL")==0){
                            std::vector<std::string>&& DWAVEL=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
//                            std::string sDWAVEL((char*)&mmap2nd[headerPosition], 80);
//                            std::cout<<sDWAVEL<<std::endl;
                            if(DWAVEL.size()>2)mtzHeader.structure_crystals.back().wave_length=std::strtod(DWAVEL[1].c_str(), nullptr);
                            headerPosition+=80;
                        }
                        else if(crystalKeys[keyIndex].compare("BATCH")==0){
                            std::vector<std::string>&& BATCH=tokenize(std::string((char*)&mmap2nd[headerPosition], 80));
                            std::string sBATCH((char*)&mmap2nd[headerPosition], 80);
                            std::cout<<sBATCH<<" "<<headerPosition<<std::endl;
//                            if(DWAVEL.size()>2)mtzHeader.structure_crystals.back().wave_length=std::strtod(DWAVEL[1].c_str(), nullptr);
                            headerPosition+=80;
                        }
                    }
                }
            }
//                            std::string CHECK1((char*)&mmap2nd[headerPosition], 80);
//                            std::cout<<"check? "<<CHECK1<<std::endl;
//                            std::string CHECK2((char*)&mmap2nd[headerPosition+80], 80);
//                            std::cout<<"check? "<<CHECK2<<std::endl;
//            for(unsigned int index=0;index<30;index++){
//            std::string history((char*)&mmap2nd[1380+80*index], 80);
//            std::cout<<index<<" "<<history<<std::endl;
//                
//            }
            mmap2nd.unmap();
            for(unsigned int crystalIndex=0;crystalIndex<mtzHeader.structure_crystals.size();crystalIndex++){
                for(unsigned int colIndex=0;colIndex<mtzHeader.number_of_columns;colIndex++){
                    if(mtzHeader.column_labels[colIndex].dataset_id==mtzHeader.structure_crystals[crystalIndex].dataset_id)mtzHeader.structure_crystals[crystalIndex].number_of_columns++;
                }
            }
            mio::mmap_source mmapReflections(filePath.string(), 20, (headerOffset-20));
            for(unsigned int colIndex=0;colIndex<mtzHeader.number_of_columns;colIndex++){
                reflections.push_back(sylvanmats::linear::ArrayXf(mtzHeader.number_of_reflections_in_file));
            }
            int reflectionOffset=15;
            for(unsigned int rowIndex=0;rowIndex<mtzHeader.number_of_reflections_in_file;rowIndex++){
            for(unsigned int colIndex=0;colIndex<mtzHeader.number_of_columns;colIndex++){
                    reflections[colIndex][rowIndex]=((float*)&mmapReflections[0])[reflectionOffset];
                reflectionOffset++;
            }
////            for(unsigned int colIndex=3;colIndex<mtzHeader.number_of_columns;colIndex++){
////                reflections[colIndex][rowIndex]=((float*)&mmapReflections[0])[reflectionOffset];
////                reflectionOffset++;
////            }
            }
//            std::cout<<"S? "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<std::endl;
//            std::cout<<"Centric flag? "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<std::endl;
//            std::cout<<"Hendrickson-Lattman? "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<" "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<" "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<" "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<std::endl;
//            std::cout<<"Figure of merit? "<<(((float*)&mmapReflections[0])[reflectionOffset++])<<std::endl;
            mmapReflections.unmap();
        };
        
        mtz_header& getHeader(){return mtzHeader;};
        
    private:
        std::vector<std::string> tokenize(std::string input, const char* delimiters = " '"){
            std::vector<std::string> stack;

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
