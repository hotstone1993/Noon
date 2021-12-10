#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "include/Processor.h"

const char* const INSTANCE = "nativeInstance";
s
Processor* getInstance(JNIEnv* env, const jobject& obj) {
    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    jlong instancePointer = env->GetLongField(obj, id);
    return reinterpret_cast<Processor*>(instancePointer);
}

extern "C" JNIEXPORT jint JNICALL Java_com_example_core_NativeLib_create(
        JNIEnv *env,
        jobject obj /* this */) {
    int result = SUCESS;

    Processor* newInstance = new Processor();

    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    env->SetLongField(obj, id, reinterpret_cast<jlong>(newInstance));

    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_core_NativeLib_loadModel(
        JNIEnv *env,
        jobject obj,
        jobject assetManager, jstring modelPath, jstring labelPath) {

    const char* model = env->GetStringUTFChars(modelPath, nullptr);
    const char* label = env->GetStringUTFChars(labelPath, nullptr);

    AAssetManager *manager = AAssetManager_fromJava(env, assetManager);
    AAsset* asset = AAssetManager_open(manager, model, AASSET_MODE_UNKNOWN);
    env->ReleaseStringUTFChars(modelPath, model);
    env->ReleaseStringUTFChars(labelPath, label);

    if ( asset == nullptr )
        return FAIL;
    off_t fileSize = AAsset_getLength(asset);
    if ( fileSize == 0 )
        return FAIL;

    char *buffer = new char[fileSize];
    if(AAsset_read(asset, buffer, fileSize) < 0)
       return FAIL;

    Processor* instance =  getInstance(env, obj);
    instance->loadModel(buffer, fileSize);

    AAsset_close(asset);

    return SUCESS;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_core_NativeLib_setup(
        JNIEnv *env,
        jobject obj,
        jint width, jint height, jint pixelStride) {
    return getInstance(env, obj)->setup(width, height, pixelStride);
}

extern "C" JNIEXPORT jint JNICALL
        Java_com_example_core_NativeLib_inference(
                JNIEnv *env,
                jobject obj, jbyteArray arr) {
    int result = SUCESS;

    Processor* instance = getInstance(env, obj);
    jsize size = env->GetArrayLength(arr);
    jbyte* buff = env->GetByteArrayElements(arr, nullptr);

    instance->inference(buff, size);

    env->ReleaseByteArrayElements(arr, buff, 0);

    return result;
}

