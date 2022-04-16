#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "include/Noon.h"

const char* const INSTANCE = "nativeInstance";

int printResult(int result) {
    switch (result) {
        case -1:
            __android_log_print(ANDROID_LOG_ERROR, TAG, "Unknown Error");
            break;
        case 0:
            __android_log_print(ANDROID_LOG_INFO, TAG, "SUCCESS");
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, TAG, "ERROR TYPE: %d", result);
            break;
    }

    return result;
}

Noon<uint8_t, float>* getInstance(JNIEnv* env, const jobject& obj) {
    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    jlong instancePointer = env->GetLongField(obj, id);
    return reinterpret_cast<Noon<uint8_t, float>*>(instancePointer);
}

extern "C" JNIEXPORT jint JNICALL Java_com_newstone_noon_Noon_create(
        JNIEnv *env,
        jobject obj /* this */) {
    int result = SUCCESS;

    Noon<uint8_t, float>* newInstance = new Noon<uint8_t, float>();

    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    env->SetLongField(obj, id, reinterpret_cast<jlong>(newInstance));

    return printResult(result);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_loadModel(
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

    Noon<uint8_t, float>* instance =  getInstance(env, obj);
    int result = instance->loadModel(buffer, fileSize);

    AAsset_close(asset);
    delete[] buffer;

    return printResult(result);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_setup(
        JNIEnv *env,
        jobject obj,
        jint width, jint height, jint pixelStride) {
    InferenceInfo info;
    info.type = static_cast<int>(InferenceType::IMAGE);
    info.input.shape.push_back(width);
    info.input.shape.push_back(height);
    info.input.shape.push_back(pixelStride);
    info.output.shape.push_back(10);

    return printResult(getInstance(env, obj)->setup(info));
}

extern "C" JNIEXPORT jint JNICALL
        Java_com_newstone_noon_Noon_inference(
                JNIEnv *env,
                jobject obj,
                jbyteArray input,
                jfloatArray output) {
    int result = SUCCESS;

    Noon<uint8_t, float>* instance = getInstance(env, obj);
    jbyte* inputBuffer = env->GetByteArrayElements(input, nullptr);
    jfloat* outputBuffer = env->GetFloatArrayElements(output, nullptr);

    result = instance->inference(reinterpret_cast<uint8_t *>(inputBuffer), outputBuffer);

    env->ReleaseByteArrayElements(input, inputBuffer, JNI_ABORT);
    env->ReleaseFloatArrayElements(output, outputBuffer, 0);

    return printResult(result);
}

extern "C" JNIEXPORT void JNICALL
Java_com_newstone_noon_Noon_saveImage(
        JNIEnv *env,
        jobject obj) {
    Noon<uint8_t, float>* instance = getInstance(env, obj);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_newstone_noon_Noon_getBenchmark(JNIEnv *env, jobject obj) {
    Noon<uint8_t, float>* instance = getInstance(env, obj);
    const std::string& result = instance->getBenchmark(BM_PROCESSING);
    jstring jbuf = env->NewStringUTF(result.c_str());
    return jbuf;
}