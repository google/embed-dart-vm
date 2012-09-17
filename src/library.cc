#include "library.h"

namespace dart {

Library::Library(const char* name, const char* source,
                 Dart_NativeEntryResolver native_resolver,
                 Dart_LibraryInitializer initializer)
    : name_(name),
      source_(source),
      native_resolver_(native_resolver),
      initializer_(initializer) {
}

Dart_Handle Library::Load() {
  Dart_Handle url = Dart_NewString(name_);
  Dart_Handle library = Dart_LookupLibrary(url);

  if (Dart_IsError(library))
    library = Dart_LoadLibrary(url, Dart_NewString(source_));

  if (Dart_IsError(library)) {
    std::cerr << "Failed to load library: " << name_ << "|" << source_
              << ": " << Dart_GetError(library) << std::endl;
    return library;
  }

  if (native_resolver_ != NULL)
    Dart_SetNativeResolver(library, native_resolver_);
  if (initializer_ != NULL)
    initializer_(library);

  return library;
}

}
