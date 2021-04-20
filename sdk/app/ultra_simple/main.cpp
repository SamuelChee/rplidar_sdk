/*
 *  RPLIDAR
 *  Ultra Simple Data Grabber Demo App
 *
 *  Copyright (c) 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *  Copyright (c) 2014 - 2019 Shanghai Slamtec Co., Ltd.
 *  http://www.slamtec.com
 *
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "Circle.hpp"
#include "CircleFitByKasa.hpp"
#include "CircleFitByLevenbergMarquardtFull.hpp"
#include "LiDAR.hpp"
#include "PointArray.hpp"
#include "fliter.hpp"
#include "serialib.h"

#include <signal.h>
bool ctrl_c_pressed;
void ctrlc(int) { ctrl_c_pressed = true; }

int main(int argc, const char *argv[]) {
  serialib serial;
  PointArray<double> arr;

  const char *opt_com_path = NULL;
  bool useArgcBaudrate = false;

  printf("Ultra simple LIDAR data grabber for RPLIDAR.\n"
         "Version: " RPLIDAR_SDK_VERSION "\n");

  // read serial port from the command line...
  if (argc > 1)
    opt_com_path = argv[1]; // or set to a fixed value: e.g. "com3"

  if (!opt_com_path) {
#ifdef _WIN32
    // use default com port
    opt_com_path = "\\\\.\\com57";
#elif __APPLE__
    opt_com_path = "/dev/tty.usbserial-0001";
#else
    opt_com_path = "/dev/ttyUSB0";
#endif
  }

  RPLiDAR lidar = RPLiDAR(DRIVER_TYPE_SERIALPORT, opt_com_path);

  signal(SIGINT, ctrlc);

  // start scan...
  lidar.drv->startMotor();
  lidar.drv->startScan(0, 1);

  // fetech result and print it out...
  u_result success;
  while (1) {
#define PI (3.1415926)
    rplidar_response_measurement_node_hq_t nodes[8192];
    PointArray<double> arr;

    size_t count = _countof(nodes);

    success = lidar.drv->grabScanDataHq(nodes, count);
    if (IS_OK(success)) {
      lidar.drv->ascendScanData(nodes, count);
      for (int pos = 0; pos < (int)count; ++pos) {

        double dist = nodes[pos].dist_mm_q2 / 4.0f;

        double angle = nodes[pos].angle_z_q14 * 90.f / (1 << 14);

        double r = angle / 360 * 2 * PI;

        // printf("angle: %f, rad: %f, dist: %f ", angle, r, dist);

        PolarVector v = PolarVector(r, dist);

        // cout << "get a polar vector: " << v.toString() << endl;

        Point<double> p = Point<double>::fromPolar(PolarVector(r, dist));

        // cout << "Converted to point: " << p.toString() << endl << endl;
        if (angle > 87 && angle < 93 && dist > 100) {
          arr.push_back(p);
        }

        if (angle > 180) {
          break;
        }
      }

      PointArray<double> flitered = arr;

      // flitered = Fliter::removeInvalidData(flitered);
      cout << "flitered length: " << flitered.size() << endl;
      flitered.print();

      cout << "approximation by CircleFitByKasa:" << endl;
      Circle c = CircleFittingModel::kasaFitting(flitered);

      c.print();

      cout << "approximation by CircleFitByLevenbergMarquardtFull:" << endl;
      CircleFittingModel::levenbergMarquardtFullFitting(flitered, c, 0.001, c);

      c.print();
    }

    if (ctrl_c_pressed) {
      break;
    }
  }

  return 0;
}
