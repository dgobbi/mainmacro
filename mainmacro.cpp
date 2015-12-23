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

#include "mainmacro.h"

#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

// A struct to arguments converted from UTF-16 to UTF-8.
#ifdef _WIN32
namespace {

class Arguments
{
public:
  // Construct with an empty argument list.
  Arguments() : m_Argc(0), m_Argv(0) {}

  // Destruct by freeing the memory.
  ~Arguments() {
    if (m_Argv) {
      delete [] *m_Argv;
      delete [] m_Argv;
    }
  }

  // Allocate space for the next arg (of size n, including null).
  char *Next(int n);

  // Add the next arg.
  void Push(wchar_t *arg);

  // Get the argc and argv for the args.
  int GetArgc() { return m_Argc; }
  char **GetArgv() { return m_Argv; }

private:
  // Get the power of two equal or greater than n.
  static int NearestPowerOfTwo(int n) {
    unsigned int m = ((n > 0) ? n - 1 : 0);
    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;
    m |= m >> 16;
    return static_cast<int>(m + 1);
  }

  // Check if n is a power of two.
  static bool IsPowerOfTwo(int n) {
    return ((n & (n - 1)) == 0);
  }

  int m_Argc;
  char **m_Argv;
};

char *Arguments::Next(int n)
{
  m_Argc++;
  if (m_Argv == 0) {
    int m = NearestPowerOfTwo(n);
    m_Argv = new char *[2];
    m_Argv[0] = new char[m];
  }
  else {
    if (IsPowerOfTwo(m_Argc)) {
      char **argv = new char *[m_Argc*2];
      for (int i = 0; i < m_Argc; i++) {
        argv[i] = m_Argv[i];
      }
      delete [] m_Argv;
      m_Argv = argv;
    }
    int m = static_cast<int>(m_Argv[m_Argc-1] - m_Argv[0]);
    if (m + n > NearestPowerOfTwo(m)) {
      char *cp = new char[NearestPowerOfTwo(m + n)];
      char *oldcp = m_Argv[0];
      for (int j = 0; j < m; j++) {
        cp[j] = oldcp[j];
      }
      for (int i = 0; i < m_Argc; i++) {
        m_Argv[i] = cp + (m_Argv[i] - oldcp);
      }
      delete [] oldcp;
    }
  }

  m_Argv[m_Argc] = m_Argv[m_Argc-1] + n;
  return m_Argv[m_Argc-1];
}

void Arguments::Push(wchar_t *arg)
{
  int n = WideCharToMultiByte(
    CP_UTF8, 0, arg, -1, NULL, 0, NULL, NULL);
  char *cp = this->Next(n);
  WideCharToMultiByte(
    CP_UTF8, 0, arg, -1, cp, n, NULL, NULL);
}

} // namespace

// A static variable to hold the arguments (will destruct on exit).
static Arguments mainmacro_arguments;
#endif /* _WIN32 */

// The function, with special definition for Windows.
#ifdef _WIN32
bool mainmacro_expandargs(
  int argc, wchar_t *argv[],
  int *argc_p, char ***argv_p)
{
  WIN32_FIND_DATAW data;

  for (int i = 0; i < argc; i++)
    {
    // check for wildcards
    bool has_wildcard = false;
    wchar_t *cp = argv[i];
    wchar_t *dp = cp;
    while (*cp != 0) {
      if (*cp == '\?' || *cp == '*') {
        has_wildcard = true;
      }
      if (*cp == '\\' || *cp == '/') {
        dp = cp + 1;
        has_wildcard = false;
      }
      cp++;
    }

    if (has_wildcard)
      {
      wchar_t *temp = 0;
      size_t tempsize = 0;
      HANDLE hFile = FindFirstFileW(argv[i], &data);
      if (hFile != INVALID_HANDLE_VALUE) {
        do {
          if (data.cFileName[0] == '.' &&
              (data.cFileName[1] == 0 ||
               (data.cFileName[1] == '.' && data.cFileName[2] == 0))) {
            // The found file was '.' or '..'
            continue;
          }
          else if (data.cFileName[0] == '.' && dp[0] != '.') {
            // The found file began with '.' but the pattern file did not.
            continue;
          }
          else if (dp == argv[i]) {
            // Path does not contain a directory.
            mainmacro_arguments.Push(data.cFileName);
          }
          else {
            // Append wildcard result to the directory.
            cp = argv[i];
            size_t l = dp - cp;
            size_t k = 0;
            wchar_t *ep = data.cFileName;
            while (ep[k] != 0) {
              k++;
            }
            if (l+k+1 > tempsize) {
              tempsize = l+k+9;
              delete [] temp;
              temp = new wchar_t[tempsize];
            }
            for (size_t j = 0; j < l; j++) {
              temp[j] = cp[j];
            }
            for (size_t j = 0; j < k; j++) {
              temp[l+j] = ep[j];
            }
            temp[l+k] = 0;
            mainmacro_arguments.Push(temp);
          }
        }
        while (FindNextFileW(hFile, &data));
        FindClose(hFile);
      }
      else {
        mainmacro_arguments.Push(argv[i]);
      }
      delete [] temp;
    }
    else {
      mainmacro_arguments.Push(argv[i]);
    }
  }

  *argc_p = mainmacro_arguments.GetArgc();
  *argv_p = mainmacro_arguments.GetArgv();

  return true;
}
#else
bool mainmacro_expandargs(int *, char ***)
{
  return true;
}
#endif
