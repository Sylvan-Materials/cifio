#pragma once

#include <jni.h>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <cstdarg>

namespace sylvanmats::publishing::st{
    class Publisher{
    protected:
        std::filesystem::path stPath;
        JNIEnv *jniEnv;
        jclass jcls;
        jclass jboolcls;
        jclass jshortcls;
        jclass jintcls;
        jclass jlongcls;
        jclass jdoublecls;
        jmethodID constructorBoolId;
        jmethodID constructorShortId;
        jmethodID constructorIntId;
        jmethodID constructorLongId;
        jmethodID constructorDoubleId;
        jobject stGroupDirObject;
        jobject stObject;

        std::unordered_map<short, std::string> tupleMap = {{2, "Pair"}, {3, "Triplet"},{4, "Quartet"},{5, "Quintet"},{6, "Sextet"},{7, "Septet"},{8, "Octet"},{9, "Ennead"},{10, "Decade"},{11, "Hendecad"},{12, "Dodecad"},{21, "TwentyOne"}};

    public:
        Publisher() = delete;
        Publisher(std::filesystem::path& stPath);
        Publisher(const Publisher& orig) = delete;
        virtual ~Publisher() =  default;

//        virtual void add(std::string name, const char* value);
//        virtual void add(std::string name, std::string value);
//        virtual void add(std::string name, bool value);
//        virtual void add(std::string name, long long value);
//        virtual void add(std::string name, unsigned long long value);
//        virtual void add(std::string name, double value);
//        virtual void rawSetAttribute(std::string name, bool value);

        virtual std::string render() = 0;

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

        inline jobject toArgs(bool arg){
            jobject lObject = jniEnv->NewObject(jlongcls, constructorLongId, (jboolean) arg);
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

