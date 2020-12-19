#include <filesystem>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
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
          jmethodID constructorId = jniEnv->GetMethodID(jcls, "<init>", "(Ljava/lang/String;)V");
          if (constructorId != NULL) {
            if(stPath.string().rfind("~/", 0)==0){
                const char* home = getenv("HOME");
                if (home) {
                    stPath=home+stPath.string().substr(1);
                }
            }
            if(std::filesystem::exists(stPath)){
                    std::ifstream file(stPath.c_str());
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    file.close();
                jstring str = jniEnv->NewStringUTF(content.c_str());
                stObject = jniEnv->NewObject(jcls, constructorId, str);
                if (jniEnv->ExceptionCheck()) {
                   jniEnv->ExceptionDescribe();
                   jniEnv->ExceptionClear();
                }
            }
          }
          else std::cout<<"no constructor "<<std::endl;
       }
       jshortcls = jniEnv->FindClass("java/lang/Short");
       if (jshortcls == NULL) {
          jniEnv->ExceptionDescribe();
          return;
       }
       constructorShortId = jniEnv->GetMethodID(jshortcls, "<init>", "(S)V");
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
       constructorDoubleId = jniEnv->GetMethodID(jdoublecls, "<init>", "(D)V");
    };

    void Publisher::add(std::string name, std::string value){
       if (jcls != NULL) {
          jmethodID methodId = jniEnv->GetMethodID(jcls,
             "add", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/stringtemplate/v4/ST;");
          if (methodId != NULL) {
             jstring jname = jniEnv->NewStringUTF(name.c_str());
             jstring jvalue = jniEnv->NewStringUTF(value.c_str());
             jniEnv->CallObjectMethod(stObject, methodId, jname, jvalue);
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

    /*void Publisher::add(std::string name, std::tuple<>& value){
    };*/

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

