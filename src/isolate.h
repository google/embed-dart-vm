#include "dart_api.h"

namespace dart {

class Library;

class Isolate {
  public:
    Isolate(Dart_Isolate isolate, Dart_Handle library)
      : isolate_(isolate),
      library_(library) {}

    static void InitializeBuiltinLibraries();
    void Invoke(const char* function);

    static Library* core_library;
    static Library* uri_library;
    static Library* io_library;

  private:
    Dart_Isolate isolate_;
    Dart_Handle library_;
};

}
