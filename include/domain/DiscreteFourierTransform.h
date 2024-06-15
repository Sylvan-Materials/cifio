#pragma once

#include "linear/Array.h"
#include <omp.h>

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
        
        //1-d dft
        T operator()(const sylvanmats::linear::ArrayXcd& input){
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
        
        //2-d dft
        T operator()(const sylvanmats::linear::ArrayXXcd& input){
            using namespace std::complex_literals;
            T output(input.rows(), input.cols());
            size_t m = input.cols();
            size_t n = input.rows();
            double c = (direction==FORWARD) ? -2.0: 2.0;
//            std::cout<<"do 2 dft "<<m<<" "<<n<<" "<<c<<std::endl;
            int count=0;
            double max=0.0;
#pragma omp parallel for shared(m, n, c, output)
            for (size_t l = 0; l < m; l++) {  // For each output column
            for (size_t k = 0; k < n; k++) {  // For each output row
                    F sum(0.0, 0.0);
//                    count=0;
//                    max=0.0;
                    for (size_t s = 0; s < m; s++) {  // For each input column
                            F sarg(0.0, c * std::numbers::pi * s * l / (double)m);
                    for (size_t t = 0; t < n; t++) {  // For each input row
                            F targ(0.0, c * std::numbers::pi * t * k / (double)n);
                            sum += ((T&)input)[t, s] * std::exp(targ) * std::exp(sarg);
//                    if(max>targ.imag())max=targ.imag();
//                    if(((T&)input)[t, s].real()==1.0)count++;
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
        
        //3-d dft
        T operator()(const sylvanmats::linear::Tensor<F>& input){
            using namespace std::complex_literals;
            std::unique_ptr<size_t[]> dims(new size_t[input.getRank()]);
            for(size_t index=0;index<input.getRank();index++){dims[index]=input.getDimension(index);};
            T output(input.getRank(), dims);
            size_t l = input.getDimension(2);
            size_t m = input.getDimension(1);
            size_t n = input.getDimension(0);
            double c = (direction==FORWARD) ? -2.0: 2.0;
            std::cout<<"do 3 dft "<<l<<" "<<m<<" "<<n<<" "<<c<<std::endl;
            int count=0;
            double max=0.0;
#pragma omp parallel for num_threads(omp_get_max_threads()) shared(l, m, n, c, output) // schedule(static, 8) reduction( +: l[0:4] )
            for (size_t k = 0; k < l; k++) {  // For each output slice
            for (size_t j = 0; j < m; j++) {  // For each output column
            for (size_t i = 0; i < n; i++) {  // For each output row
                    F sum(0.0, 0.0);
//                    count=0;
//                    max=0.0;
//                    ((T&)input).reset();
//            size_t position=0;
                    for (size_t r = 0; r < l; r++) {  // For each input slice
                            F rarg(0.0, c * std::numbers::pi * r * k / (double)l);
                    for (size_t s = 0; s < m; s++) {  // For each input column
                            F sarg(0.0, c * std::numbers::pi * s * j / (double)m);
                    for (size_t t = 0; t < n; t++) {  // For each input row
                            F targ(0.0, c * std::numbers::pi * t * i / (double)n);
                            sum += ((T&)input)[t, s, r] * std::exp(targ) * std::exp(sarg) * std::exp(rarg);
//                    if(max<targ.imag())max=targ.imag();
//                    if(((T&)input)[t, s, r].real()==1.0)count++;
                    }
//                    position+=n;
                    }
                    }
//                    sum/=(double)(n*m);
#pragma omp critical                    
                    ((T&)output)[i, j, k]=sum;
//                    if(max<output[k, l].real())max=output[k, l].real();
//            std::cout<<" i "<<i;
            }
//            std::cout<<" j "<<j;
            }
            std::cout<<" k "<<k<<std::endl;
            }
            std::cout<<"did 3 dft "<<l<<" "<<m<<" "<<n<<" count: "<<count<<" max: "<<max<<std::endl;
            return output;
        };
    };
}
