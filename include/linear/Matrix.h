#pragma once

#include <iostream>
#include <iomanip>
#include <utility>
#include <valarray>
#include <type_traits>

namespace sylvanmats::linear{

    template<std::floating_point U, signed long long V, signed long long W> class Array;
    template<std::floating_point U, signed long long V> class Vector;

    template<std::floating_point T, signed long long R, signed long long C>
    class Matrix : public std::valarray<T> {
    template<std::floating_point, signed long long, signed long long> friend class Array;
    template<std::floating_point, signed long long> friend class Vector;

    public:
        using std::valarray<T>::operator[];
        protected:
        unsigned long long _rows = R;
        unsigned long long _cols = C;
    public:
        Matrix() : std::valarray<T>(0.0, R * C) {};
        Matrix(T a, unsigned int s) : std::valarray<T>(a, s), _rows (s), _cols (1) {};
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

        template<std::floating_point U = T, signed long long V = R> friend class Vector<U, V> operator * (const Matrix<T, R, C>& m, const Vector<U, V>& v){
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

    typedef Matrix<double, 3, 3> Matrix3d;
    typedef Matrix<long double, 3, 3> Matrix3l;
}
