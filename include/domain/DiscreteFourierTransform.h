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
        
        T operator()(const sylvanmats::linear::Array<std::complex<double>, -1, 1>& input){
            using namespace std::complex_literals;
            T output(input.rows(), input.cols());
            size_t n = input.size();
            double c = (direction==FORWARD) ? -2.0: 2.0;
//            std::cout<<"do 1 dft "<<n<<" "<<c<<std::endl;
#pragma omp parallel for
            for (size_t k = 0; k < n; k++) {  // For each output element
                    F sum(0, 0);
                    for (size_t t = 0; t < n; t++) {  // For each input element
                            F arg(0.0, c * std::numbers::pi * t * k / n);
                            sum += input[t] * std::exp(arg);
                    }
//                    sum/=(double)n;
                    output[k]=sum;
            }
            return output;
            
        };
        
        T operator()(const sylvanmats::linear::ArrayXXcd& input){
            using namespace std::complex_literals;
            T output(input.rows(), input.cols());
            size_t m = input.cols();
            size_t n = input.rows();
            double c = (direction==FORWARD) ? -2.0: 2.0;
//            std::cout<<"do 2 dft "<<m<<" "<<n<<" "<<c<<std::endl;
            int count=0;
            double max=0.0;
#pragma omp parallel for
            for (size_t l = 0; l < m; l++) {  // For each output column
            for (size_t k = 0; k < n; k++) {  // For each output row
                    F sum(0.0, 0.0);
                    count=0;
                    max=0.0;
                    for (size_t s = 0; s < m; s++) {  // For each input column
                    for (size_t t = 0; t < n; t++) {  // For each input row
                            F sarg(0.0, c * std::numbers::pi * s * l / (double)m);
                            F targ(0.0, c * std::numbers::pi * t * k / (double)n);
//                            F arg(0.0, c * std::numbers::pi * t * k / n);
                            sum += ((T&)input)[t, s] * std::exp(targ) * std::exp(sarg);
                    if(max>targ.imag())max=targ.imag();
                    if(((T&)input)[t, s].real()==1.0)count++;
                    }
                    }
//                    sum/=(double)(n*m);
                    ((T&)output)[k, l]=sum;
//                    if(max<output[k, l].real())max=output[k, l].real();
            }
            }
//            std::cout<<"did 2 dft "<<m<<" "<<n<<" count: "<<count<<" max: "<<max<<std::endl;
            return output;
            
        };
    };
}
