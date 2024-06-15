#pragma once

#include <iostream>
#include <memory>
#include <iomanip>
#include <utility>
#include <valarray>
#include <complex>
#include <utility>
#include <concepts>
#include <type_traits>

#include "linear/Matrix.h"

namespace sylvanmats::linear{
    
    template<std::numerical T, typename F = T::value_type, std::convertible_to<std::size_t> auto  ...R>
    class Tensor : public std::valarray<T> {
    public:
        using std::valarray<T>::operator[];
        protected:
            const size_t D;
            std::unique_ptr<size_t[]> dims;
            std::vector<size_t> v;
    public:
        Tensor() = delete;
        Tensor(std::convertible_to<std::size_t> auto ...i) : std::valarray<T>(0.0, (i * ...)), D (sizeof...(i)), dims (new size_t[D]) {
            (v.push_back(i), ...);
            for(size_t index=0;index<D;index++){dims[index]=v[index];};
        };
        Tensor(T a, std::convertible_to<std::size_t> auto ...i) : std::valarray<T>(a, (i * ...)), D (sizeof...(i)), dims (new size_t[D]) {
            (v.push_back(i), ...);
            for(size_t index=0;index<D;index++){dims[index]=v[index];};
        };
        Tensor(size_t rank, std::unique_ptr<size_t[]>& ds) : std::valarray<T>(0.0, init(rank, ds)), D (rank), dims (new size_t[D]) {
            for(size_t index=0;index<D;index++){dims[index]=ds[index];};
        };
        Tensor(const Tensor& orig) = delete;
        Tensor(Tensor&& other) = default;
        virtual ~Tensor() = default;
        Tensor& operator=(const Tensor& other) = delete;
        Tensor& operator=(Tensor&& other) = delete;
    
    private:
        inline size_t init(const size_t D, std::unique_ptr<size_t[]>& dims){
            size_t t=1;
            for(size_t index=0;index<D;index++){t*=dims[index];};
            return t;
        };
//        inline std::unique_ptr<size_t[]> init(const size_t D, std::vector<size_t>& v, auto&&...i){
//            std::unique_ptr<size_t[]> dims(new size_t[D]);
//            
//            (v.push_back(i), ...);
//            for(size_t index=0;index<D;index++){dims[index]=v[index];};
//            return std::move(dims);
//        };
        size_t position=-1;
    public:
        void reset(){position=-1;};
        T& next(){position++; return std::valarray<T>::operator[](position); };
        T& next(size_t position){ return std::valarray<T>::operator[](position); };
         T& operator[](std::convertible_to<std::size_t> auto ... indices){
             const size_t size = sizeof...(indices);
             size_t res[size] = {indices...};
             size_t t = res[0];
             size_t s = (size>=2)? res[1] : 0;
             size_t r = (size>=3)? res[2] : 0;
//             if(D==3 && s==0 && t==0)std::cout<<"t "<<t<<" s "<<s<<" "<<((std::valarray<T>&)(*this))[t+s*dims[0]]<<std::endl;
//             if(D==1)return std::valarray<T>::operator[](t);
//             else if(D==2)return std::valarray<T>::operator[](t+s*dims[0]);
             return std::valarray<T>::operator[](t+s*dims[0]+r*dims[0]*dims[1]);
         };
         size_t getRank()const{return D;};
         size_t getDimension(size_t i) const {return dims[i];};
        
    };
    
//    using Tensor3d = Tensor<double, 3, 3, 3>;
//    using Tensor3l = Tensor<long double, 3, 3, 3>;
}
