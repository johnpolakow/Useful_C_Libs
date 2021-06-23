





#include "Meter.h"





int Meter::open_serial_port(const char* device)
{
    int port;
    port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    /*  O_RDWR POSIX read write
        O_NOCTTY: this program does not want to be the "controlling terminal"
        O_NDELAY": Ignore DCD signal state. If you dont set this, program will sleep until DCD is at space voltage
   */

  printf(" opened device " COLOR_MAGENTA "%s\n" COLOR_RESET, device);
  if(port == -1)
    { perror(" - Unable to open ttyUSB[x] - \n"); return false; }
  else
    fcntl(port, F_SETFL, 0);
        // fnctl - performs operation on file descriptor specified, "port"
        // F_SETFL  - set file descriptor flags. Can only change these flags: O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK
        // 0 - clear all flags



  return port;
}

int Meter::setup_serial_port(int port_FD)
{
  struct termios SerialPortSettings;                            // declare object to hold the port settings
  memset(&SerialPortSettings, 0, sizeof(SerialPortSettings));  // clear the new struct

  cfsetispeed(&SerialPortSettings, B9600); // set input speed
  cfsetospeed(&SerialPortSettings, B9600); // set output speed
  cfmakeraw(&SerialPortSettings);           // make port raw

  // Control Mode Flags
  SerialPortSettings.c_cflag |= (CLOCAL|CREAD);     // CLOCAL - do not change "owner" of the port.  CREAD - enable the receiver
  SerialPortSettings.c_cflag &= ~CSIZE;             // bit mask for character size. Resets the bit to 0 so we can set it
  SerialPortSettings.c_cflag |=  CS8;               // set the bit for 8 data bits
  SerialPortSettings.c_cflag |=  CSTOPB;            // enable this bit, sets for 2 stop bits to be used. If bit was cleared there would be 1 stop bit
  SerialPortSettings.c_cflag &= ~PARENB;            // clear the parity bit
  //SerialPortSettings.c_cflag |=  CRTSCTS;  //Turn on flow control

  // Local Mode Flags
  SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //Make sure that Canonical input is off (raw data mode).
  // Clears all these flags:
        // ICANON - enable Canonical input (else raw)
        // ECHO - enable echoing of input Characters
        // ECHOE - echo erase characters (backspace, space)
        // ISG - Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals

  // Input Mode Flags
  SerialPortSettings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                                | INLCR | IGNCR | ICRNL | IXON);    //Turn off software flow control

  // Output Mode Flags, filters output
  SerialPortSettings.c_oflag &= ~OPOST; // Clear post processing flag. (use raw output)
  // note, when OPOST is disabled, all other flags are ignored

  // Control Characters
  SerialPortSettings.c_cc[VMIN]  = 0;       // minimum number of characters to read
  SerialPortSettings.c_cc[VTIME] = 2;       // amount of time to wait in tenths of seconds before returning
                                            // If VTIME is set to 0 (the default), reads will block (wait) indefinitely unless the NDELAY option
                                            // is set on the port with open or fcntl. If VTIME > 0, then read blocks until nBytes have been received,
                                            // or the timer expires. See: http://www.unixwiz.net/techtips/termios-vmin-vtime.html

  tcflush(port_FD, TCIOFLUSH); // clear out the input and output buffer

  int result = tcsetattr(port_FD, TCSANOW, &SerialPortSettings); // Set the port settings. TCSANOW == make changes now
  if (result < 0)
        printf("failed to set attr: %d, %s\n", port_FD, strerror(errno));

  Meter::Enable_DTR(port_FD);     // Enable output flow control pins
  Meter::Enable_DSR(port_FD);     // Enable output flow control pins
  Meter::Enable_RTS(port_FD);     // Enable output flow control pins


  return result;
}

void Meter::Print_Modem_Bits(int port_FD)
{
    int status_bits;
    ioctl(port_FD, TIOCMGET, &status_bits);   // gets the status of the modem bits: TIOCM_DTR, TIOCM_RTS, TIOCM_CTS, TIOCM_DSR, ...

    int DTR_flag, CTS_flag, RTS_flag, DSR_flag;

    DTR_flag = status_bits & TIOCM_DTR; // Mask out the specific bit
    DSR_flag = status_bits & TIOCM_DSR;
    CTS_flag = status_bits & TIOCM_CTS;
    RTS_flag = status_bits & TIOCM_RTS;

    printf("--Modem Status Bits--\n");

    if(DTR_flag)    printf("\tDTR bit: SET\n");
    else            printf("\tDTR bit: OFF\n");

    if(DSR_flag)    printf("\tDSR bit: SET\n");
    else            printf("\tDSR bit: OFF\n");

    if(CTS_flag)    printf("\tCTS bit: SET\n");
    else            printf("\tCTS bit: OFF\n");

    if(RTS_flag)    printf("\tRTS bit: SET\n\n");
    else            printf("\tRTS bit: OFF\n\n");

    return;
}


void Meter::Enable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIS, &DTR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void Meter::Disable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIC, &DTR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void Meter::Enable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIS, &DSR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void Meter::Disable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIC, &DSR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void Meter::Enable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIS, &RTS_flag);  //Set RTS pin w/ "TIOCMBIS" command
}

void Meter::Disable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIC,&RTS_flag);   //Clear RTS pin w/ "TIOCMBIC" command
}