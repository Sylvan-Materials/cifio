#pragma once

#include <type_traits>
#include "linear/Matrix.h"

namespace sylvanmats::linear{

    template<std::numerical T, signed long long R>
    class Vector : public Matrix<T, R, 1> {
    protected:
        using Matrix<T, R, 1>::_rows;
        using Matrix<T, R, 1>::_cols;
    public:
        using std::valarray<T>::operator[];
        using std::valarray<T>::operator+=;
        using std::valarray<T>::operator/=;
        using std::valarray<T>::sum;
        using Matrix<T, R, 1>::rows;
        using Matrix<T, R, 1>::cols;
        using Matrix<T, R, 1>::transpose;
        using Matrix<T, R, 1>::size;
    public:
        Vector() : Matrix<T, R, 1>(){};
        Vector(T a, T b) : Matrix<T, R, 1>(a, b){};
        Vector(T a, T b, T c) : Matrix<T, R, 1>({a, b, c}){};
        Vector(T* a) : Matrix<T, R, 1>(a) {};
        Vector(std::initializer_list<T> il) : Matrix<T, R, 1>(il) {};
        Vector(const Vector& orig) : Matrix<T, R, 1>(orig) {};
        virtual ~Vector() = default;
    public:
        T x(){return operator[](0);};
        T y(){return operator[](1);};
        T z(){return operator[](2);};

        Vector<T, R> pow(const unsigned int exp){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=std::pow(operator[](i), exp);
            return v2;
        };
        
        class Zero : public Vector<T, R>{
        public:
            Zero() : Vector<T, R>(){};
            Zero(T a, T b) : Vector<T, R>(a, b){};
        };

        class UnitX : public Vector<T, R>{
        public:
            UnitX(){operator[](0)=1.0;operator[](1)=0.0;if constexpr(R==3)operator[](2)=0.0;};
        };

        class UnitY : public Vector<T, R>{
        public:
            UnitY(){operator[](0)=0.0;operator[](1)=1.0;if constexpr(R==3)operator[](2)=0.0;};
        };

        class UnitZ : public Vector<T, R>{
        public:
            UnitZ(){operator[](0)=0.0;operator[](1)=0.0;if constexpr(R==3)operator[](2)=1.0;};
        };

        Vector<T, R> operator += (const Vector<T, R>& v){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)+=v[i];
            return *this;
        };

        Vector<T, R> operator /= (const Vector<T, R>& v){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)/=v[i];
            return *this;
        };

        Vector<T, R> operator /= (const typename Vector<T, R>::value_type& s){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)/=s;
            return *this;
        };

        Vector<T, R> operator + (){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i));
            return v2;
        };

        Vector<T, R> operator + (const typename Vector<T, R>::value_type& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(s+operator[](i));
            return v2;
        };

        Vector<T, R> operator + (const Vector<T, R>& v){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)+v[i]);
            return v2;
        };

        Vector<T, R> operator - (){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(-operator[](i));
            return v2;
        };

        Vector<T, R> operator - (const typename Vector<T, R>::value_type& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(s-operator[](i));
            return v2;
        };

        Vector<T, R> operator - (const Vector<T, R>& v){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)-v[i]);
            return v2;
        };

        Vector<T, R> operator * (const typename Vector<T, R>::value_type& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)*s;
            return v2;
        };

        Vector<T, R> operator * (const Vector<T, R>& v){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)*v[i];
            return v2;
        };

        Vector<T, R> operator / (const typename Vector<T, R>::value_type& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)/s;
            return v2;
        };

        Vector<T, R>  operator=(const std::valarray<T>& s){
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ operator[](i)=s[i];
            return *this;
        };
        
        Vector<T, R> operator + (const std::valarray<T>& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)+s[i]);
            return v2;
        };

        Vector<T, R> operator - (const std::valarray<T>& s){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=(operator[](i)-s[i]);
            return v2;
        };

        T dot(const Vector<T, R>& v){
            Vector<T, R> v2(*this);
            v2*=v;
            return v2.sum();
        };

        Vector<T, R> cross(const Vector<T, R>& v){
            Vector<T, R> v2(*this);
            
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
            Vector<T, R> v2(*this);
            T n = norm();
            *this/=n;
            //for(unsigned int i=0;i<R;i++)
                //v2[]
        }

        Vector<T, R> normalized(){
            Vector<T, R> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i);
            T n = norm();
            v2/=n;
            //for(unsigned int i=0;i<R;i++)
                //v2[]
            return v2;
        }
        
        Vector<T, R> row(unsigned int s){
            Vector<T, R> v2(*this[std::slice(s, _cols, _rows)]);
            return v2;
        }
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator + (const typename Vector<T, R>::value_type& s, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s+v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator + (const Vector<T, R>& lv, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]+v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator - (const typename Vector<T, R>::value_type& s, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s-v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator - (const Vector<T, R>& lv, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]-v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator * (const typename Vector<T, R>::value_type& s, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s*v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator * (const Vector<T, R>& lv, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]*v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator / (const typename Vector<T, R>::value_type& s, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(s/v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator / (const Vector<T, R>& v, const typename Vector<T, R>::value_type& s){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(v[i]/s);
        return v2;
    };

    template<std::numerical T, signed long long R>
    Vector<T, R> operator / (const Vector<T, R>& lv, const Vector<T, R>& v){
        Vector<T, R> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]/v[i]);
        return v2;
    };

    using Vector2i = Vector<int, 2>;
    using Vector2d = Vector<double, 2>;
    using Vector2l = Vector<long double, 2>;

    using Vector3i = Vector<int, 3>;
    using Vector3d = Vector<double, 3>;
    using Vector3l = Vector<long double, 3>;

}

