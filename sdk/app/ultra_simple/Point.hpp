#pragma once
#include <string>

class PolarVector;

template <typename T> class Point {
public:
  T x;
  T y;

  Point() {
    x = 0;
    y = 0;
  }

  Point(T x, T y) {
    this->x = x;
    this->y = y;
  }

  Point operator+(Point &p) { return Point(this->x + p.x, this->y + p.y); }
  Point operator/(double d) { return Point(this->x / d, this->y / d); }

  static Point fromPolar(const PolarVector &p);

  std::string toString() {
    return std::string("x: ") + std::to_string(x) + ", " + std::string("y: ") +
           std::to_string(y);
  }
};