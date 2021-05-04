#include "symmetry/LatticeSites.h"

#include "LuaException.h"

namespace sylvanmats::symmetry {

    LatticeSites::LatticeSites(std::vector<std::string>& functionNames, std::string& symmetryOperations, sylvanmats::linear::Matrix3d& opMatrix, sylvanmats::linear::Vector3d& opVector) : functionNames (functionNames), symmetryOperations (symmetryOperations), opMatrix (opMatrix), opVector (opVector) {
        int err=luaL_dostring (L, symmetryOperations.c_str());
        if(err!=LUA_OK) throw sylvanmats::LuaException("Lua dostring errored: "+std::to_string(err));
        err=lua_pcall(L, 0, 0, 0);
        //CHECK_EQ(err, LUA_ERRRUN);
        
    }; 
    
    void LatticeSites::operator()(sylvanmats::linear::Vector3d& pA, std::function<void(sylvanmats::linear::Vector3d& pAPrime)> apply){
        sylvanmats::linear::Vector3d pAPrime=pA;
        for(unsigned int symIndex=0;symIndex<functionNames.size();symIndex++){
            if(symIndex>=1){
                pAPrime=opMatrix.transpose()*(pA-opVector);//+opVector;
                //std::cout<<"pA "<<pA.transpose()<<" <--> "<<pB.transpose()<<" pB "<<std::endl;
                lua_getglobal(L, functionNames[symIndex].c_str());
                lua_pushnumber(L, pAPrime[0]);
                lua_pushnumber(L, pAPrime[1]);
                lua_pushnumber(L, pAPrime[2]);
                if (lua_pcall(L, 3, 3, 0) != LUA_OK) {
                        //printf("Error calling sym.f2");
                        lua_error(L);
                        return;
                }
                else{
                    pAPrime[0]=lua_tonumber(L, -3);
                    pAPrime[1]=lua_tonumber(L, -2);
                    pAPrime[2]=lua_tonumber(L, -1);
                    lua_pop(L, 3);
                }
                pAPrime=opMatrix.transpose().inverse()*(pAPrime);//+opVector;
                pAPrime+=opVector;
                apply(pAPrime);
            }
        }
    };

}
