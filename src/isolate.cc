#include "isolate.h"
#include "library.h"

#include <iostream>

namespace dart {
namespace {

Dart_NativeFunction CoreLibraryResolver(Dart_Handle name, int arg_count) {
  const char* nativeFunctionName = 0;
  Dart_StringToCString(name, &nativeFunctionName);
  std::cout << "WARNING: " << nativeFunctionName << " unresolved." << std::endl;
  return NULL;
}

Dart_NativeFunction IOLibraryResolver(Dart_Handle name, int arg_count) {
  const char* nativeFunctionName = 0;
  Dart_StringToCString(name, &nativeFunctionName);
  std::cout << "WARNING: " << nativeFunctionName << " unresolved." << std::endl;
  return NULL;
}

Library* CreateCoreLibrary() {
  // TODO: register any native methods.
  return new Library("dart:builtin", NULL, CoreLibraryResolver, NULL);
}

void IOLibraryInitializer(Dart_Handle library) {
  Dart_Handle timerClosure =
    Dart_Invoke(library, Dart_NewString("_getTimerFactoryClosure"), 0, 0);
  Dart_Handle isolateLibrary =
    Dart_LookupLibrary(Dart_NewString("dart:isolate"));

  Dart_Handle args[1];
  args[0] = timerClosure;
  Dart_Handle result = Dart_Invoke(isolateLibrary,
                                   Dart_NewString("_setTimerFactoryClosure"),
                                   1,
                                   args);
  assert(!Dart_IsError(result));
}

Library* CreateIOLibrary() {
  // TODO: register any native methods.
  return new Library("dart:io", 0, IOLibraryResolver, IOLibraryInitializer);
}

Library* CreateUriLibrary() {
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
  assert(core_library == NULL);
  core_library = CreateCoreLibrary();
  io_library = CreateIOLibrary();
  uri_library = CreateUriLibrary();
}

void Isolate::Invoke(const char* function) {
  std::cout << __FUNCTION__ << ": " << function << std::endl;
  Dart_EnterScope();
  Dart_Handle result = Dart_Invoke(library_,
                                   Dart_NewString(function),
                                   0,
                                   NULL);
  assert(!Dart_IsError(result));
  Dart_RunLoop();
  Dart_ExitScope();
}

}
