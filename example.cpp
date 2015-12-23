//
// A simple example of mainmacro
//

#include "mainmacro.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

MAINMACRO(argc, argv)
{
#if defined(_WIN32)
  // Temporarily switch the console to utf-8
  UINT codePage = GetConsoleOutputCP();
  SetConsoleOutputCP(65001);
#endif

  std::cout << "Program: " << argv[0] << std::endl;
  for (int i = 1; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }

#if defined(_WIN32)
  // Restore the console code page
  SetConsoleOutputCP(codePage);
#endif

  return 0;
}
