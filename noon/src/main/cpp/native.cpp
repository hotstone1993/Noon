#include <jni.h>
#include <string>
#include <android/log.h>
#include "include/Noon.h"

const char* const INSTANCE = "nativeInstance";
void** outputBuffer = nullptr;

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

extern "C" JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_setup(
        JNIEnv *env,
        jobject obj,
        jobject inferenceInfo) {

    // create Noon instance
    Noon<uint8_t, float>* newInstance = new Noon<uint8_t, float>();

    jclass instanceCls = env->GetObjectClass(obj);
    jfieldID instanceId = env->GetFieldID(instanceCls, INSTANCE, "J");
    env->SetLongField(obj, instanceId, reinterpret_cast<jlong>(newInstance));

    // setup InferenceInfo
    InferenceInfo info;
    jclass cls = env->GetObjectClass(inferenceInfo);
    jfieldID typeId = env->GetFieldID(cls, "type", "I");
    info.type = env->GetIntField(inferenceInfo, typeId);
    jfieldID delegateId = env->GetFieldID(cls, "delegate", "I");
    info.delegate = env->GetIntField(inferenceInfo, delegateId);
    jfieldID modelSizeId = env->GetFieldID(cls, "modelSize", "I");
    info.modelSize = env->GetIntField(inferenceInfo, modelSizeId);

    if (info.modelSize > 0) {
        jfieldID modelId = env->GetFieldID(cls, "model", "[B");
        jobject modelObject = env->GetObjectField (inferenceInfo, modelId);
        info.model = new int8_t[info.modelSize];
        jbyteArray arr = reinterpret_cast<jbyteArray>(modelObject);
        env->GetByteArrayRegion(arr, 0, sizeof(int8_t) * info.modelSize, info.model);
    }

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
        env->DeleteLocalRef(itemObj);
    }

    jclass outputCls = env->GetObjectClass(output);
    jfieldID outputShapeFieldId = env->GetFieldID(outputCls, "shape", "Ljava/util/List;");
    jobject outputShapeObject = env->GetObjectField(output, outputShapeFieldId);
    int outputShapeSize = (int)env->CallIntMethod( outputShapeObject, sizeMethodId );
    for(int i = 0; i < outputShapeSize; ++i) {
        jobject itemObj = env->CallObjectMethod(outputShapeObject, getMethodId, i);
        int item = env->CallIntMethod(itemObj, getIntMethodId);
        info.output.shape.push_back(item);
        env->DeleteLocalRef(itemObj);
    }

    return printResult(getInstance(env, obj)->setup(info));
}

extern "C" JNIEXPORT jint JNICALL
        Java_com_newstone_noon_Noon_nativeInference(
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

    return result;
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
extern "C"
JNIEXPORT void JNICALL
Java_com_newstone_noon_Noon_destroy(JNIEnv *env, jobject thiz) {
    if (outputBuffer != nullptr) {
        delete[] outputBuffer;
    }
}