namespace dart {
  class Isolate;

  void Initialize(int argc, const char* argv[]);
  Isolate* LoadScript(const char* script);
}
