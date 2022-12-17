cmake_minimum_required(VERSION 3.10.2)

set(TARGET "Noon")
set( THIRDPARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty )
set( SRC ${CMAKE_CURRENT_SOURCE_DIR})

if (ANDROID)
        set (JNI_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../Android/noon/src/main/jniLibs/${ANDROID_ABI})
        set (TENSORFLOW_LITE_LIBS 
                ${JNI_LIBS}/libtensorflowlite.so
                ${JNI_LIBS}/libtensorflowlite_gpu_delegate.so
                ${JNI_LIBS}/libnnapi_delegate_no_nnapi_implementation.so
                ${JNI_LIBS}/libnnapi_implementation.so
                ${JNI_LIBS}/libnnapi_util.so
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

add_library( ${TARGET}
             SHARED
             ${SOURCE_PATH_LIST} )
             
set_target_properties(${TARGET}
PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(${TARGET} PUBLIC
                                ${HEADER_PATH_LIST})

target_link_libraries( ${TARGET} PUBLIC
                        ${TENSORFLOW_LITE_LIBS}
                        ${THIRDPARTY_LIBRARY})