#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "flashjs_lib.h"

static struct libusb_device_handle *devh = NULL;

static void hexprint(int a, unsigned char *p, int s)
{
  int i;
  printf("%02x : ",a);
  for(i=0;i<s;i++) printf("%02x ",p[i]);
  printf("\n");
}

int char2int(unsigned char c)
{
  if((c>='0')&&(c<='9')) return c-'0';
  else if ((c>='a') && (c<='f')) return c-'a'+10;
  else if ((c>='A') && (c<='F')) return c-'A'+10;
  return 0;
}

int str2bin(unsigned char *str, unsigned char *buf, int size)
{
  int i;
  for(i=0;i<size;i++) {
    buf[i]=(char2int(str[i*2])<<4)+char2int(str[i*2+1]);
  }
}

int program_s19(struct libusb_device_handle *devh, char *fname, int idx)
{
  FILE *fw;
  unsigned char buf[1024];
  unsigned char bin_buf[512];
  int line = 1;
  int size;
  unsigned int addr=0;
  char c;

  fw=fopen(fname,"r");
  if(fw==NULL) {
    printf("can't open file %s\n",fname);
    return -1;
  }

  
  while(fgets(buf,1023,fw)!=NULL) {
    if(buf[0]=='\n'||buf[0]=='\r') continue;
    //    printf("%s",buf);
    if(buf[0]!='S') {
    printf("malformed file at line %d\n",line);
    return -1;
    }
    size=(char2int(buf[2])<<4)+char2int(buf[3]);
    switch(buf[1]) {
    case '0':
      //      printf("header size:%d\n",size-3);
      str2bin(&buf[8],bin_buf,size-3);
      bin_buf[size-3]=0;
      printf("firmware: %s\n",bin_buf);
      break;
    case '1':
      addr=(char2int(buf[4])<<12)+(char2int(buf[5])<<8)+(char2int(buf[6])<<4)+char2int(buf[7]);
      // size is -3 because need to subtract the address size and the checksum
      //      printf("data16 @%04x size:%d\n",addr,size-3);
      str2bin(&buf[8],bin_buf,size-3);
      if( (idx==-1) || (addr<mcu_info[idx].pstart) || (addr>mcu_info[idx].pend)) {
	c='.';
	//		printf("+%d %04X ",idx, addr);
	flashjs_program(devh,addr, bin_buf, size-3);
#ifdef WIN32
	Sleep(2);
#else	
	usleep(2000);
#endif
	if(flashjs_status(devh)!=1) {
	  c='e';
	  printf("error at addr %04x\n",addr);
	}
      } else {
	c='_';
      }
      
      printf("%c",c);
      fflush(stdout);
      break;
      
    case '9':
    case '5':
      break;
      
    default:
      printf("unsupported record type %c at line %d\n",buf[1],line);
    }
    line++;
  }
  printf("\n");

}

int main(int argc, char *argv[])
{
  unsigned char mcu_string[255];
  int r,idx;
  
  r = libusb_init(NULL);
  if (r < 0) {
    fprintf(stderr, "Failed to initialise libusb\n");
    exit(1);
  }
  //    libusb_set_debug(NULL,3);
  
  devh = flashjs_open();
  if (devh==NULL) {
    fprintf(stderr, "Could not find/open JS device\n");
    goto out;
  }

  r=flashjs_mcu_string(devh,mcu_string,254);
  if(r<0) {
    fprintf(stderr, "Failed to retrieve MCU type\n");
    goto out;
  }
  mcu_string[r]=0;

  for(idx=0;mcu_info[idx].name;idx++) {
    if(strcmp(mcu_info[idx].name,mcu_string)==0) break;
  }

  if(mcu_info[idx].name) {
    printf("%s MCU Detected (index %d)\n",mcu_string,idx);
  } else {
    printf("WARNING: %s MCU is not recognized (user flash cannot be automatically preserved)\n",mcu_string);
    idx=-1;
  }

  if(argc==2 && *argv[1]=='e') {
    flashjs_send(devh,CMD_PARTIAL_ERASE);
    if(flashjs_status(devh)==1) printf("Partial erase successfull\n");
      else printf("ERROR: partial erase failed\n");
  } else if(argc==2 && *argv[1]=='m') {
    flashjs_send(devh,CMD_MASS_ERASE);
    if(flashjs_status(devh)==1) printf("Mass erase successfull\n");
      else printf("ERROR: mass erase failed\n");
  } else if(argc==2 && *argv[1]=='c') {
    flashjs_send(devh,CMD_CRC_CHECK);
    if(flashjs_status(devh)==1) printf("CRC check OK\n");
      else printf("ERROR: CRC check failed\n");
  } else if(argc==2 && *argv[1]=='x') {
    flashjs_send(devh,CMD_RESET);
  } else if(argc==3 && *argv[1]=='p') {
    program_s19(devh,argv[2],-1);
  } else if(argc==3 && *argv[1]=='u') {
    if(idx==-1)
      printf("upgrade is not possible for unrecognized MCUs\n");
    else 
      program_s19(devh,argv[2],idx);
  } else {
    printf("usage:\n");
    printf("flashjs e           partial erase\n");
    printf("flashjs m           mass erase\n");
    printf("flashjs c           CRC check\n");
    printf("flashjs x           reboot\n");
    printf("flashjs p file.s19  program fw with file\n");
    printf("flashjs u file.s19  upgrade fw with file (preserves user flash)\n");
  }

out:
  flashjs_close(devh);
  libusb_exit(NULL);
  return 0;
}
