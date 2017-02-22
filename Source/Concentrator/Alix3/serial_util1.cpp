/*
 * Serial port utility routines.
 *
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>

/* select() */ /* According to POSIX 1003.1-2001 */
#include <sys/select.h>

/* select() */ /* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>



#include "serial_util1.h"
#include "debug.h"


#define _POSIX_SOURCE 1 /* POSIX compliant source */


/* TODO: the variable oldtio should not be global! */
struct termios oldtio;


/* open a serial port. */
/* serialdevice: /dev/ttyS0
 *               /dev/ttyS1
 *               etc...
 *
 * returns: fd  (on success)
 *          -1  (on failure)
 */
int serial_open(const char *serialdevice)
{
  int fd, res;

  fd = open(serialdevice, O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(serialdevice);
    return(-1);
  }

  res = 0;
  res |= tcgetattr(fd,&oldtio); /* save current port settings */
  res |= serial_config(fd, DEF_BAUDRATE, DEF_CHAR_SIZE, DEF_STOP_BITS, DEF_PARITY);

  if (res < 0)  {
    serial_close(fd);
    return -1;
  }

  return fd;
}



int serial_close(int fd)
{
  int res = 0;

  if (fd < 0)
    return 0;

  res = tcsetattr(fd,TCSANOW,&oldtio);
  res |= close(fd);

  fd = -1;

  return res;
}



/* Configure a serial port alreayd previously opened... */
int serial_config(int fd, tcflag_t baudrate, tcflag_t charsize, tcflag_t stopbits, tcflag_t parity) {
  int res = 0;
  struct termios newtio;

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag |= 0;
  newtio.c_cflag |= baudrate;   /* set the baudrate                       */
  newtio.c_cflag |= charsize;   /* set character size in bits.            */
  newtio.c_cflag |= stopbits;   /* set the number of stop bits.           */
  newtio.c_cflag |= parity;     /* set the parity                         */
//newtio.c_cflag |= CRTSCTS;    /* enable RTS/CTS  flow control on output */
//newtio.c_cflag &= ~CRTSCTS;  	/* disable RTS/CTS  flow control on output */
  newtio.c_cflag |= CLOCAL;     /* ignore modem control lines             */
  newtio.c_cflag |= CREAD;      /* enable receiver                        */

  newtio.c_iflag |= IGNPAR;     /* ignore framing and parity errors       */
//newtio.c_iflag |= INPCK;      /* enable input parity checking           */
//newtio.c_iflag |= IXON;       /* enable XON/XOFF flow control on output */
//newtio.c_iflag |= IXOFF;      /* enable XON/XOFF flow control on input  */
//newtio.c_iflag |= IXANY;      /* enable any character to restart output */

  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

  res |= tcflush(fd, TCIOFLUSH);
  res |= tcsetattr(fd,TCSANOW,&newtio);

  return res;
}


/* returns: bufsize on success (num characters read)
 *          0..(bufsize-1) (num characters read) on timeout
 *          -1 on error
 */
int serial_read(int fd, unsigned char *buf, int bufsize, int timeout)
{
  fd_set readfds;
  struct timeval tv;
  int res, count;

#ifdef DEBUG
  printf("serial_read(): called on fd %d, bufsize %d\n", fd, bufsize);
#endif

  res = 0;
  count = 0;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  for (count = 0; count < bufsize; count += res) {
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    res = select(fd+1, &readfds, NULL, NULL, &tv); // wait for input data
    if (res < 0) {
      if (errno != EINTR) return -1;
      continue;
    }
    if (res == 0) //timeout!
		return count;

    if ((res = read(fd,buf+count,bufsize-count)) <= 0)
      return -1;
  }

#ifdef DEBUG
printf("serial_read(): received %d bytes. ", count);
{int i;
 for(i = 0; ((i < count) && (i < 6)) ; i++)
  printf("[0x%2X]", buf[i]);
 if (i < count) printf("...");
 printf("\n");
}
#endif


  return count;
}



int serial_write(int fd, unsigned char *buf, int bufsize)
{ int res, size = 0;

#ifdef DEBUG
//printf("serial_write(): called on fd %d, bufsize %d\n", fd, bufsize);
printf("serial_write(): sent %d bytes ", bufsize);
{int count;
 for(count = 0; ((count < bufsize) && (count < 6)); count++)
  printf("<0x%2X>", buf[count]);
 if (count < bufsize) printf("...");
 printf("\n");
}
#endif

  while (bufsize > 0) {
    res = write(fd,buf,bufsize);       /* writes to the serial line */
    if (res < 0)
      return -1;
    buf += res;
    bufsize -= res;
    size += res;
  }

#ifdef DEBUG
//printf("serial_write(): returning %d\n", size);
#endif
  return size;
}
