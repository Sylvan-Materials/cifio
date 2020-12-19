#pragma once

#include <jni.h>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>

namespace sylvanmats::utils{

    class JVMSingleton{
        static JVMSingleton* jvmSingleton;
        JVMSingleton();
        JVMSingleton(const JVMSingleton& orig) = delete;
        virtual ~JVMSingleton();
        static JavaVM *javaVM;
        static JNIEnv *jniEnv;
        public:
        static JVMSingleton* getInstance();
        JNIEnv* getEnv(){return jniEnv;};
    };
}
