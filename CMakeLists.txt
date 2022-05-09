cmake_minimum_required(VERSION 3.10.2)

project("Native_Noon")

set( THIRDPARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty )
set( SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (ANDROID)
        set (JNI_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/Android/noon/src/main/jniLibs)
        set (TENSORFLOW_LITE_LIBS 
                ${JNI_LIBS}/${ANDROID_ABI}/libtensorflowlite.so
                ${JNI_LIBS}/${ANDROID_ABI}/libtensorflowlite_gpu_delegate.so
                ${JNI_LIBS}/${ANDROID_ABI}/libnnapi_delegate_no_nnapi_implementation.so
                ${JNI_LIBS}/${ANDROID_ABI}/libnnapi_implementation.so
                ${JNI_LIBS}/${ANDROID_ABI}/libnnapi_util.so
        )
        set (THIRDPARTY_LIBRARY android
                        log)
        find_library( ${THIRDPARTY_LIBRARY})
endif()

set( SOURCE_PATH_LIST
        ${SRC}/Noon.cpp
        ${SRC}/Processor.cpp
        ${SRC}/NoonTensorFlowLite.cpp)
        
set( HEADER_PATH_LIST
        ${SRC}/include
        ${THIRDPARTY_PATH}/tensorflow/ )

add_library( ${PROJECT_NAME}
             SHARED
             ${SOURCE_PATH_LIST} )

target_include_directories(${PROJECT_NAME} PUBLIC
                                ${HEADER_PATH_LIST})

target_link_libraries( ${PROJECT_NAME} PUBLIC
                        ${TENSORFLOW_LITE_LIBS}
                        ${THIRDPARTY_LIBRARY})