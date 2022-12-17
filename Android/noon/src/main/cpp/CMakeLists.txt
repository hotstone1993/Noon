cmake_minimum_required(VERSION 3.10.2)

project("Android_Noon")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../src
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../../build/Noon)

add_library( # Sets the name of the library.
        ${PROJECT_NAME}
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             native.cpp )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        ${PROJECT_NAME} PUBLIC
                        Noon )