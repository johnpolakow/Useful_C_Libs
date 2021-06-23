

#ifndef PARAMETERS_H
#define PARAMETERS_H


#define ONE_MILLISECOND 1000

// These are declarations of global variables defined in Cal_Parameters.cpp
// Definition of Raspberry PI Pin #s are in ./src/PI/Pin_Defs.h
// to compile the project, enter the command "make" in the Cal_Program directory
// default IP address of the Calibration Box is 192.168.123.7, setting is in /etc/dhcpcd.conf 

extern const char* DEFAULT_SERVER_PORT;
extern const char CAL_LOG_DIR[];

extern const char* HP_A_ID;
extern const char* HP_B_ID;
extern const char* AGILENT_PS_ID;

extern double DCV_Set_Points[];
extern int V_POINTS_NUM_ELEMENTS;

extern double Diode_Cal_Points[];
extern int DIODE_POINTS_NUM_ELEMENTS;

extern const int DAC_INCREMENT;

#endif