#include "isolate.h"
#include "library.h"

#include <iostream>

namespace dart {
namespace {

Dart_NativeFunction CoreLibraryResolver(Dart_Handle name, int arg_count) {
  const char* nativeFunctionName = 0;
  Dart_StringToCString(name, &nativeFunctionName);
  std::cout << nativeFunctionName << " unresolved." << std::endl;
  return NULL;
}

Library* CreateCoreLibrary() {
  std::cout << __FUNCTION__ << std::endl;
  /*
     native_entry_exit = { fnv1aHash("Exit"), FUNCTION_NAME(Exit), "Exit", 1 };
     native_entry_print = { fnv1aHash("Logger_PrintString"),
     FUNCTION_NAME(Print), "Print", 1 };
     native_entry_exit = { fnv1aHash(""), NULL, "", 0 };
   */
  return new Library("dart:builtin", NULL, CoreLibraryResolver, NULL);
}

void SetupIONativeEntries() {
  //		__setupClassEntries();

  //		__setupDirectoryEntries();
  //		__setupEventHandlerEntries();
  //		__setupFileEntries();
  //		__setupPlatformEntries();
  //		__setupProcessEntries();
  //		__setupServerSocketEntries();
  //		__setupSocketEntries();
}

Dart_NativeFunction IOLibraryResolver(Dart_Handle name, int arg_count) {
  const char* nativeFunctionName = 0;
  Dart_StringToCString(name, &nativeFunctionName);
  std::cout << nativeFunctionName << " unresolved." << std::endl;
  return NULL;
}

void IOLibraryInitializer(Dart_Handle library) {
  Dart_Handle timerClosure = Dart_Invoke(library,
      Dart_NewString("_getTimerFactoryClosure"), 0, 0);
  Dart_Handle isolateLibrary = Dart_LookupLibrary(
      Dart_NewString("dart:isolate"));

  Dart_Handle args[1];
  args[0] = timerClosure;
  Dart_Handle result =
    Dart_Invoke(isolateLibrary, Dart_NewString("_setTimerFactoryClosure"),
        1, args);
  assert(!Dart_IsError(result));
}

Library* CreateIOLibrary() {
  std::cout << __FUNCTION__ << std::endl;
  // Setup the native entries
  SetupIONativeEntries();

  // The library source is compiled in along with the snapshot.
  // Native entries are present within the library so a resolver needs to be set.
  // IO initializer allows some functions to be called before the library is used.
  return new Library("dart:io", 0, IOLibraryResolver, IOLibraryInitializer);
}

Library* CreateUriLibrary() {
  std::cout << __FUNCTION__ << std::endl;
  return new Library("dart:uri", NULL, NULL, NULL);
}

}

// static
Library* Isolate::core_library = NULL;

// static
Library* Isolate::io_library = NULL;

// static
Library* Isolate::uri_library = NULL;

// static
void Isolate::InitializeBuiltinLibraries() {
  std::cout << __FUNCTION__ << std::endl;
  assert(core_library == NULL);
  core_library = CreateCoreLibrary();
  io_library = CreateIOLibrary();
  uri_library = CreateUriLibrary();
}

void Isolate::Invoke(const char* function) {
  std::cout << __FUNCTION__ << ": " << function << std::endl;
  Dart_EnterScope();
  Dart_Handle result =
    Dart_Invoke(library_, Dart_NewString(function), 0, NULL);
  assert(!Dart_IsError(result));
  Dart_RunLoop();
  Dart_ExitScope();
}

}
