#include <jni.h>
#include <string>
#include "include/Processor.h"

const char* const INSTANCE = "nativeInstance";

Processor* getInstance(JNIEnv* env, const jobject& obj) {
    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    jlong instancePointer = env->GetLongField(obj, id);
    return reinterpret_cast<Processor*>(instancePointer);
}

extern "C" JNIEXPORT jint JNICALL Java_com_example_core_NativeLib_create(
        JNIEnv *env,
        jobject obj /* this */) {
    int result = 0;


    Processor* newInstance = new Processor();

    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    env->SetLongField(obj, id, reinterpret_cast<jlong>(newInstance));

    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_core_NativeLib_loadModel(
        JNIEnv *env,
        jobject /* this */, jstring path) {
    int result = 0;
    return result;
}
extern "C" JNIEXPORT jint JNICALL
        Java_com_example_core_NativeLib_inference(
                JNIEnv *env,
                jobject /* this */, jintArray arr) {
    int result = 0;

    return result;
}

