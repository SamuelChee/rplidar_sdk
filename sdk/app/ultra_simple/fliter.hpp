#include "PointArray.hpp"
#include "PolarVector.hpp"
#include <vector>
using namespace std;

namespace Fliter {
template <typename T>
void withinRange(PointArray<T> &arr, const double min, const double max) {
  for (Point<T> &p : arr) {
    const PolarVector v = PolarVector::fromCartesian(p);
    if (v.mag > max || v.mag < min) {
      p.x = 0;
      p.y = 0;
    }
  }
}

template <typename T>
PointArray<T> withinRad(PointArray<T> &arr, const double min,
                        const double max) {
  PointArray<T> ret;
  for (Point<T> &p : arr) {
    const PolarVector v = PolarVector::fromCartesian(p);
    printf("\t v.rad(%f) > max(%f) || v.rad(%f) < min(%f)\n", v.rad, max, v.rad, min);
    if (v.rad > max || v.rad < min) {
      p.x = 0;
      p.y = 0;
    } else {
      ret.push_back(p);
    }
  }

  return ret;
}
template<typename T>
PointArray<T> removeInvalidData(const PointArray<T> &arr) {
    PointArray<T> ret;


    for (const Point<T>& p: arr) {
      if (PolarVector::fromCartesian(p).mag == 0)
        continue;

      ret.push_back(p);
    }

    return ret;
}

template <typename T>
vector<int> localMinimum(const PointArray<T> &arr, const double halfRad) {
  vector<int> min;
  double lastFoundRad = .0;
  bool downward = false;
  bool upward = false;

  for (const Point<T> &p : arr) {
    const PolarVector v = PolarVector::fromCartesian(p);
    if (v.rad <= lastFoundRad)
      continue;
  }

  return min;
}
} // namespace Fliter
