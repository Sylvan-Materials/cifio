#include <filesystem>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "publishing/CIFPublisher.h"


namespace sylvanmats::publishing{

    CIFPublisher::CIFPublisher(){
        std::string classPath="../../cifio/src:../../ST-4.3.1.jar:../../javatuples-1.2.jar:../../antlr4/tool/target/antlr4-4.8-2-SNAPSHOT-complete.jar:../../:.";
        if(getenv("CLASSPATH")!=NULL){
        }
        classPath="-Djava.class.path="+classPath;
       JavaVMOption jvmopt[3];
       jvmopt[0].optionString = (char*)classPath.c_str();
        jvmopt[1].optionString = (char*)"-Xcheck:jni";
        jvmopt[2].optionString = (char*)"-Xss1000m";

       JavaVMInitArgs vmArgs;
       vmArgs.version = JNI_VERSION_10;
       vmArgs.nOptions = 3;
       vmArgs.options = jvmopt;
       vmArgs.ignoreUnrecognized = JNI_TRUE;

       // Create the JVM
       long flag = JNI_CreateJavaVM(&javaVM, (void**)
          &jniEnv, &vmArgs);
       if (flag == JNI_ERR) {
          std::cout << "Error creating VM. Exiting...\n";
          return;
       }

       jcls = jniEnv->FindClass("org/stringtemplate/v4/ST");
       if (jcls == NULL) {
          jniEnv->ExceptionDescribe();
          javaVM->DestroyJavaVM();
          return;
       }
       if (jcls != NULL) {
          jmethodID constructorId = jniEnv->GetMethodID(jcls, "<init>", "(Ljava/lang/String;)V");
          if (constructorId != NULL) {
            std::filesystem::path path="../../cif.st4";
            if(path.string().rfind("~/", 0)==0){
                const char* home = getenv("HOME");
                if (home) {
                    path=home+path.string().substr(1);
                }
            }
            if(std::filesystem::exists(path)){
                    std::ifstream file(path.c_str());
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
          javaVM->DestroyJavaVM();
          return;
       }
       constructorShortId = jniEnv->GetMethodID(jshortcls, "<init>", "(S)V");
       jintcls = jniEnv->FindClass("java/lang/Integer");
       if (jintcls == NULL) {
          jniEnv->ExceptionDescribe();
          javaVM->DestroyJavaVM();
          return;
       }
       constructorIntId = jniEnv->GetMethodID(jintcls, "<init>", "(I)V");
       jlongcls = jniEnv->FindClass("java/lang/Long");
       if (jlongcls == NULL) {
          jniEnv->ExceptionDescribe();
          javaVM->DestroyJavaVM();
          return;
       }
       constructorLongId = jniEnv->GetMethodID(jlongcls, "<init>", "(J)V");
       jdoublecls = jniEnv->FindClass("java/lang/Double");
       if (jdoublecls == NULL) {
          jniEnv->ExceptionDescribe();
          javaVM->DestroyJavaVM();
          return;
       }
       constructorDoubleId = jniEnv->GetMethodID(jdoublecls, "<init>", "(D)V");
    };

    CIFPublisher::~CIFPublisher(){
        javaVM->DestroyJavaVM();
    };

    void CIFPublisher::add(std::string name, std::string value){
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

    /*void CIFPublisher::add(std::string name, std::tuple<>& value){
    };*/

    std::string CIFPublisher::render(){
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
