#include "rplidar.h"
#include <string>

using namespace std;

using namespace rp::standalone::rplidar;

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

class RPLiDAR {
public:
  RPlidarDriver *drv = nullptr;
  rplidar_response_device_info_t devinfo;
  RPLiDAR() {}

  RPLiDAR(unsigned driverType, string serial) {
    drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (!drv) {
      fprintf(stderr, "insufficent memory\n");
    }

    // make connection...
    const char *opt_com_path = NULL;
    bool connectSuccess = false;
    const unsigned baudRates[] = {115200, 256000};
    u_result op_result;

    for (size_t i = 0; i < 2; ++i) {
      if (!drv)
        drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
      if (IS_OK(drv->connect(serial.c_str(), baudRates[i]))) {
        op_result = drv->getDeviceInfo(devinfo);

        if (IS_OK(op_result)) {
          connectSuccess = true;
          break;
        } else {
          delete drv;
          drv = NULL;
        }
      }
      if (!connectSuccess) {
        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n",
                opt_com_path);
      }
    }
  }

  bool driverCreated() { return drv != nullptr; }

public:
  bool getHealth(RPlidarDriver *drv) {
    u_result op_result;
    rplidar_response_device_health_t healthinfo;

    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge
                            // whether the operation is succeed.
      printf("RPLidar health status : %d\n", healthinfo.status);
      if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
        fprintf(stderr, "Error, rplidar internal error detected. Please reboot "
                        "the device to retry.\n");
        // enable the following code if you want rplidar to be reboot by
        // software drv->reset();
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

  void printDeviceInfo() {
    // print out the device serial number, firmware and hardware version
    // number..
    printf("RPLIDAR S/N: ");
    for (int pos = 0; pos < 16; ++pos) {
      printf("%02X", devinfo.serialnum[pos]);
    }

    printf("\n"
           "Firmware Ver: %d.%02d\n"
           "Hardware Rev: %d\n",
           devinfo.firmware_version >> 8, devinfo.firmware_version & 0xFF,
           (int)devinfo.hardware_version);
  }

  ~RPLiDAR() {
    if (drv) {
      drv->stop();
      drv->stopMotor();
    }
    RPlidarDriver::DisposeDriver(drv);
    delete drv;
    drv = NULL;
  }
};