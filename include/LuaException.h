#pragma once

#include <string>
#include <exception>

namespace sylvanmats{
    class LuaException : public std::exception{

    protected:
        std::string explanatoryString;
        int error=0;
    public:
        LuaException(std::string explanatoryString) : explanatoryString (explanatoryString){};
        LuaException(const LuaException& orig) = delete;
        virtual ~LuaException() =  default;
        
        virtual const char* what(){return explanatoryString.c_str();};
    };
}
