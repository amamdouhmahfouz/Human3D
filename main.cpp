#include <iostream>
#include "MyExampleClass.h"
#include "MyOtherClass.h"

int main()
{
  std::cout << "Hello world" << std::endl;
  
  MyExampleClass example_class;
  example_class.functionExample();

  MyOtherClass other_class;
  other_class.otherFunction();

  std::cout << "done" << std::endl;

  return 0;
}