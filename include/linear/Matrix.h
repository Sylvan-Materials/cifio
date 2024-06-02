#pragma once

#include <iostream>
#include <iomanip>
#include <utility>
#include <valarray>
#include <complex>
#include <concepts>
#include <type_traits>

namespace std{
    template <typename T, typename F = T::value_type>
    concept Complex = (std::is_floating_point_v<F> &&
                   std::is_same_v<std::remove_cv_t<T>, std::complex<F>>);

    template<typename T> 
    concept numerical = std::integral<T> || std::floating_point<T> || Complex<T>;
}

namespace sylvanmats::linear{

    template<std::numerical U, signed long long V, signed long long W> class Array;
    template<std::numerical U, signed long long V> class Vector;

    template<std::numerical T, signed long long R, signed long long C>
    class Matrix : public std::valarray<T> {
//    template<std::numerical, signed long long, signed long long> friend class Array;
//    template<std::numerical, signed long long> friend class Vector;

    public:
        using std::valarray<T>::operator[];
        protected:
        unsigned long long _rows = R;
        unsigned long long _cols = C;
    public:
        Matrix() : std::valarray<T>(0.0, R * C) {};
        Matrix(T a, unsigned int s) : std::valarray<T>(a, s), _rows (s), _cols (1) {};
        Matrix(T a, unsigned int s, unsigned int t) : std::valarray<T>(a, s*t), _rows (s), _cols (t) {};
        Matrix(T a, T b) : std::valarray<T>(0.0, R * C) {operator[](0)=a;operator[](1)=b;};
        Matrix(T a, T b, T c) : std::valarray<T>(0.0, R * C) {operator[](0)=a;operator[](1)=b;operator[](2)=c;};
        Matrix(T* a, unsigned int s) : std::valarray<T>(s), _rows (s), _cols (1) {
            for(unsigned int i=0;i<s;i++)
                    /*co_yield*/ operator[](i)=a[i];
        };
        Matrix(T* a) : std::valarray<T>(R * C) {
            for(unsigned long long j=0;j<_cols;j++)
            for(unsigned long long i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i+_rows*j)=a[i+_rows*j];
        };
        Matrix(std::initializer_list<T> il) : std::valarray<T>(il) {};
        Matrix(const Matrix& orig) : std::valarray<T>(orig) {_rows=orig._rows;_cols=orig._cols;};
        virtual ~Matrix() = default;
    public:
        class Identity : public Matrix<T, R, C>{
        public:
            Identity() : Matrix<T, R, C>() {
            for(unsigned long long i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i+_rows*i)=1.0;
            };
        };

        Matrix<T, C, R> transpose(){
            Matrix<T, C, R> m;
            for(unsigned long long j=0;j<_cols;j++)
            for(unsigned long long i=0;i<_rows;i++)
                    /*co_yield*/ m[j+C*i]=operator[](i+_rows*j);
                
            return m;
        };

        T det(){
            return  operator[](0+0*R) * (operator[](1+1*_rows) * operator[](2+2*_rows) - operator[](2+1*_rows) * operator[](1+2*_rows)) -
                    operator[](0+1*R) * (operator[](1+0*_rows) * operator[](2+2*_rows) - operator[](1+2*_rows) * operator[](2+0*_rows)) +
                    operator[](0+2*R) * (operator[](1+0*_rows) * operator[](2+1*_rows) - operator[](1+1*_rows) * operator[](2+0*_rows));
        };

        Matrix<T, C, R> inverse(){
            Matrix<T, C, R> m(*this);
            T d = det();

            m[0+0*_rows] = (operator[](1+1*_rows) * operator[](2+2*_rows) - operator[](2+1*_rows) * operator[](1+2*_rows)) / d;
            m[0+1*_rows] = (operator[](0+2*_rows) * operator[](2+1*_rows) - operator[](0+1*_rows) * operator[](2+2*_rows)) / d;
            m[0+2*_rows] = (operator[](0+1*_rows) * operator[](1+2*_rows) - operator[](0+2*_rows) * operator[](1+1*_rows)) / d;
            m[1+0*_rows] = (operator[](1+2*_rows) * operator[](2+0*_rows) - operator[](1+0*_rows) * operator[](2+2*_rows)) / d;
            m[1+1*_rows] = (operator[](0+0*_rows) * operator[](2+2*_rows) - operator[](0+2*_rows) * operator[](2+0*_rows)) / d;
            m[1+2*_rows] = (operator[](1+0*_rows) * operator[](0+2*_rows) - operator[](0+0*_rows) * operator[](1+2*_rows)) / d;
            m[2+0*_rows] = (operator[](1+0*_rows) * operator[](2+1*_rows) - operator[](2+0*_rows) * operator[](1+1*_rows)) / d;
            m[2+1*_rows] = (operator[](2+0*_rows) * operator[](0+1*_rows) - operator[](0+0*_rows) * operator[](2+1*_rows)) / d;
            m[2+2*_rows] = (operator[](0+0*_rows) * operator[](1+1*_rows) - operator[](1+0*_rows) * operator[](0+1*_rows)) / d;                
            return m;
        };

        std::valarray<T> row(unsigned int s){
            return (*this)[std::slice(s, _cols, _rows)];
        }
        
        std::slice_array<T> block(unsigned int s, unsigned int t, unsigned int l, unsigned int m){
            return (*this)[std::slice(s, _cols, _rows)];
        }
        
        template<std::numerical U = T, signed long long V = R> friend class Vector<U, V> operator * (const Matrix<T, R, C>& m, const Vector<U, V>& v){
            Vector<T, R> v2(v);
            for(unsigned long long i=0;i<m.cols();i++)
                    /*co_yield*/ v2[i]=0.0;
            for(unsigned long long j=0;j<m.cols();j++)
            for(unsigned long long i=0;i<m.rows();i++)
                    /*co_yield*/ v2[j]+=(m[i+j*m.rows()]*v[i]);
                    //v2[i]=(m[std::slice(i, C, C)]*v[i]).sum();
            return v2;
        };

        unsigned long long rows() const {return _rows;};
        unsigned long long cols() const {return _cols;};
        unsigned long long size() const {return _rows*_cols;};
//         T& operator[](std::convertible_to<std::size_t> auto index){
//             return (*this)[index];
//         }
         T& operator[](std::convertible_to<std::size_t> auto ... indices){
             const size_t size = sizeof...(indices);
             size_t res[size] = {indices...};
             size_t t = res[0];
             size_t s = (size>=2)? res[1] : 0;
//             std::cout<<"t "<<t<<" s "<<s<<" "<<((std::valarray<T>&)(*this))[t+s*_rows]<<std::endl;
//             if(_cols<=1)return ((std::valarray<T>&)(*this))[t];
//             else if(_cols>=1)return ((std::valarray<T>&)(*this))[s+t*_cols];
             return std::valarray<T>::operator[](t+s*_rows);
         }
        
        friend std::ostream& operator<<(std::ostream& s, const Matrix<T, R, C>& r) {
            for(unsigned long long j=0;j<r.cols();j++){
                for(unsigned long long i=0;i<r.rows();i++){
                    s <<std::setw(12)<< r[i+j*r.rows()];
                }
                s<<std::endl;
            }
          return s;
        }
    };

    using Matrix3d = Matrix<double, 3, 3>;
    using Matrix3l = Matrix<long double, 3, 3>;
}
