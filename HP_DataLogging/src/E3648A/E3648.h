

#ifndef _E3648A_
#define _E3648A_


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

#include "colors.h"
#include "StringH.h"
#include "Data.h"



#define ONE_MILLISECOND 1000

#define DEBUG false
#define DEBUG_HEX false
#define DEBUG_FLOW false

// 34401A COMMANDS //

#define APPLY_QUERY         "QUERY?"
#define SET_REMOTE          "SYSTEM:REMOTE"
#define SET_LOCAL           "SYSTEM:LOCAL"
#define GET_ID              "*IDN?"
#define RESET               "*RST"
#define READ_ERROR          "SYST:ERR"
#define SET_RS232           "SYST:INT RS232"


#define INSTR_SEL_QUERY     "INST?"
#define SET_INSTRUMENT      "INSTR:SEL"   // INSTR:[SEL] OUTP1

#define SET_VOLTAGE         "VOLT"                  // VOLT XXX
#define SET_CURRENT         "CURR"                  // CURR XXX
#define QUERY_PROG_CURRENT  "CURR?"          // Queries the programmed value of the current
#define QUERY_PROG_VOLT     "VOLT?"          // Queries the programmed value of the current
#define MEAS_CURRENT        "MEAS:CURR?"          // Measures how much current is being output
#define MEAS_VOLT           "MEAS:VOLT?"          // Measures how much current is being output

#define VOLT_STEP           "VOLT:STEP"         // VOLT:STEP 0.1    set step size to 100mV
#define VOLT_RANGE          "VOLT:RANG"         // set to "HIGH" for 1-20V, "LOW" for 1-8V

#define DISPLAY_ON          "DISPLAY ON"        // turn on the display
#define GET_TEXT            "DISP:TEXT?"
#define CLEAR_DISPLAY       "DISPLAY:TEXT:CLEAR"
#define SET_DISPLAY_VI      "DISP:MODE VI"
#define DISPLAY_TEXT        "DISP:"
#define OUTPUT_ON           "OUTP ON"
#define OUTPUT_OFF          "OUTP OFF"

#define SET_AUTOZERO        "SENSE:ZERO:AUTO ON"




class E3648
{
public:
    E3648(int file_descr, const char* devpath, const char* mfr, const char* model_string, const char* ID )
    {
        this->file_descriptor = file_descr;
        strcpy(this->device_path, devpath);
        strcpy(this->manufacturer, mfr);
        strcpy(this->model, model_string);
        strcpy(this->PS_ID, ID);

        control_PS = false;
        this->Set_Remote();
    }

    E3648( )
    {
        this->file_descriptor = -1;
        control_PS = false;
    }


    int open_serial_port(const char* device);
    int setup_serial_port(int port_FD);
    int Send_Command(const char* command_string);
    int Get_Num_Chars_Waiting(int port_FD);
    void Read_Response(char* response_string);
    void Enable_Control(bool setting);

    char* Get_Power_Supply_ID();

    void Print_Modem_Bits(int port_FD);
    void Print_Hex(char* string);
    void Enable_DTR(int port_FD);
    void Enable_DSR(int port_FD);
    void Disable_DTR(int port_FD);
    void Disable_DSR(int port_FD);
    void Enable_RTS(int port_FD);
    void Disable_RTS(int port_FD);

    void Initialize_PowerSupply();
    void Set_Remote();
    void Set_Local();
    void Reset();
    void Display_Text(const char* text);

    void Set_Output_ON();
    void Set_Output_OFF();
    void Set_Volts(double volts);
    void Set_Current(double amps);
    void Set_Volt_Range_HIGH();
    void Set_Volt_Range_LOW();
    void Set_Instrument1();
    void Set_Instrument2();
    float Measure_Volts();
    float Measure_Current();
    int Query_Instrument();
    char* Get_Display_Text();
    float Query_Set_Voltage();
    void Get_Errors();


    int file_descriptor;
    char device_path[255];
    char manufacturer[30];
    char model[20];
    char PS_ID[25];
    char text[15];
    int cur_instrument;
    bool ON;
    float instr1_volt;
    float instr2_volt;
    bool control_PS;
};



#endif