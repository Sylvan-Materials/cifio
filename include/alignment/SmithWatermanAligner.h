#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "linear/Matrix.h"

namespace sylvanmats::alignment{

    enum SCORING_ALGORITHM{
        BLOSUM62
    };

    class SmithWatermanAligner{
        protected:
        SCORING_ALGORITHM scoringAlgorithm;
        public:
        SmithWatermanAligner() = delete;
        SmithWatermanAligner(SCORING_ALGORITHM scoringAlgorithm) : scoringAlgorithm (scoringAlgorithm) {};
        SmithWatermanAligner(const SmithWatermanAligner& org) = delete;
        virtual ~SmithWatermanAligner() = default;

        std::tuple<std::string, std::string> operator ()(std::string& sequenceA, std::string& sequenceB){
            int W1=2;
            sylvanmats::linear::MatrixXXi scoreMatrix(0, sequenceA.size()+1u, sequenceB.size()+1u);
            for(size_t i=0;i<scoreMatrix.cols();i++)scoreMatrix[0,i]=0;
            for(size_t i=0;i<scoreMatrix.rows();i++)scoreMatrix[i,0]=0;
            for(size_t j=1;j<scoreMatrix.cols();j++)
                for(size_t i=1;i<scoreMatrix.rows();i++){
                    scoreMatrix[i,j]=(sequenceA.at(i-1)==sequenceB.at(j-1))? 3: -3;
                }
            size_t iMax=1;
            size_t jMax=1;
            for(size_t j=1;j<scoreMatrix.cols();j++)
                for(size_t i=1;i<scoreMatrix.rows();i++){
                    int Hij=scoreMatrix[i-1,j-1]+scoreMatrix[i,j];
                    Hij=std::max(Hij, scoreMatrix[i-1,j]-W1);
                    Hij=std::max(Hij, scoreMatrix[i,j-1]-W1);
                    scoreMatrix[i,j]=std::max(Hij, 0);
                    if(scoreMatrix[i,j]>scoreMatrix[iMax,jMax]){
                        iMax=i;
                        jMax=j;
                    }
                }
            std::vector<std::tuple<size_t,size_t, char>> traceBack;
            //traceBack.push_back(std::make_tuple(iMax, jMax, ' '));
            size_t i=iMax;
            size_t j=jMax;
            while(i>1 && j>1){
                if(sequenceA.at(i-1)==sequenceB.at(j-1)){
                    i--;
                    j--;
                    traceBack.insert(traceBack.begin(), std::make_tuple(i, j, ' '));
                }
                else if(scoreMatrix[i-1,j]>scoreMatrix[i,j-1]){
                    i--;
                    traceBack.insert(traceBack.begin(), std::make_tuple(i, j, '-'));
                }
                else{
                    j--;
                    traceBack.insert(traceBack.begin(), std::make_tuple(i, j, '+'));
                }
                //std::cout<<" "<<scoreMatrix[i,j]<<" "<<sequenceA.substr(i, 1)<<" "<<sequenceB.substr(j, 1)<<std::endl;
            }
            std::string sequenceI;
            std::string sequenceJ;
            for(std::vector<std::tuple<size_t,size_t, char>>::iterator it=traceBack.begin();it!=traceBack.end();it++){
                if(std::get<2>(*it)=='+'){
                    sequenceI.append("-");
                    sequenceJ.append(sequenceB.substr(std::get<1>(*it), 1));
                }
                else if(std::get<2>(*it)=='-'){
                    sequenceI.append(sequenceA.substr(std::get<0>(*it), 1));
                    sequenceJ.append("-");
                }
                else{
                    sequenceI.append(sequenceA.substr(std::get<0>(*it), 1));
                    sequenceJ.append(sequenceB.substr(std::get<1>(*it), 1));
                }
            }
            return std::make_tuple(sequenceI, sequenceJ);
        };
    };
}
