#pragma once
#include "Point.hpp"
#include <cmath>
#include <string>

class PolarVector {
public:
  double rad;
  double mag;

  PolarVector() {
    rad = .0;
    mag = .0;
  }

  PolarVector(const double r, const double m) {
    rad = r;
    mag = m;
  }

  static PolarVector fromCartesian(const Point<double> &p) {
    return PolarVector(sqrt(p.x * p.x + p.y * p.y), atan(p.y / p.x));
  }

  void toCartesian(double &x, double &y) {
    x = cos(rad) * mag;
    y = sin(rad) * mag;
  }

  std::string toString() {
    return std::string("angle: ") + std::to_string(rad / (2 * 3.1415926) * 360) + std::string(", mag: ") + std::to_string(mag);
  }
};

template <typename T> Point<T> Point<T>::fromPolar(const PolarVector &p) {
  return Point<T>(cos(p.rad) * p.mag, sin(p.rad) * p.mag);
}