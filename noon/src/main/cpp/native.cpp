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
        jobject inferenceInfo) {
    InferenceInfo info;
    jclass cls = env->GetObjectClass(inferenceInfo);

    jfieldID typeId = env->GetFieldID(cls, "type", "I");
    info.type = env->GetIntField(inferenceInfo, typeId);

    jfieldID inputInfoId = env->GetFieldID(cls, "input", "Lcom/newstone/noon/InferenceInfo$InputInfo;");
    jfieldID outputInfoId = env->GetFieldID(cls, "output", "Lcom/newstone/noon/InferenceInfo$OutputInfo;");
    jobject input = env->GetObjectField(inferenceInfo, inputInfoId);
    jobject output = env->GetObjectField(inferenceInfo, outputInfoId);
    jclass listCls = env->FindClass( "java/util/List" );
    jmethodID getMethodId = env->GetMethodID( listCls, "get", "(I)Ljava/lang/Object;" );
    jmethodID sizeMethodId = env->GetMethodID( listCls, "size", "()I" );
    jclass integerCls = env->FindClass("java/lang/Integer");
    jmethodID getIntMethodId = env->GetMethodID(integerCls, "intValue", "()I");

    jclass inputCls = env->GetObjectClass(input);
    jfieldID inputShapeFieldId = env->GetFieldID(inputCls, "shape", "Ljava/util/List;");
    jobject inputShapeObject = env->GetObjectField(input, inputShapeFieldId);
    int inputShapeSize = (int)env->CallIntMethod( inputShapeObject, sizeMethodId );
    for(int i = 0; i < inputShapeSize; ++i) {
        jobject itemObj = env->CallObjectMethod(inputShapeObject, getMethodId, i);
        int item = env->CallIntMethod(itemObj, getIntMethodId);
        info.input.shape.push_back(item);
        env->DeleteGlobalRef(itemObj);
    }

    jclass outputCls = env->GetObjectClass(output);
    jfieldID outputShapeFieldId = env->GetFieldID(outputCls, "shape", "Ljava/util/List;");
    jobject outputShapeObject = env->GetObjectField(output, outputShapeFieldId);
    int outputShapeSize = (int)env->CallIntMethod( outputShapeObject, sizeMethodId );
    for(int i = 0; i < outputShapeSize; ++i) {
        jobject itemObj = env->CallObjectMethod(outputShapeObject, getMethodId, i);
        int item = env->CallIntMethod(itemObj, getIntMethodId);
        info.output.shape.push_back(item);
        env->DeleteGlobalRef(itemObj);
    }

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