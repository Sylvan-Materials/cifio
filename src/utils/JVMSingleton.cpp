
#include "utils/JVMSingleton.h"

namespace sylvanmats::utils{

    JVMSingleton* JVMSingleton::jvmSingleton=nullptr;
    JavaVM* JVMSingleton::javaVM=nullptr;
    JNIEnv* JVMSingleton::jniEnv=nullptr;

    JVMSingleton::JVMSingleton(){
        std::string classPath="../../cifio/src:../../ST-4.3.1.jar:../../javatuples-1.2.jar:../../antlr4/tool/target/antlr4-4.8-2-SNAPSHOT-complete.jar:../../:.";
        if(getenv("CLASSPATH")!=NULL){
        }
        classPath="-Djava.class.path="+classPath;
       JavaVMOption jvmopt[2];
       jvmopt[0].optionString = (char*)classPath.c_str();
        jvmopt[1].optionString = (char*)"-Xss1000m";
        //jvmopt[2].optionString = (char*)"-Xcheck:jni";

       JavaVMInitArgs vmArgs;
       vmArgs.version = JNI_VERSION_10;
       vmArgs.nOptions = 2;
       vmArgs.options = jvmopt;
       vmArgs.ignoreUnrecognized = JNI_TRUE;

       // Create the JVM
       long flag = JNI_CreateJavaVM(&javaVM, (void**)
          &jniEnv, &vmArgs);
       if (flag == JNI_ERR) {
          std::cout << "Error creating VM. Exiting...\n";
          return;
       }
    };
    JVMSingleton::~JVMSingleton(){
        //javaVM->DestroyJavaVM();
    };

    JVMSingleton* JVMSingleton::JVMSingleton::getInstance(){
        if(jvmSingleton==nullptr)
            jvmSingleton=new JVMSingleton();
        return jvmSingleton;
    };
}
