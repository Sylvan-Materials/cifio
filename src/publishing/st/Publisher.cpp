#include <filesystem>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
//#include <format>

#include "publishing/st/Publisher.h"
#include "utils/JVMSingleton.h"

namespace sylvanmats::publishing::st{

    Publisher::Publisher(std::filesystem::path& stPath) : stPath (stPath){

        sylvanmats::utils::JVMSingleton* jvmSingleton=sylvanmats::utils::JVMSingleton::getInstance();
        jniEnv=jvmSingleton->getEnv();
       jcls = jniEnv->FindClass("org/stringtemplate/v4/ST");
       if (jcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       if (jcls != NULL) {
        jclass jdcls = jniEnv->FindClass("org/stringtemplate/v4/STGroupDir");
        if (jdcls == NULL) {
           jniEnv->ExceptionDescribe();
           return;
        }
          jmethodID constructorId = jniEnv->GetMethodID(jdcls, "<init>", "(Ljava/lang/String;)V");
          if (constructorId != NULL) {
            if(stPath.string().rfind("~/", 0)==0){
                const char* home = getenv("HOME");
                if (home) {
                    stPath=home+stPath.string().substr(1);
                }
            }
            if(std::filesystem::exists(stPath)){
                    //std::ifstream file(stPath.c_str());
                    //std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    //file.close();
                jstring str = jniEnv->NewStringUTF(stPath.c_str());
                stGroupDirObject = jniEnv->NewObject(jdcls, constructorId, str);
                if (jniEnv->ExceptionCheck()) {
                   jniEnv->ExceptionDescribe();
                   jniEnv->ExceptionClear();
                }
                jclass jncls = jniEnv->FindClass("java/lang/Number");
                jclass jscls = jniEnv->FindClass("java/lang/String");
                jclass jnrcls = jniEnv->FindClass("org/stringtemplate/v4/NumberRenderer");
                jmethodID constructorNRId = jniEnv->GetMethodID(jnrcls, "<init>", "()V");
                jobject stNRObject = jniEnv->NewObject(jnrcls, constructorNRId);

                jclass jsrcls = jniEnv->FindClass("org/stringtemplate/v4/StringRenderer");
                jmethodID constructorSRId = jniEnv->GetMethodID(jsrcls, "<init>", "()V");
                jobject stSRObject = jniEnv->NewObject(jsrcls, constructorSRId);

                jmethodID methodId = jniEnv->GetMethodID(jdcls, "registerRenderer", "(Ljava/lang/Class;Lorg/stringtemplate/v4/AttributeRenderer;)V");
                jniEnv->CallVoidMethod(stGroupDirObject, methodId, jncls, stNRObject);
                jniEnv->CallVoidMethod(stGroupDirObject, methodId, jscls, stSRObject);
            }
          }
          else std::cout<<"no constructor "<<std::endl;
          jmethodID methodId = jniEnv->GetMethodID(jdcls, "getInstanceOf", "(Ljava/lang/String;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
            if(stPath.string().rfind("~/", 0)==0){
                const char* home = getenv("HOME");
                if (home) {
                    stPath=home+stPath.string().substr(1);
                }
            }
            if(std::filesystem::exists(stPath)){
                for(auto& p: std::filesystem::directory_iterator(stPath)){
                    jstring str = jniEnv->NewStringUTF(p.path().stem().c_str());
                    stObject = jniEnv->CallObjectMethod(stGroupDirObject, methodId, str);
                    if (jniEnv->ExceptionCheck()) {
                       jniEnv->ExceptionDescribe();
                       jniEnv->ExceptionClear();
                    }
                }
            }
          }
       }
       jshortcls = jniEnv->FindClass("java/lang/Short");
       if (jshortcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       constructorShortId = jniEnv->GetMethodID(jshortcls, "<init>", "(S)V");
       jboolcls = jniEnv->FindClass("java/lang/Boolean");
       if (jboolcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       constructorBoolId = jniEnv->GetMethodID(jboolcls, "<init>", "(Z)V");
       jintcls = jniEnv->FindClass("java/lang/Integer");
       if (jintcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       constructorIntId = jniEnv->GetMethodID(jintcls, "<init>", "(I)V");
       jlongcls = jniEnv->FindClass("java/lang/Long");
       if (jlongcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       constructorLongId = jniEnv->GetMethodID(jlongcls, "<init>", "(J)V");
       jdoublecls = jniEnv->FindClass("java/lang/Double");
       if (jdoublecls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
          std::cout << "jdoublecls. ...\n";
       constructorDoubleId = jniEnv->GetMethodID(jdoublecls, "<init>", "(D)V");
    };

    void Publisher::add(std::string name, const char* value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jstring jvalue = jniEnv->NewStringUTF(value);
             jniEnv->CallObjectMethod(stObject, methodId, jname, jvalue);
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };

    void Publisher::add(std::string name, std::string value){
       Publisher::add(name, value.c_str());
    };

    void Publisher::add(std::string name, bool value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, toArgs(value));
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };
    void Publisher::add(std::string name, long long value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, toArgs(value));
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };
    void Publisher::add(std::string name, unsigned long long value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, toArgs(value));
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };
    void Publisher::add(std::string name, double value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, toArgs(value));
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };

    void Publisher::rawSetAttribute(std::string name, bool value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "rawSetAttribute", "(Ljava/lang/String;Ljava/lang/Object;)V");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jniEnv->CallVoidMethod(stObject, methodId, jname, toArgs(value));
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
          }
       }
    };

    std::string Publisher::render(){
         std::string content;
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls, "render", "()Ljava/lang/String;");
          if (methodId != NULL) {
             jstring jcontent = (jstring)jniEnv->CallObjectMethod(stObject, methodId);
             if (jniEnv->ExceptionCheck()) {
                jniEnv->ExceptionDescribe();
                jniEnv->ExceptionClear();
             }
            const char *cstr = jniEnv->GetStringUTFChars(jcontent, NULL);
            content = std::string(cstr);
            jniEnv->ReleaseStringUTFChars(jcontent, cstr);             
          }
       }
       return content;
    };

}

