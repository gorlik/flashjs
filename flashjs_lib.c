#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#include "flashjs_lib.h"

#define VERSION "0.1.0"

/* vendor and product id */
#define VENDOR_ID    0x15A2
#define PRODUCT_ID   0x0038

#define CMD_PROGRAM 0x61
#define CMD_GET_RESULT 0x6F

#define CTRL_IN		(LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_DEVICE)
#define CTRL_OUT	(LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_DEVICE)

#define BUF_SIZE 20

const static int TIMEOUT=5000; /* timeout in ms */

const struct mcu_info_t mcu_info[] = {
  { "JS8 ", 0xE000, 0xE3FF },
  { "JS16", 0xC000, 0xC3FF },
  { NULL, 0, 0 }
};


struct libusb_device_handle *flashjs_open(void)
{
  libusb_device_handle *devh;
  int r;
  
  devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
  
  if(devh==NULL) return NULL;
  
#ifndef WIN32
  r= libusb_kernel_driver_active(devh, 0);
  if(r) {
    r= libusb_detach_kernel_driver(devh, 0);
    if (r < 0) {
      fprintf(stderr, "cannot detach kernel driver %d\n", r);
      goto out;
    }
  }
#endif
  
  r = libusb_set_configuration(devh, 1);
  if (r < 0) {
    fprintf(stderr, "libusb_set_configuration error %d\n", r);
    goto out;
  }
  
  //  printf("Successfully set usb configuration 1\n");
  r = libusb_claim_interface(devh, 0);
  if (r < 0) {
    fprintf(stderr, "libusb_claim_interface error %d\n", r);
    goto out;
  }
  //  printf("Successfully claimed interface\n");
  
  return devh;
 out:
  libusb_close(devh);
  return NULL;
}


int flashjs_close(struct libusb_device_handle *devh)
{
  if(devh) {
    libusb_release_interface(devh, 0);
    libusb_close(devh);
    return 0;
  }
  return -1;
}

int flashjs_send(libusb_device_handle *devh, int cmd)
{
  return libusb_control_transfer(devh, CTRL_OUT, cmd, 0, 0, 
				 NULL, 0, TIMEOUT);
}

int flashjs_program(libusb_device_handle *devh, unsigned int addr, unsigned char *data, unsigned int len)
{
  return libusb_control_transfer(devh, CTRL_OUT, CMD_PROGRAM, addr, addr+len-1, 
				 data, len, TIMEOUT);
}

int flashjs_status(libusb_device_handle *devh)
{
  unsigned char buf[1];
  int r;
  
  r = libusb_control_transfer(devh, CTRL_IN, CMD_GET_RESULT, 0, 0, 
			      (char *)buf, sizeof(buf), TIMEOUT);
  
  if(r<0) return -1;
  
  return buf[0];
}

int flashjs_mcu_string(libusb_device_handle *devh, unsigned char *s, int len)
{
  return libusb_get_string_descriptor_ascii  	(  devh, 2,s,len);
} 	
