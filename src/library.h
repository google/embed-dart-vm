#include "dart_api.h"
#include <iostream>

typedef void (*Dart_LibraryInitializer)(Dart_Handle library);

namespace dart {

class Library {
   public:
    Library(const char* name, const char* source,
            Dart_NativeEntryResolver native_resolver,
            Dart_LibraryInitializer initializer);

    Dart_Handle Load();

   private:
    const char* name_;
    const char* source_;
    Dart_NativeEntryResolver native_resolver_;
    Dart_LibraryInitializer initializer_;
  };

}
