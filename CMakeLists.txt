cmake_minimum_required(VERSION 2.8)
project(embed-dart-vm)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) 
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os")
set(DART_SDK_ROOT ~/git/dart/dart)
include_directories(${DART_SDK_ROOT}/runtime/include)
link_directories(
    ${DART_SDK_ROOT}/xcodebuild/${CMAKE_BUILD_TYPE})
add_executable(embed-dart-vm main.cc)
target_link_libraries(embed-dart-vm
    dart
    dart_builtin
    dart_export
    dart_lib
    dart_vm
    double_conversion
    jscre)
