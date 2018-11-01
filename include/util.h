#ifndef _IUYTE_UTIL_H_
#define _IUYTE_UTIL_H_

#include "main.h"
#include <string>

#define trim(num, min, max) (num > min ? (num < max ? num : max) : min)

std::vector<std::string> split(const std::string &s, char delim);

#endif
