/*=========================================================================

  Program: mainmacro

  Copyright (c) 2015 David Gobbi

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

  * Neither the name of David Gobbi nor the names of any contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef mainmacro_h
#define mainmacro_h

//! Expand command line arguments for Windows command line.
/*!
 *  This will expand any wildcards in the arguments, and is only applied
 *  on Windows because UNIX shells do wildcard expansion by themselves.
 */
bool mainmacro_expandargs(
  int wargc, wchar_t *wargv[],
  int *argc_p, char ***argv_p);

//! A macro to use wmain on Windows.
#ifdef _WIN32
#define MAINMACRO(argc, argv) \
int main_with_utf8_args(int argc, char *argv[]); \
int wmain(int wargc, wchar_t *wargv[]) \
{ \
  int argc = 0; \
  char **argv = 0; \
  mainmacro_expandargs(wargc, wargv, &argc, &argv); \
  return main_with_utf8_args(argc, argv); \
} \
int main_with_utf8_args(int argc, char *argv[])
#else
#define MAINMACRO(argc, argv) \
int main(int argc, char *argv[])
#endif

#endif /* mainmacro_h */
