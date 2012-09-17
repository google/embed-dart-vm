#include <unistd.h>

#include <iostream>

#include "isolate.h"
#include "library.h"

extern const uint8_t* snapshot_buffer;

namespace dart {

Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
                              Dart_Handle library,
                              Dart_Handle url) {
  const char* url_str = NULL;
  Dart_Handle result = Dart_StringToCString(url, &url_str);
  if (Dart_IsError(result))
    return result;
  assert(false);  // TODO: implement.
}

Dart_Handle ResolveScript(const char* script, Dart_Handle core_library) {
  char* cwd = getcwd(NULL, 0);
  Dart_Handle args[3] = {
    Dart_NewString(cwd),
    Dart_NewString(script),
    Dart_True()  // TODO: should this be true or false?
  };
  Dart_Handle ret = Dart_Invoke(
      core_library, Dart_NewString("_resolveScriptUri"), 3, args);
  free(cwd);
  return ret;
}

Dart_Handle FilePathFromUri(Dart_Handle script, Dart_Handle core_library) {
  Dart_Handle args[2] = {
    script,
    Dart_True()  // TODO: should this be true or false?
  };
  return Dart_Invoke(core_library, Dart_NewString("_filePathFromUri"), 2, args);
}

Dart_Handle ReadSource(Dart_Handle script, Dart_Handle core_library) {
  Dart_Handle script_path = FilePathFromUri(script, core_library);
  if (Dart_IsError(script_path))
    return script_path;

  const char* script_path_str;
  Dart_StringToCString(script_path, &script_path_str);

  FILE* file = fopen(script_path_str, "r");
  if (file == NULL)
    return Dart_Error("Unable to read file '%s'", script_path_str);

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = new char[length + 1];
  size_t read = fread(buffer, 1, length, file);
  fclose(file);
  buffer[read] = '\0';

  Dart_Handle source = Dart_NewString(buffer);

  delete[] buffer;

  return source;
}

Dart_Handle LoadScript(const char* script,
                       bool resolve,
                       Dart_Handle core_library) {
  std::cout << __FUNCTION__ << ": " << script << ", " << resolve << std::endl;
  Dart_Handle resolved_script;

  if (resolve) {
    resolved_script = ResolveScript(script, core_library);
    if (Dart_IsError(resolved_script))
      return resolved_script;
  } else {
    resolved_script = Dart_NewString(script);
  }

  Dart_Handle source = ReadSource(resolved_script, core_library);
  if (Dart_IsError(source))
    return source;

  return Dart_LoadScript(resolved_script, source);
}

Isolate* CreateIsolate(const char* script, const char* main, bool resolve,
                       void* data, char** error) {
  std::cout << __FUNCTION__ << ": " << script << ", " << main << ", "
            << resolve << std::endl;
  Dart_Isolate isolate = Dart_CreateIsolate(script, main, snapshot_buffer,
                                            NULL, error);
  assert(isolate);

  std::cout << "Created isolate" << std::endl;
  Dart_EnterScope();

  Dart_Handle result = Dart_SetLibraryTagHandler(LibraryTagHandler);

  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }

  // Set up uri library.
  Dart_Handle uri_library = Isolate::uri_library->Load();
  if (Dart_IsError(uri_library)) {
    *error = strdup(Dart_GetError(result));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }

  // Set up core library.
  Dart_Handle core_library = Isolate::core_library->Load();
  if (Dart_IsError(core_library)) {
    *error = strdup(Dart_GetError(result));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }

  // Set up io library.
  Dart_Handle io_library = Isolate::io_library->Load();
  if (Dart_IsError(io_library)) {
    *error = strdup(Dart_GetError(result));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }
  std::cout << "Loaded builtin libraries" << std::endl;

  std::cout << "About to load " << script << std::endl;
  Dart_Handle library = LoadScript(script, true, core_library);

  if (Dart_IsError(library)) {
    *error = strdup(Dart_GetError(library));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }

  result = Dart_LibraryImportLibrary(library, core_library, Dart_Null());

  if (Dart_IsError(library)) {
    *error = strdup(Dart_GetError(library));
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    return NULL;
  }

  return new Isolate(isolate, library);
}

bool IsolateCreateCallback(const char* script_uri,
                           const char* main,
                           void* callback_data,
                           char** error) {
  Isolate* isolate = CreateIsolate(script_uri, main, true, NULL, error);
  if (isolate == NULL) {
    std::cerr << "Failed to create Isolate: " << script_uri << "|" << main
              << ": " << error << std::endl;
  }
  return isolate != NULL;
}

bool IsolateInterruptCallback() {
  return true;
}

void IsolateShutdownCallback(void* callback_data) {
}

// public
void Initialize(int argc, const char* argv[]) {
  Dart_SetVMFlags(argc, argv);
  assert(Dart_Initialize(IsolateCreateCallback,
                         IsolateInterruptCallback,
                         IsolateShutdownCallback));

  Isolate::InitializeBuiltinLibraries();
}

// public
void Shutdown() {
  Isolate::ShutdownBuiltinLibraries();
}

// public
Isolate* LoadScript(const char* script) {
  std::cout << __FUNCTION__ << ": " << script << std::endl;
  char* error = NULL;
  Isolate* isolate = CreateIsolate(script, "main", true, NULL, &error);
  if (!isolate)
    std::cerr << "Failed to create Isolate: " << std::endl
              << "    " << error << std::endl;
  return isolate;
}

}

