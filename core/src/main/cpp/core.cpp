#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "include/Processor.h"

const char* const INSTANCE = "nativeInstance";

Processor<uint8_t, float>* getInstance(JNIEnv* env, const jobject& obj) {
    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    jlong instancePointer = env->GetLongField(obj, id);
    return reinterpret_cast<Processor<uint8_t, float>*>(instancePointer);
}

extern "C" JNIEXPORT jint JNICALL Java_com_example_core_NativeLib_create(
        JNIEnv *env,
        jobject obj /* this */) {
    int result = SUCCESS;

    Processor<uint8_t, float>* newInstance = new Processor<uint8_t, float>();

    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    env->SetLongField(obj, id, reinterpret_cast<jlong>(newInstance));

    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_core_NativeLib_loadModel(
        JNIEnv *env,
        jobject obj,
        jobject assetManager, jstring modelPath) {

    const char* model = env->GetStringUTFChars(modelPath, nullptr);

    AAssetManager *manager = AAssetManager_fromJava(env, assetManager);
    AAsset* asset = AAssetManager_open(manager, model, AASSET_MODE_UNKNOWN);
    env->ReleaseStringUTFChars(modelPath, model);

    if ( asset == nullptr )
        return FAIL;
    off_t fileSize = AAsset_getLength(asset);
    if ( fileSize == 0 )
        return FAIL;

    char *buffer = new char[fileSize];
    if(AAsset_read(asset, buffer, fileSize) < 0)
       return FAIL;

    Processor<uint8_t, float>* instance =  getInstance(env, obj);
    int result = instance->loadModel(buffer, fileSize);

    AAsset_close(asset);
    delete[] buffer;

    return result;
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
                jobject obj,
                jbyteArray input,
                jfloatArray output) {
    int result = SUCCESS;

    Processor<uint8_t, float>* instance = getInstance(env, obj);
    jbyte* inputBuffer = env->GetByteArrayElements(input, nullptr);
    jfloat* outputBuffer = env->GetFloatArrayElements(output, nullptr);

    result = instance->inference(reinterpret_cast<uint8_t *>(inputBuffer), outputBuffer);

    env->ReleaseByteArrayElements(input, inputBuffer, JNI_ABORT);
    env->ReleaseFloatArrayElements(output, outputBuffer, 0);

    return result;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_core_NativeLib_saveImage(
        JNIEnv *env,
        jobject obj) {
    Processor<uint8_t, float>* instance = getInstance(env, obj);
    instance->saveImage();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_core_NativeLib_getBenchmark(JNIEnv *env, jobject obj) {
    Processor<uint8_t, float>* instance = getInstance(env, obj);
    const std::string& result = instance->getBenchmark();
    jstring jbuf = env->NewStringUTF(result.c_str());
    return jbuf;
}