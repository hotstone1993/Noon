#include <jni.h>
#include <string>
#include <android/log.h>
#include "include/Noon.h"

const char* const INSTANCE = "nativeInstance";
void** outputBuffer = nullptr;
const char* TAG = "Noon";

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

Noon* getInstance(JNIEnv* env, const jobject& obj) {
    jclass cls = env->GetObjectClass(obj);
    jfieldID id = env->GetFieldID(cls, INSTANCE, "J");
    jlong instancePointer = env->GetLongField(obj, id);
    return reinterpret_cast<Noon*>(instancePointer);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_setup(
        JNIEnv *env,
        jobject obj,
        jobject inferenceInfo) {

    // create Noon instance
    Noon* newInstance = new Noon();

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
    if (inputShapeObject != NULL) {
        int inputShapeSize = (int)env->CallIntMethod( inputShapeObject, sizeMethodId );
        for(int i = 0; i < inputShapeSize; ++i) {
            jobject itemObj = env->CallObjectMethod(inputShapeObject, getMethodId, i);
            int item = env->CallIntMethod(itemObj, getIntMethodId);
            info.input.shape.push_back(item);
            env->DeleteLocalRef(itemObj);
        }
    }

    jclass outputCls = env->GetObjectClass(output);
    jfieldID outputShapeFieldId = env->GetFieldID(outputCls, "shape", "Ljava/util/List;");
    jobject outputShapeObject = env->GetObjectField(output, outputShapeFieldId);
    if (outputShapeObject != NULL) {
        int outputShapeSize = (int)env->CallIntMethod( outputShapeObject, sizeMethodId );
        for(int i = 0; i < outputShapeSize; ++i) {
            jobject itemObj = env->CallObjectMethod(outputShapeObject, getMethodId, i);
            int item = env->CallIntMethod(itemObj, getIntMethodId);
            info.output.shape.push_back(item);
            env->DeleteLocalRef(itemObj);
        }
    }
    newInstance->loadModel((char*)info.model, info.modelSize, (MLMode)info.type);

    return printResult(newInstance->setup(info));
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceULongArray(JNIEnv *env, jobject obj, jlongArray input) {
    Noon* instance = getInstance(env, obj);
    jlong* inputBuffer = env->GetLongArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<uint64_t *>(inputBuffer));
    env->ReleaseLongArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceLongArray(JNIEnv *env, jobject obj, jlongArray input) {
    Noon* instance = getInstance(env, obj);
    jlong* inputBuffer = env->GetLongArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<int64_t *>(inputBuffer));
    env->ReleaseLongArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceDoubleArray(JNIEnv *env, jobject obj,
                                                       jdoubleArray input) {
    Noon* instance = getInstance(env, obj);
    jdouble* inputBuffer = env->GetDoubleArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<double *>(inputBuffer));
    env->ReleaseDoubleArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceFloatArray(JNIEnv *env, jobject obj,
                                                      jfloatArray input) {
    Noon* instance = getInstance(env, obj);
    jfloat* inputBuffer = env->GetFloatArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<float *>(inputBuffer));
    env->ReleaseFloatArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceIntArray(JNIEnv *env, jobject obj, jintArray input) {
    Noon* instance = getInstance(env, obj);
    jint* inputBuffer = env->GetIntArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<int32_t *>(inputBuffer));
    env->ReleaseIntArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceUIntArray(JNIEnv *env, jobject obj, jintArray input) {
    // TODO: implement nativeInferenceUIntArray()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceShortArray(JNIEnv *env, jobject obj,
                                                      jshortArray input) {
    Noon* instance = getInstance(env, obj);
    jshort* inputBuffer = env->GetShortArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<int16_t *>(inputBuffer));
    env->ReleaseShortArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceUShortArray(JNIEnv *env, jobject obj, jshortArray input) {
    Noon* instance = getInstance(env, obj);
    jshort* inputBuffer = env->GetShortArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<uint16_t *>(inputBuffer));
    env->ReleaseShortArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceByteArray(JNIEnv *env, jobject obj, jbyteArray input) {
    Noon* instance = getInstance(env, obj);
    jbyte* inputBuffer = env->GetByteArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<int8_t *>(inputBuffer));
    env->ReleaseByteArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_inferenceUByteArray(JNIEnv *env, jobject obj, jbyteArray input) {
    Noon* instance = getInstance(env, obj);
    jbyte* inputBuffer = env->GetByteArrayElements(input, nullptr);
    int result = instance->inference(reinterpret_cast<uint8_t *>(inputBuffer));
    env->ReleaseByteArrayElements(input, inputBuffer, JNI_ABORT);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetULongArrayOutput(JNIEnv *env, jobject obj, jobject output) {
    // TODO: implement nativeGetULongArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetLongArrayOutput(JNIEnv *env, jobject obj, jlongArray output) {
    // TODO: implement nativeGetLongArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetDoubleArrayOutput(JNIEnv *env, jobject obj,
                                                       jdoubleArray output) {
    // TODO: implement nativeGetDoubleArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetFloatArrayOutput(JNIEnv *env, jobject obj,
                                                      jfloatArray output) {
    jfloat* outputBuffer = env->GetFloatArrayElements(output, nullptr);
    Noon* instance = getInstance(env, obj);
    int result = instance->getOutput(outputBuffer);
    env->ReleaseFloatArrayElements(output, outputBuffer, 0);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetIntArrayOutput(JNIEnv *env, jobject obj, jintArray output) {
    // TODO: implement nativeGetIntArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetUIntArrayOutput(JNIEnv *env, jobject obj, jobject output) {
    // TODO: implement nativeGetUIntArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetShortArrayOutput(JNIEnv *env, jobject obj,
                                                      jshortArray output) {
    // TODO: implement nativeGetShortArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetUShortArrayOutput(JNIEnv *env, jobject obj, jobject output) {
    // TODO: implement nativeGetUShortArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetByteArrayOutput(JNIEnv *env, jobject obj, jbyteArray output) {
    // TODO: implement nativeGetByteArrayOutput()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_newstone_noon_Noon_nativeGetUByteArrayOutput(JNIEnv *env, jobject obj, jobject output) {
    // TODO: implement nativeGetUByteArrayOutput()
}

extern "C" JNIEXPORT void JNICALL
Java_com_newstone_noon_Noon_saveImage(
        JNIEnv *env,
        jobject obj) {
    Noon* instance = getInstance(env, obj);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_newstone_noon_Noon_getBenchmark(JNIEnv *env, jobject obj) {
    Noon* instance = getInstance(env, obj);
    const std::string& result = instance->getBenchmark(BM_PROCESSING);
    jstring jbuf = env->NewStringUTF(result.c_str());
    return jbuf;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_newstone_noon_Noon_destroy(JNIEnv *env, jobject obj) {
    if (outputBuffer != nullptr) {
        delete[] outputBuffer;
    }
}