#pragma once

#include <type_traits>
#include "linear/Matrix.h"

namespace sylvanmats::linear{

    template<std::numerical U, signed long long V, signed long long W> class Array;

    template<std::numerical T, signed long long R, signed long long C=1>
    class Vector : public Matrix<T, R, C> {
    protected:
        using Matrix<T, R, C>::_rows;
        using Matrix<T, R, C>::_cols;
    public:
        using std::valarray<T>::operator[];
        using std::valarray<T>::operator+=;
        using std::valarray<T>::operator/=;
        using std::valarray<T>::operator*=;
        using std::valarray<T>::sum;
        using Matrix<T, R, C>::rows;
        using Matrix<T, R, C>::cols;
        //using Matrix<T, R, C>::transpose;
        using Matrix<T, R, C>::size;
        using Matrix<T, R, C>::operator*;
        //using Matrix<T, R, C>::operator/;
    public:
        Vector() : Matrix<T, R, 1l>(){};
        Vector(T a, T b) : Matrix<T, R, C>({a, b}){};
        Vector(T a, T b, T c) : Matrix<T, R, C>({a, b, c}){};
        Vector(T a, signed long long s, signed long long t) requires(std::is_same_v<T, double> ): Matrix<T, R, C>(a, s, t) {};
        Vector(T a, signed long long s, signed long long t) requires(std::is_same_v<T, long double>): Matrix<T, R, C>(a, s, t) {};
        Vector(T* a) : Matrix<T, R, 1>(a) {};
        Vector(std::initializer_list<T> il) : Matrix<T, R, 1>(il) {};
        Vector(const Vector& orig) = default;//: Matrix<T, R, C>(orig) {};
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

        /*Vector<T, R, C> operator * (const std::valarray<T>& v){
            std::cout<<"Vector::* va "<<" "<<_rows<<" "<<_cols<<" "<<v.size()<<std::endl;
            return *this;
        };*/

        template<std::numerical U = T, signed long long V = R, signed long long W = C>  Vector<U, V, W> operator * (const Vector<T, R, C>& v) const {
            std::cout<<"Vector::* "<<" "<<_rows<<" "<<_cols<<" "<<v.rows()<<" "<<v.cols()<<std::endl;
            if(_rows==v.rows() && _cols==v.cols()){
            Vector<T, R, C> v2(*this);
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i)*v[i];
            return v2;
            }
            if(_rows>v.rows()){
            Vector<T, R, C> v2(0.0l, _rows, v.cols());
            std::cout<<"Vector::* "<<this->rows()<<" "<<v.cols()<<std::endl;
            for(unsigned int j=0;j<v.cols();j++)
            for(unsigned int i=0;i<_rows;i++)
                    /*co_yield*/ v2[i+j*_rows]+=operator[](i)*v[j];
            return v2;
            }
            Vector<T, R,C> v2(0.0l, _cols, v.rows());
            std::cout<<"Vector::* "<<_cols<<" "<<v.rows()<<std::endl;
            for(unsigned int j=0;j<v.rows();j++)
            for(unsigned int i=0;i<_cols;i++)
                    /*co_yield*/ v2[i+j*_cols]+=operator[](i)*v[j];
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

        Vector<T, C, R> transpose(){
            //std::cout<<"Vector::transpose "<<" "<<this->rows()<<" "<<this->cols()<<std::endl;
            Vector<T, C, R> v2(0.0l, this->cols(), this->rows());
            //std::swap(v2._rows, v2._cols);
            //std::cout<<"\t"<<v2.rows()<<" "<<v2.cols()<<std::endl;
            for(unsigned long long j=0;j<_cols;j++)
            for(unsigned long long i=0;i<_rows;i++)
                    /*co_yield*/ v2[j+_cols*i]=operator[](i+_rows*j);
            //std::cout<<"\t\t"<<v2.rows()<<" "<<v2.cols()<<std::endl;
            return v2;
        };
 
        template<std::numerical U = T, signed long long V = R, signed long long W = C>
        Array<U, V, W> array(){
           Array<U, V, W> v2(this->rows(), this->cols());
            for(unsigned long long i=0;i<_rows;i++)
                    /*co_yield*/ v2[i]=operator[](i);
           return v2;
        };

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

    template<std::numerical T, signed long long R, signed long long C>
    Vector<T, R> operator * (const typename Vector<T, R, C>::value_type& s, const Vector<T, R, C>& v){
        Vector<T, R, C> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                v2[i]=(s*v[i]);
        return v2;
    };

    template<std::numerical T, signed long long R, signed long long C>
    Vector<T, R, C> operator * (const Vector<T, R, C>& lv, const Vector<T, R, C>& v){
            std::cout<<"Vector::* "<<" "<<lv.rows()<<" "<<v.rows()<<" "<<v.rows()<<" "<<v.cols()<<std::endl;
            if(lv.rows()==v.rows() && lv.cols()==v.cols()){
        Vector<T, R, C> v2(lv);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]*v[i]);
        return v2;
            }
            if(lv.rows()>v.rows()){
            Vector<T, R, C> v2(0.0l, lv.rows(), v.cols());
            std::cout<<"Vector::* "<<lv.rows()<<" "<<v.cols()<<std::endl;
            for(unsigned int j=0;j<v.cols();j++)
            for(unsigned int i=0;i<lv.rows();i++)
                   /*co_yield*/ v2[i+j*lv.rows()]+=lv[i]*v[j];
            return v2;
            }
            Vector<T, R, C> v2(0.0l, lv.cols(), v.rows());
            std::cout<<"Vector::* "<<lv.cols()<<" "<<v.rows()<<std::endl;
            for(unsigned int j=0;j<v.rows();j++)
            for(unsigned int i=0;i<lv.cos();i++)
                   /*co_yield*/ v2[i+j*lv.cols()]+=lv[i]*v[j];
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

    template<std::numerical T, signed long long R, signed long long C>
    Vector<T, R, C> operator / (const Vector<T, R, C>& lv, const Vector<T, R, C>& v){
        std::cout<<"vec / "<<lv.rows()<<" "<<lv.cols()<<std::endl;
        Vector<T, R, C> v2(v);
        for(unsigned int i=0;i<v.rows();i++)
                /*co_yield*/ v2[i]=(lv[i]/v[i]);
        return v2;
    };

    using Vector2i = Vector<int, 2, 1>;
    using Vector2d = Vector<double, 2, 1>;
    using Vector2l = Vector<long double, 2, 1>;

    using Vector3i = Vector<int, 3, 1>;
    using Vector3d = Vector<double, 3, 1>;
    using Vector3l = Vector<long double, 3, 1>;

}

