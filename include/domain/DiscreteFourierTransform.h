#pragma once

#include "linear/Array.h"

namespace sylvanmats::domain{
    
    enum DIRECTION{
        FORWARD,
        INVERSE
    };
    template<typename T, typename F = T::value_type>
    class DiscreteFourierTransform{
    protected:
        DIRECTION direction=FORWARD;
    public:
        DiscreteFourierTransform() = delete;
        DiscreteFourierTransform(DIRECTION direction) : direction (direction) {};
        DiscreteFourierTransform(const DiscreteFourierTransform& orig) = delete;
        virtual ~DiscreteFourierTransform() = default;
        
        T operator()(const T& input){
            using namespace std::complex_literals;
            T output(input.size());
            size_t n = input.size();
            double c = (direction==FORWARD) ? -2.0: 2.0;
#pragma omp parallel for
            for (size_t k = 0; k < n; k++) {  // For each output element
                    F sum(0, 0);
                    for (size_t t = 0; t < n; t++) {  // For each input element
                            F arg(0.0, c * std::numbers::pi * t * k / n);
                            sum += input[t] * std::exp(arg);
                    }
                    output[k]=sum;
            }
            return output;
            
        };
    };
}
