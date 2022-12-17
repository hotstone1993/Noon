cmake_minimum_required(VERSION 3.10.2)

project("Noon_Test")

if (NOT BUILD_TESTING)
        return()
endif()

enable_testing()

find_file(CONAN_SCRIPT 
    NAMES conanbuildinfo.txt
    PATHS ${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/build
)
if(CONAN_SCRIPT)
    get_filename_component(CONAN_SCRIPT_DIR ${CONAN_SCRIPT} DIRECTORY)
    list(APPEND CMAKE_MODULE_PATH ${CONAN_SCRIPT_DIR})
endif()

find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME} 
                    main.cpp
)

set_target_properties(${PROJECT_NAME}
PROPERTIES
    CXX_STANDARD 17
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
                        Catch2::Catch2)


add_test(NAME test_main
            COMMAND Noon_Test
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)                        
