cmake_minimum_required(VERSION 2.8)
project(embed-dart-vm)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) 

set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os")
set(DART_SDK_ROOT ~/git/dart/dart)
set(DART_SDK_GEN ${DART_SDK_ROOT}/xcodebuild/DerivedSources/${CMAKE_BUILD_TYPE})

# needed for dart_vm_gen
include_directories(${DART_SDK_ROOT}/runtime)
include_directories(${DART_SDK_ROOT}/runtime/include)
link_directories(${DART_SDK_ROOT}/xcodebuild/${CMAKE_BUILD_TYPE})

add_library(dart_vm_gen
    ${DART_SDK_GEN}/builtin_gen.cc
    ${DART_SDK_GEN}/corelib_gen.cc
    ${DART_SDK_GEN}/corelib_impl_gen.cc
    ${DART_SDK_GEN}/crypto_gen.cc
    ${DART_SDK_GEN}/io_gen.cc
    ${DART_SDK_GEN}/json_gen.cc
    ${DART_SDK_GEN}/snapshot_gen.cc
    ${DART_SDK_GEN}/uri_gen.cc
    ${DART_SDK_GEN}/utf_gen.cc)

add_library(embed_dart_vm
    src/dart.cc
    src/isolate.cc
    src/library.cc
    ${DART_SDK_ROOT}/runtime/bin/builtin.cc
    ${DART_SDK_ROOT}/runtime/bin/gen_snapshot.cc)

add_executable(embed-dart-vm_a src/main.cc)
target_link_libraries(embed-dart-vm_a
    embed_dart_vm
    dart_builtin
    dart_export
    dart_lib_withcore
    dart_vm
    dart_vm_gen
    double_conversion
    jscre)

add_executable(embed-dart-vm_b src/main.cc)
target_link_libraries(embed-dart-vm_b
    embed_dart_vm
    dart_builtin
    dart_export
    dart_lib
    dart_vm
    dart_vm_gen
    double_conversion
    jscre)
