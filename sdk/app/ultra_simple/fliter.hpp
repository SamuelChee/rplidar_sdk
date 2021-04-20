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
    printf("\t v.rad(%f) > max(%f) || v.rad(%f) < min(%f)\n", v.rad, max, v.rad,
           min);
    if (v.rad > max || v.rad < min) {
      p.x = 0;
      p.y = 0;
    } else {
      ret.push_back(p);
    }
  }

  return ret;
}
template <typename T>
PointArray<T> removeInvalidData(const PointArray<T> &arr) {
  PointArray<T> ret;

  for (const Point<T> &p : arr) {
    if (PolarVector::fromCartesian(p).mag == 0)
      continue;

    ret.push_back(p);
  }

  return ret;
}

/**
 * @brief Calculate the first derivative of given point.
 * The first derivative is d(distance)/d(rad).
 *
 * It will look for next point first, if it cannot find a point for calculation,
 * then it will look backwards.
 *
 * @tparam T
 * @param arr
 * @param index
 * @return float
 */
template <typename T>
float firstDerivative(const PointArray<T> &arr, int index) {
  const int size = arr.size();
  const PolarVector pv = PolarVector::fromCartesian(arr[index]);
  for (int i = index + 1; i < size; ++i) {
    const PolarVector next = PolarVector::fromCartesian(arr[i]);
    if (next.mag != 0) {
      return (pv.mag - next.mag) / (pv.rad - next.rad);
    }
  }

  for (int i = index - 1; i >= 0; --i) {
    const PolarVector prev = PolarVector::fromCartesian(arr[i]);
    if (prev.mag != 0) {
      return (pv.mag - prev.mag) / (pv.rad - prev.rad);
    }
  }

  return .0f;
}

template <typename T>
vector<int> localMinimum(const PointArray<T> &arr, const double searchingRad) {
  vector<int> min;

  for (int i = 1; i < arr.size() - 1; ++i) {
    PolarVector pv = PolarVector::fromCartesian(arr[i]);

    if (pv.mag == 0) // invalid data, skip
      continue;

    float dwSlope = .0f; // downward slope
    int numDW = 0;
    float uwSlope = .0f; // upward slope
    int numUW = 0;

    const double curRad = pv.rad;

    int r = i - 1;
    do {
      float d = firstDerivative(arr, r);
      if (!d)
        continue;
      dwSlope += d;
      ++numDW;
    } while (r >= 0 &&
             PolarVector::fromCartesian(arr[r]).rad >= curRad - searchingRad);

    int k = i + 1;

    do {
      float d = firstDerivative(arr, k);
      if (!d)
        continue;
      dwSlope += d;
      ++numDW;
    } while (k < arr.size() &&
             PolarVector::fromCartesian(arr[k]).rad <= curRad + searchingRad);

    if (dwSlope / numDW < 0 && uwSlope / numUW > 0) {
      min.push_back(i);
      i += k;
    }
  }

  return min;
}
} // namespace Fliter
