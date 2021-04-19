//
//						 circle.h
//
/************************************************************************
                        DECLARATION OF THE CLASS CIRCLE
************************************************************************/
// Class for Circle
// A circle has 7 fields:
//     a, b, r (of type double), the circle parameters
//     s (of type double), the estimate of sigma (standard deviation)
//     g (of type double), the norm of the gradient of the objective function
//     i and j (of type int), the iteration counters (outer and inner,
//     respectively)
#pragma once
#include "helper_funcs.hpp"

class Circle {
public:
  // The fields of a Circle
  double a, b, r, s, g, Gx, Gy;
  int i, j;

  // constructors
  Circle();
  Circle(double aa, double bb, double rr);

  // routines
  void print(void);

  // no destructor we didn't allocate memory by hand.
};

/************************************************************************
                        BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor

Circle::Circle() {
  a = 0.;
  b = 0.;
  r = 1.;
  s = 0.;
  i = 0;
  j = 0;
}

// Constructor with assignment of the circle parameters only

Circle::Circle(double aa, double bb, double rr) {
  a = aa;
  b = bb;
  r = rr;
}

// Printing routine

void Circle::print(void) {
  cout << endl;
  cout << setprecision(10) << "center (" << a << "," << b << ")  radius " << r
       << "  sigma " << s << "  gradient " << g << "  iter " << i << "  inner "
       << j << endl;
}
