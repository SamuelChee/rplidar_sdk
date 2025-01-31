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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Circle.hpp"
#include "CircleFitByKasa.hpp"
#include "CircleFitByLevenbergMarquardtFull.hpp"
#include "PointArray.hpp"
#include "fliter.hpp"
#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifdef _WIN32
#include <Windows.h>
#define delay(x) ::Sleep(x)
#else
#include <unistd.h>
using namespace std;

static inline void delay(_word_size_t ms) {
  while (ms >= 1000) {
    usleep(1000 * 1000);
    ms -= 1000;
  };
  if (ms != 0)
    usleep(ms * 1000);
}
#endif

using namespace rp::standalone::rplidar;

bool checkRPLIDARHealth(RPlidarDriver *drv) {
  u_result op_result;
  rplidar_response_device_health_t healthinfo;

  op_result = drv->getHealth(healthinfo);
  if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge
                          // whether the operation is succeed.
    printf("RPLidar health status : %d\n", healthinfo.status);
    if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
      fprintf(stderr, "Error, rplidar internal error detected. Please reboot "
                      "the device to retry.\n");
      // enable the following code if you want rplidar to be reboot by software
      // drv->reset();
      return false;
    } else {
      return true;
    }

  } else {
    fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n",
            op_result);
    return false;
  }
}

#include <signal.h>
bool ctrl_c_pressed;
void ctrlc(int) { ctrl_c_pressed = true; }

int main(int argc, const char *argv[]) {
  PointArray<double> arr;

  const char *opt_com_path = NULL;
  _u32 baudrateArray[2] = {115200, 256000};
  _u32 opt_com_baudrate = 0;
  u_result op_result;

  bool useArgcBaudrate = false;

  printf("Ultra simple LIDAR data grabber for RPLIDAR.\n"
         "Version: " RPLIDAR_SDK_VERSION "\n");

  // read serial port from the command line...
  if (argc > 1)
    opt_com_path = argv[1]; // or set to a fixed value: e.g. "com3"

  // read baud rate from the command line if specified...
  if (argc > 2) {
    opt_com_baudrate = strtoul(argv[2], NULL, 10);
    useArgcBaudrate = true;
  }

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

  // create the driver instance
  RPlidarDriver *drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
  if (!drv) {
    fprintf(stderr, "insufficent memory, exit\n");
    exit(-2);
  }

  rplidar_response_device_info_t devinfo;
  bool connectSuccess = false;
  // make connection...
  if (useArgcBaudrate) {
    if (!drv)
      drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (IS_OK(drv->connect(opt_com_path, opt_com_baudrate))) {
      op_result = drv->getDeviceInfo(devinfo);

      if (IS_OK(op_result)) {
        connectSuccess = true;
      } else {
        delete drv;
        drv = NULL;
      }
    }
  } else {
    size_t baudRateArraySize =
        (sizeof(baudrateArray)) / (sizeof(baudrateArray[0]));
    for (size_t i = 0; i < baudRateArraySize; ++i) {
      if (!drv)
        drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
      if (IS_OK(drv->connect(opt_com_path, baudrateArray[i]))) {
        op_result = drv->getDeviceInfo(devinfo);

        if (IS_OK(op_result)) {
          connectSuccess = true;
          break;
        } else {
          delete drv;
          drv = NULL;
        }
      }
    }
  }
  if (!connectSuccess) {

    fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n",
            opt_com_path);
    goto on_finished;
  }

  // print out the device serial number, firmware and hardware version number..
  printf("RPLIDAR S/N: ");
  for (int pos = 0; pos < 16; ++pos) {
    printf("%02X", devinfo.serialnum[pos]);
  }

  printf("\n"
         "Firmware Ver: %d.%02d\n"
         "Hardware Rev: %d\n",
         devinfo.firmware_version >> 8, devinfo.firmware_version & 0xFF,
         (int)devinfo.hardware_version);

  // check health...
  if (!checkRPLIDARHealth(drv)) {
    goto on_finished;
  }

  signal(SIGINT, ctrlc);

  drv->startMotor();
  // start scan...
  drv->startScan(0, 1);

  // fetech result and print it out...
  cout << "angle,distance" << endl;
  while (1) {
#define PI (3.1415926)
    rplidar_response_measurement_node_hq_t nodes[8192];
    PointArray<double> arr;

    size_t count = _countof(nodes);

    op_result = drv->grabScanDataHq(nodes, count);
    if (IS_OK(op_result)) {
      drv->ascendScanData(nodes, count);
      for (int pos = 0; pos < (int)count; ++pos) {

        double dist = nodes[pos].dist_mm_q2 / 4.0f;

        double angle = nodes[pos].angle_z_q14 * 90.f / (1 << 14);

        double r = angle / 360 * 2 * PI;

        PolarVector v = PolarVector(r, dist);

        // cout << "get a polar vector: " << v.toString() << endl;

        Point<double> p = Point<double>::fromPolar(PolarVector(r, dist));

        // cout << "Converted to point: " << p.toString() << endl << endl;
        if (angle > 70 && angle < 150) {
          printf("%f,%f\n", angle, dist);
          arr.push_back(p);
        }

        if (angle > 180) {
          break;
        }
      }

      vector<int> minIndex = Fliter::localMinimum(arr, 0.01);
      PointArray<double> flitered = arr;

      cout << "flitered data:" << endl;
      cout << "angle,distance" << endl;

      for (int i : minIndex) {
        PolarVector pv = PolarVector::fromCartesian(arr[i]);
        printf("%d: %f,%f\n", i, pv.rad * 2 / 360 * PI, pv.mag);
      }

      break;
      // break;

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

  drv->stop();
  drv->stopMotor();
  // done!
on_finished:
  RPlidarDriver::DisposeDriver(drv);
  drv = NULL;
  return 0;
}
