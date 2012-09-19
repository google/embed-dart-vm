#include "isolate.h"
#include "library.h"

#include <iostream>
#include <map>
#include <string>

#define BUILTIN_FUNCTION_NAME(name) Builtin_##name
#define DECLARE_BUILTIN_FUNCTION(name) \
    extern void BUILTIN_FUNCTION_NAME(name)(Dart_NativeArguments args)

DECLARE_BUILTIN_FUNCTION(Exit);
DECLARE_BUILTIN_FUNCTION(Logger_PrintString);

namespace dart {
namespace {

typedef std::map<std::string, Dart_NativeFunction> NativeFunctionMap;

NativeFunctionMap core_native_function_map;
NativeFunctionMap io_native_function_map;

template<const NativeFunctionMap& function_map>
Dart_NativeFunction LibraryResolver(Dart_Handle name, int arg_count) {
  const char* native_function_name = 0;
  Dart_StringToCString(name, &native_function_name);
  NativeFunctionMap::const_iterator func_it =
      function_map.find(native_function_name);
  if (func_it != function_map.end())
    return func_it->second;
  std::cout << "WARNING: " << native_function_name << " is unresolved."
            << std::endl;
  return NULL;
}

Library* CreateCoreLibrary() {
  core_native_function_map.insert(
      std::make_pair("Exit", BUILTIN_FUNCTION_NAME(Exit)));
  core_native_function_map.insert(
      std::make_pair("Logger_PrintString",
                     BUILTIN_FUNCTION_NAME(Logger_PrintString)));
  return new Library("dart:builtin",
                     NULL,
                     LibraryResolver<core_native_function_map>,
                     NULL);
}

void IOLibraryInitializer(Dart_Handle library) {
  Dart_Handle timer_closure =
    Dart_Invoke(library, Dart_NewString("_getTimerFactoryClosure"), 0, 0);
  Dart_Handle isolate_library =
    Dart_LookupLibrary(Dart_NewString("dart:isolate"));

  Dart_Handle args[1];
  args[0] = timer_closure;
  Dart_Handle result = Dart_Invoke(isolate_library,
                                   Dart_NewString("_setTimerFactoryClosure"),
                                   1,
                                   args);
  assert(!Dart_IsError(result));
}

Library* CreateIOLibrary() {
  // TODO: register any native methods.
  return new Library("dart:io", 0, LibraryResolver<io_native_function_map>,
                     IOLibraryInitializer);
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

// static
void Isolate::ShutdownBuiltinLibraries() {
  assert(core_library != NULL);
  delete uri_library;
  delete io_library;
  delete core_library;
}

void Isolate::Invoke(const char* function) {
  std::cout << __FUNCTION__ << ": " << function << std::endl;
  Dart_EnterScope();
  Dart_Handle result = Dart_Invoke(library_,
                                   Dart_NewString(function),
                                   0,
                                   NULL);
  if (Dart_IsError(result)) {
    std::cerr << "Failed to invoke " << function << ": "
              << Dart_GetError(result);
  }
  Dart_RunLoop();
  Dart_ExitScope();
}

}
