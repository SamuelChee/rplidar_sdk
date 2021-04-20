#pragma once
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

const double REAL_MAX = numeric_limits<double>::max();
const double REAL_MIN = numeric_limits<double>::min();
const double REAL_EPSILON = numeric_limits<double>::epsilon();

template <typename T> static inline T SQR(T t) { return t * t; };

static double pythag(double a, double b) {
  a = abs(a), b = abs(b);

  if (a > b)
    return a * sqrt(1 + SQR(b / a));
  else
    return (b == 0.0 ? 0.0 : b * sqrt(1 + SQR(a / b)));
}

#ifdef _WIN32
#include <Windows.h>
#define delay(x) ::Sleep(x)
#else
#include <unistd.h>
static inline void delay(unsigned long ms) {
  int i;
  while (ms >= 1000) {
    usleep(1000 * 1000);
    ms -= 1000;
  };
  if (ms != 0)
    usleep(ms * 1000);
}
#endif