#include <fcntl.h>  /* File Control Definitions          */
#include <dirent.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>


#include "StringH.h"
#include "colors.h"

class Meter
{
public:
    Meter(int file_descr, const char* dev_path, const char* ID, const char* meter_description, const char* mfr, const char* model)
    {
        // initialize all strings to empty
        StringH::Erase_Num_Chars(this->device_path, 255);
        StringH::Erase_Num_Chars(this->meter_ID, 25);
        StringH::Erase_Num_Chars(this->meter_string, 10);
        StringH::Erase_Num_Chars(this->manufacturer, 30);
        StringH::Erase_Num_Chars(this->model, 20);

        this->file_descriptor = file_descr;
        strncpy(this->device_path, dev_path, 255);
        strncpy(this->meter_ID, ID, 25);
        strncpy(this->meter_string, meter_description, 10);
        strncpy(this->manufacturer, mfr, 30);
        strncpy(this->model, model, 20);

        if(!(file_descr > 0))
        {
            this->file_descriptor = open_serial_port(this->device_path);
            setup_serial_port(this->file_descriptor);
        }
    }

    Meter()
    {
        // initialize all strings to empty
        StringH::Erase_Num_Chars(this->device_path, 255);
        StringH::Erase_Num_Chars(this->meter_ID, 25);
        StringH::Erase_Num_Chars(this->meter_string, 10);
        StringH::Erase_Num_Chars(this->manufacturer, 30);
        StringH::Erase_Num_Chars(this->model, 20);

        this->file_descriptor = -1;
    }

    static void Enable_DTR(int port_FD);     // Enable output flow control pins
    static void Enable_DSR(int port_FD);     // Enable output flow control pins
    static void Enable_RTS(int port_FD);     // Enable output flow control pins
    static void Disable_DTR(int port_FD);
    static void Disable_DSR(int port_FD);     // Enable output flow control pins
    static void Disable_RTS(int port_FD); 

    static int setup_serial_port(int port_FD);
    static int open_serial_port(const char* device);
    static void Print_Modem_Bits(int port_FD);

    int file_descriptor;
    char device_path[255];  // "/dev/ttyUSBXX"
    char meter_ID[25];      // "0,2-1-1" or "0,11-5-2"
    char meter_string[10];  // "HP34401A" or "HP34401B"
    char manufacturer[30];  // HEWLETT-PACKARD
    char model[20];   
};