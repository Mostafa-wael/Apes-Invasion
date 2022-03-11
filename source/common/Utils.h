#pragma once
#include <memory>
#include <stdexcept>
#include <string>

using std::string;

namespace our {

// Function to format strings like C's printf
// But for C++'s std::string
// Don't call it in performance intensive places like render loops
// since it dynamically allocates memory
// source:
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template <typename... Args>
string FormatString(const string &format, Args... args) {
  int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) +
               1; // Extra space for '\0'
  if (size_s <= 0) {
    return "ERROR DURING STRING FORMATTING";
  }
  auto size = static_cast<size_t>(size_s);
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}
} // namespace our