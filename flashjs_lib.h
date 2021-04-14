#ifndef DS00_LIB_H
#define DS00_LIB_H

#include <libusb-1.0/libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_MASS_ERASE    0x63
#define CMD_PARTIAL_ERASE 0x64
#define CMD_RESET         0x68
#define CMD_CRC_CHECK     0x69

  struct mcu_info_t {
    char *name;
    unsigned int pstart;
    unsigned int pend;
  };
  
  struct libusb_device_handle *flashjs_open(void);
  int flashjs_close(struct libusb_device_handle *devh);

  int flashjs_send(libusb_device_handle *devh, int cmd);
  int flashjs_program(libusb_device_handle *devh, unsigned int addr, unsigned char *data, unsigned int len);
  int flashjs_status(libusb_device_handle *devh);
  
  int flashjs_mcu_string(libusb_device_handle *devh, unsigned char *s, int len);
  
  extern const struct mcu_info_t mcu_info[];
  
#ifdef __cplusplus
}
#endif

#endif
