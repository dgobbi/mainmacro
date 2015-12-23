## A main() function for cross-platform command-line programs.

This package solves two of the difficulties with porting UNIX
command-line programs to Windows:

1.  It allows the use of unicode arguments via wmain().
2.  It automatically expands the wildcards * and ?.

The way that it works is simple.  Include the "mainmacro.h" header file,
and then use MAINMACRO() instead of main(), as shown in this example:

    #include "mainmacro.h"
    
    MAINMACRO(argc, argv)
    {
      // do stuff
      return 0;
    }

The arguments received will be encoded as utf-8 (having been converted
from the Windows-native utf-16).

Wildcard expansion will not automatically catch files that begin with ".".
So if a directory contains these files:

    .gitignore
    file1.txt
    file2.txt

Then the "dot" file will be skipped if the command is called like this:

    C:\> example *
    file1.txt
    file2.txt

But it will not be skipped if the command is called like this:

    C:\> example .*
    .gitignore
