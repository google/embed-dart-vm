namespace dart {
  class Isolate;

  void Initialize(int argc, const char* argv[]);
  void Shutdown();
  Isolate* LoadScript(const char* script);
}
