#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <valarray>
#include <cmath>

#include "linear/Matrix.h"

namespace sylvanmats::statistics{
    
    template<std::numerical T>
    struct min{
        T v;
        bool firstTime=true;
        void operator()(T value){
            if(firstTime){
                v=value;
            }
            else if(v>value){
                v=value;
            }
            firstTime=false;
        };
        T operator()(){return v;};
    };
    
    template<std::numerical T>
    struct max{
        T v;
        bool firstTime=true;
        void operator()(T value){
            if(firstTime){
                v=value;
            }
            else if(v<value){
                v=value;
            }
            firstTime=false;
        };
        T operator()(){return v;};
    };
    
    template<std::numerical T>
    struct mean{
        T v=0.0;
        size_t counter=0;
        void operator()(T value){
            v+=value;
            counter++;
        };
        T operator()(){return (v/((T)counter));};
    };
    
    template<std::numerical T>
    struct median{
        std::vector<T> v;
        void operator()(T value){
            bool hit=false;
            for(typename std::vector<T>::reverse_iterator it=v.rbegin();!hit && it!=v.rend();it++){
                if(value>(*it)){
                    v.insert(it.base(), value);
                    hit=true;
                }
            }
            if(!hit) v.push_back(value);
        };
        std::tuple<bool, T, T> operator()(){return std::make_tuple(v.size() % 2 == 1, v[v.size()/2], v[v.size()/2+1]);};
    };
    
    template<std::numerical T>
    struct mode{
        std::unordered_multimap<T, size_t> m;
        void operator()(T value){
            m.insert(m.end(), std::make_pair(value, m.size()));
        };
        std::vector<size_t> operator()(){
            std::vector<size_t> v;
            size_t bucketIndex=0;
            size_t bucketSize=0;
            for(size_t i=0;i<m.bucket_count();i++){
                if(i==0)bucketSize=m.bucket_size(i);
                else if(bucketSize<m.bucket_size(i)){
                    bucketSize=m.bucket_size(i);
                    bucketIndex=i;
                }
            }
            for(typename std::unordered_multimap<T, size_t>::local_iterator it=m.begin(bucketIndex);it!=m.end(bucketIndex);it++){
                v.push_back((*it).second);
            }
            return v;
        };
    };
    
    template<std::numerical T>
    struct population_stddev{
        std::valarray<T> v;
        void operator()(T value){
            v.resize(v.size()+1);
            v[v.size()-1]=value;
        };
        T operator()(){
            T μ=v.sum()/v.size();
            return std::sqrt(std::pow((v-μ),2).sum()/v.size());
        };
    };
    
    template<std::numerical T>
    struct sample_stddev{
        std::valarray<T> v;
        void operator()(T value){
            v.resize(v.size()+1);
            v[v.size()-1]=value;
        };
        T operator()(){
            T μ=v.sum()/v.size();
            return std::sqrt(std::pow((v-μ),2).sum()/(v.size()-1.0));
        };
    };
    
    template<std::numerical T, typename R = T::value_type,  typename  ...F>//, std::enable_if_t<std::is_invocable_v<(func...), void(*)>, bool> = true>
    class Accumulator {
        protected:
            size_t kept;
            std::tuple<F& ...> f;
            size_t counter;
        public:
        Accumulator() = delete;
        Accumulator(size_t kept, F& ...f) : kept (kept), f (f...), counter (0) {
            
        };
        Accumulator(const Accumulator& orig) = delete;
        virtual ~Accumulator() = default;
        
        void operator()(T value){
            std::apply([&](auto &... args) {
                 (args(value), ...);
            }, f);
//            for... (const auto &elem : f)
//                elem(value);
            counter++;
        };
    };
}
