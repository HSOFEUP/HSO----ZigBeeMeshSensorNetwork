/*
 * Serial port utility routines.
 *
 * 
 */

#include <termios.h>


// VELOCIDADE DE TRANSMISSÃO (bits/s)

// #define DEF_BAUDRATE             B0		/* 0 bits/s */
// #define DEF_BAUDRATE             B50		/* 50 bits/s */
// #define DEF_BAUDRATE             B75		/* 75 bits/s */
// #define DEF_BAUDRATE             B110	/* 110 bits/s */
// #define DEF_BAUDRATE             B134	/* 134 bits/s */
// #define DEF_BAUDRATE             B150	/* 150 bits/s */
// #define DEF_BAUDRATE             B200	/* 200 bits/s */
// #define DEF_BAUDRATE             B300	/* 300 bits/s */
// #define DEF_BAUDRATE             B600	/* 600 bits/s */
// #define DEF_BAUDRATE             B1200	/* 1200 bits/s */
// #define DEF_BAUDRATE             B1800	/* 1800 bits/s */
// #define DEF_BAUDRATE             B2400	/* 2400 bits/s */
// #define DEF_BAUDRATE             B4800	/* 4800 bits/s */
#define DEF_BAUDRATE             B9600	/* 9600 bits/s */
// #define DEF_BAUDRATE             B19200	/* 19200 bits/s */
// #define DEF_BAUDRATE             B38400	/* 38400 bits/s */
// #define DEF_BAUDRATE             B57600	/* 57600 bits/s */
// #define DEF_BAUDRATE             B115200	/* 115200 bits/s */
// #define DEF_BAUDRATE             B230400	/* 230400 bits/s */


// TAMANHO DO CARACTER TRANSMITIDO (bits)

// #define DEF_CHAR_SIZE            CS5		/* 5 bits */
// #define DEF_CHAR_SIZE            CS6		/* 6 bits */
// #define DEF_CHAR_SIZE            CS7		/* 7 bits */
#define DEF_CHAR_SIZE               CS8		/* 8 bits */


// NÚMERO DE STOP BITS

#define DEF_STOP_BITS               0               /* 1 stop bits */
//#define DEF_STOP_BITS             CSTOPB          /* 2 stop bits */


// TIPO DE PARIDADE

 #define DEF_PARITY               0               /* sem paridade   */
// #define DEF_PARITY               PARENB          /* paridade par   */
// #define DEF_PARITY               PARENB|PARODD   /* paridade impar */


/* open a serial port. */
/* serialdevice: /dev/ttyS0
 *               /dev/ttyS1
 *               etc...
 *
 * returns: fd  (on success)
 *          -1  (on failure)
 */
int serial_open(const char *serialdevice);

int serial_close(int fd);

int serial_config(int fd, tcflag_t baudrate, tcflag_t charsize, tcflag_t stopbits, tcflag_t parity);

/* Blocking read from the serial port. */
/* Returns when bufsize characters have been read, or timeout occurs,
 * whichever happens first.
 *
 * Returns number of characters read.
 * Timeout in seconds
 */
int serial_read(int fd, unsigned char *buf, int bufsize, int timeout);

int serial_write(int fd, unsigned char *buf, int bufsize);
