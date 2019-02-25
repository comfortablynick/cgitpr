// Generic utility functions
#include "Utils.hpp"
#include <cstdarg>
#include <sstream>

const std::string Run(std::string cmd) {
  std::string data;
  FILE *stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL)
        data.append(buffer);
    pclose(stream);
  }
  return data;
}

std::vector<std::string> split(const std::string &str, char delim = ' ') {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);
  while (std::getline(tokenStream, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}

const std::string vformat(const char *const zcFormat, ...) {

  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen);
}
