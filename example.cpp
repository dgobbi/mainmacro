//
// A simple example of mainmacro
//

#include "mainmacro.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>

// This is an example of marking options for passthrough (wildcards will
// not be expanded in the arg that follows these options).
MAINMACRO_PASSTHROUGH(-type -stretch);

int MAINMACRO(int argc, char *argv[])
{
#if defined(_WIN32)
  // Temporarily switch the console to utf-8
  UINT codePage = GetConsoleOutputCP();
  SetConsoleOutputCP(65001);
#endif

  // Use printf (cout doesn't respect SetConsoleOutputCP).
  printf("Program: %s\n", argv[0]);
  for (int i = 1; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

#if defined(_WIN32)
  // Restore the console code page
  SetConsoleOutputCP(codePage);
#endif

  return 0;
}
