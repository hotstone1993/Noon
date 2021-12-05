#include <jni.h>
#include <string>
#include "include/Processor.h"

extern "C" JNIEXPORT jint JNICALL
Java_com_example_core_NativeLib_create(
        JNIEnv *env,
        jobject /* this */) {
    int result = 0;

    Processor* processor = new Processor();
    processor->loadModel("test");
    delete processor;

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
                jobject /* this */) {
    int result = 0;
    return result;
}