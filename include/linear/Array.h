#pragma once

#include <type_traits>
#include "linear/Matrix.h"

namespace sylvanmats::linear{

    template<std::numerical T, signed long long R, signed long long C=1>
    class Array : public Matrix<T, R, C> {
    protected:
        using Matrix<T, R, C>::_rows;
        using Matrix<T, R, C>::_cols;
    public:
        using std::valarray<T>::operator[];
        using std::valarray<T>::operator+=;
        using std::valarray<T>::operator/=;
        using std::valarray<T>::sum;
        using Matrix<T, R, C>::rows;
        using Matrix<T, R, C>::cols;
        using Matrix<T, R, C>::transpose;
        using Matrix<T, R, C>::size;
    public:
        Array() : Matrix<T, R, C>() {};
        Array(unsigned int s) :Matrix<T, R, C>((T)0.0, s){};
        Array(unsigned int s, unsigned int t) :Matrix<T, R, C>((T)0.0, s, t){};
        Array(T* a) : Matrix<T, R, C>(a) {};
        Array(T* a, unsigned int s) :Matrix<T, R, 1>(a, s) {};
        Array(T* a, unsigned int s, unsigned int t) :Matrix<T, R, C>(a, s, t) {};
        Array(std::initializer_list<T> il) : Matrix<T, R, C>(il) {};
        Array(const Array& orig) = default;
        virtual ~Array() = default;
    public:
        
        Array<T, R, C> pow(const unsigned int exp){
                Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::pow(operator[](i), exp);
            return v2;
        };

        Array<T, R, C> sqrt(){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::sqrt(operator[](i));
            return v2;
        };

        Array<T, R, C> cos(){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::cos(operator[](i));
            return v2;
        };

        Array<T, R, C> sin(){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::sin(operator[](i));
            return v2;
        };

        Array<T, R, C> atan(){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::atan(operator[](i));
            return v2;
        };

        class Zero : public Array<T, R, C>{
        public:
                Zero(){};
                Zero(unsigned int s) : Array<T, R, C>(s) {};
                Zero(unsigned int s, unsigned int t) : Array<T, R, C>(s, t) {};
        };

        /*class UnitX : public Array<T, 3>{
        public:
            UnitX(){operator[](0)=1.0;operator[](1)=0.0;operator[](2)=0.0;};
        };

        class UnitY : public Array<T, 3>{
        public:
            UnitY(){operator[](0)=0.0;operator[](1)=1.0;operator[](2)=0.0;};
        };

        class UnitZ : public Array<T, 3>{
        public:
            UnitZ(){operator[](0)=0.0;operator[](1)=0.0;operator[](2)=1.0;};
        };*/

        Array<T, R, C> operator += (const Array<T, R, C>& v){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)+=v[i];
            return *this;
        };

        Array<T, R, C> operator /= (const Array<T, R, C>& v){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)/=v[i];
            return *this;
        };

        Array<T, R, C> operator /= (const typename Array<T, R, C>::value_type& s){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)/=s;
            return *this;
        };

        Array<T, R> operator + (){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i));
            return v2;
        };

        Array<T, R, C> operator + (const typename Array<T, R, C>::value_type& s){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(s+operator[](i));
            return v2;
        };

        Array<T, R, C> operator + (const Array<T, R, C>& v){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)+v[i]);
            return v2;
        };

        Array<T, R, C> operator - (){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(-operator[](i));
            return v2;
        };

        Array<T, R, C> operator - (const typename Array<T, R, C>::value_type& s){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(s-operator[](i));
            return v2;
        };

        Array<T, R, C> operator - (const Array<T, R, C>& v){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)-v[i]);
            return v2;
        };

        Array<T, R, C> operator * (const typename Array<T, R, C>::value_type& s){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)*s;
            return v2;
        };

        Array<T, R, C> operator * (const Array<T, R, C>& v){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)*v[i];
            return v2;
        };

        Array<T, R, C> operator / (const typename Array<T, R, C>::value_type& s){
            Array<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)/s;
            return v2;
        };

        T dot(const Array<T, R, C>& v){
            Array<T, R, C> v2(*this);
            v2*=v;
            return v2.sum();
        };

        Array<T, R, C> cross(const Array<T, R, C>& v){
            Array<T, R, C> v2(*this);
            
            v2[0] = operator[](1) * v[2] - operator[](2) * v[1] ;
            v2[1] = operator[](2) * v[0] - operator[](0) * v[2] ;
            v2[2] = operator[](0) * v[1] - operator[](1) * v[0] ;

            return v2;
        };

        T norm(){
            T sum=0.0;
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ sum+=std::pow(operator[](i), 2);
            return std::sqrt(sum);
        };

        void normalize(){
            Array<T, R> v2(*this);
            T n = norm();
            *this/=n;
            //for(unsigned int i=0;i<R;i++)
                //v2[]
        }

        Array<T, R, C> normalized(){
            Array<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i);
            T n = norm();
            v2/=n;
            //for(unsigned int i=0;i<R;i++)
                //v2[]
            return v2;
        }
        
//        Array<T, R, C> row(unsigned int s){
//            Array<T, R, C> v2((*this)[std::slice(s, _cols, _rows)]);
//            return v2;
//        }
    };

    using Array2d = Array<double, 2>;
    using Array2l = Array<long double, 2>;

    using Array3d = Array<double, 3>;
    using Array3l = Array<long double, 3>;

    using ArrayXi = Array<int, -1>;
    using ArrayXd = Array<double, -1>;
    using ArrayXl = Array<long double, -1>;

    using ArrayX3d = Array<double, -1, 3>;
    
    using ArrayXXi = Array<int, -1, -1>;
    using ArrayXXd = Array<double, -1, -1>;
    using ArrayXXl = Array<long double, -1, -1>;

    template<std::numerical T, signed long long R, signed long long C=1>
    Array<T, R, C> operator + (const typename Array<T, R, C>::value_type& s, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s+v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator + (const Array<T, R, C>& lv, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]+v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
        Array<T, R, C> operator - (const typename Array<T, R>::value_type& s, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s-v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
        Array<T, R, C> operator - (const Array<T, R, C>& lv, const Array<T, R>& v){
        Array<T, R> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]-v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator * (const typename Array<T, R>::value_type& s, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s*v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator * (const Array<T, R, C>& lv, const Array<T, R, C>& v){
        Array<T, R> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]*v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator / (const typename Array<T, R>::value_type& s, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s/v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator / (const Array<T, R, C>& v, const typename Array<T, R>::value_type& s){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(v[i]/s);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Array<T, R, C> operator / (const Array<T, R, C>& lv, const Array<T, R, C>& v){
        Array<T, R, C> v2(v);
        for(unsigned long long i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]/v[i]);
        return v2;
    };

}


