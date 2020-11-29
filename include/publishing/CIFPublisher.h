#pragma once

#include <jni.h>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <cstdarg>

namespace sylvanmats::publishing{
    class CIFPublisher{
    protected:
        JavaVM *javaVM;
        JNIEnv *jniEnv;
        jclass jcls;
        jclass jshortcls;
        jclass jintcls;
        jclass jlongcls;
        jclass jdoublecls;
        jmethodID constructorShortId;
        jmethodID constructorIntId;
        jmethodID constructorLongId;
        jmethodID constructorDoubleId;
        jobject stObject;

        std::unordered_map<short, std::string> tupleMap = {{2, "Pair"}, {3, "Triplet"},{4, "Quartet"},{5, "Quintet"},{6, "Sextet"},{7, "Septet"},{8, "Octet"},{9, "Ennead"},{10, "Decade"},{11, "Hendecad"},{21, "TwentyOne"}};
    public:
        CIFPublisher();
        CIFPublisher(const CIFPublisher& orig) = delete;
        virtual ~CIFPublisher();

        void add(std::string name, std::string value);

        template< class... Types >
        void add(std::string name, std::vector<std::tuple<Types...>>& value){
            jclass jalcls = jniEnv->FindClass("java/util/ArrayList");
            jmethodID constructorId = jniEnv->GetMethodID(jalcls, "<init>", "()V");
            jobject alObject = jniEnv->NewObject(jalcls, constructorId);
            jmethodID methodALId = jniEnv->GetMethodID(jalcls,
             "add", "(Ljava/lang/Object;)Z");
            std::string className="org/javatuples/"+tupleMap[std::tuple_size<std::tuple<Types...>>::value];
            jclass jtcls = jniEnv->FindClass(className.c_str());
                if (jtcls == NULL) {
                   jniEnv->ExceptionDescribe();
                   jniEnv->ExceptionClear();
                }
            std::cout<<"jtcls "<<jtcls<<" "<<std::endl;
            std::string sigList="(";
            for(unsigned int index=0;index<std::tuple_size<std::tuple<Types...>>::value;index++){
                sigList.append("Ljava/lang/Object;");
            }
            sigList.append(")V");
            size_t columns = std::tuple_size<std::tuple<Types...>>::value;
//            std::cout<<(columns)<<" className "<<className<<" "<<sigList<<std::endl;
            jmethodID constructorTId = jniEnv->GetMethodID(jtcls, "<init>", sigList.c_str());
                if (constructorTId == NULL) {
                   jniEnv->ExceptionDescribe();
                   jniEnv->ExceptionClear();
                }
            
            for(std::tuple<Types...> row : value){
                jobject tObject = NULL;
                if constexpr(std::tuple_size<std::tuple<Types...>>::value==3){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==4){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==5){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==6){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==7){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==8){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)), toArgs(std::get<7>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==9){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)), toArgs(std::get<7>(row)), toArgs(std::get<8>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==10){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)), toArgs(std::get<7>(row)), toArgs(std::get<8>(row)), toArgs(std::get<9>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==11){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)), toArgs(std::get<7>(row)), toArgs(std::get<8>(row)), toArgs(std::get<9>(row)), toArgs(std::get<10>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                else if constexpr(std::tuple_size<std::tuple<Types...>>::value==21){
                    tObject = jniEnv->NewObject(jtcls, constructorTId, toArgs(std::get<0>(row)), toArgs(std::get<1>(row)), toArgs(std::get<2>(row)), toArgs(std::get<3>(row)), toArgs(std::get<4>(row)), toArgs(std::get<5>(row)), toArgs(std::get<6>(row)), toArgs(std::get<7>(row)), toArgs(std::get<8>(row)), toArgs(std::get<9>(row)), toArgs(std::get<10>(row)),
                                                                       toArgs(std::get<11>(row)), toArgs(std::get<12>(row)), toArgs(std::get<13>(row)), toArgs(std::get<14>(row)), toArgs(std::get<15>(row)), toArgs(std::get<16>(row)), toArgs(std::get<17>(row)), toArgs(std::get<18>(row)), toArgs(std::get<19>(row)), toArgs(std::get<20>(row)));
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
                jniEnv->CallBooleanMethod(alObject, methodALId, tObject);
                if (jniEnv->ExceptionCheck()) {
                   jniEnv->ExceptionDescribe();
                   jniEnv->ExceptionClear();
                }
                
            }
            jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, alObject);
            if (jniEnv->ExceptionCheck()) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }

        };
        std::string render();

        protected:

        inline jstring toArgs(std::string& arg){
            //jvalue ret = jniEnv->NewStringUTF(arg.c_str());
            return jniEnv->NewStringUTF(arg.c_str());
        };
        
        inline jobject toArgs(long long arg){
            jobject lObject = jniEnv->NewObject(jlongcls, constructorLongId, (jlong) arg);
            if (lObject == NULL) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }
            return lObject;
        };

        inline jobject toArgs(short arg){
            jobject lObject = jniEnv->NewObject(jshortcls, constructorShortId, (jshort) arg);
            if (lObject == NULL) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }
            return lObject;
        };

        inline jobject toArgs(int arg){
            jobject lObject = jniEnv->NewObject(jintcls, constructorIntId, (jint) arg);
            if (lObject == NULL) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }
            return lObject;
        };

        inline jobject toArgs(unsigned long long arg){
            jobject lObject = jniEnv->NewObject(jlongcls, constructorLongId, (jlong) arg);
            if (lObject == NULL) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }
            return lObject;
        };
        
        inline jobject toArgs(double arg){
            jobject lObject = jniEnv->NewObject(jdoublecls, constructorDoubleId, (jdouble) arg);
            if (lObject == NULL) {
               jniEnv->ExceptionDescribe();
               jniEnv->ExceptionClear();
            }
            return lObject;
        };

    };
}
