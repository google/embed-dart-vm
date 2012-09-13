#include <iostream>
#include "dart_api.h"

const char kScriptChars[] =
    "void main() {\n"
    "  print('hello world');\n"
    "}";

namespace dart {
  bool IsolateCreateCallback(const char* script_uri,
                             const char* main,
                             void* callback_data,
                             char** error) {
    std::cout << "uri: " << script_uri;
    std::cout << "main: " << main;
    std::cout << "callback_data: " << callback_data;

    Dart_Isolate isolate =
      Dart_CreateIsolate(script_uri, main, NULL, callback_data, error);
    if (isolate == NULL) return false;

    Dart_EnterIsolate(isolate);
    return true;
  }

  bool IsolateInterruptCallback() {
    return true;
  }

  void IsolateShutdownCallback(void* callback_data) {
    std::cout << "callback_data: " << callback_data;
    Dart_ExitIsolate();
    Dart_ShutdownIsolate();
  }
}

int main() {
  assert(Dart_Initialize(dart::IsolateCreateCallback,
                         dart::IsolateInterruptCallback,
                         dart::IsolateShutdownCallback));
  Dart_Handle lib = Dart_LoadScript(Dart_NewString("dart:test-lib"),
      Dart_NewString(kScriptChars));
  DART_CHECK_VALID(lib);
  // TODO: Benefit to Dart_CompileAll() ?
  Dart_Invoke(lib, Dart_NewString("main"), 0, NULL);
  return 0;
}
