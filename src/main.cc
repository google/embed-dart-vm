#include "dart.h"
#include "isolate.h"
#include "scoped_ptr.h"

#include <iostream>

int main(int argc, const char* argv[]) {
  dart::Initialize(argc, argv);
  std::cout << "Dart Initialized" << std::endl;

  scoped_ptr<dart::Isolate> isolate(dart::LoadScript("helloworld.dart"));
  assert(!!isolate);
  std::cout << "Script loaded into " << isolate.get() << std::endl;
 
  std::cout << "Invoking 'main'" << std::endl;
  isolate->Invoke("main"); 
  std::cout << "Done" << std::endl;

  return 0;
}
