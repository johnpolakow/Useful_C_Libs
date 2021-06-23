


#ifndef _HP34401_
#define _HP34401_

#include <ctime>
#include "Meter.h"
#include "Data.h"

#define ONE_MILLISECOND 1000
#define MAX_DESCRIPTION 15


#define SET_REMOTE          "SYSTEM:REMOTE"
#define SET_LOCAL           "SYSTEM:LOCAL"
#define GET_ID              "*IDN?"
#define RESET               "*RST"
#define SET_AVERAGE         "CALC:FUNCTION AVERAGE"
#define FUNCTION_ON         "CALC:STATE ON"
#define FUNCTION_OFF        "CALC:STATE OFF"

#define GET_AVERAGE         "CALCULATE:AVERAGE:AVERAGE?"
#define GET_TRIGGER_SOURCE  "TRIGGER:SOURCE?"
#define GET_AVG_COUNT       "CALCULATE:AVERAGE:COUNT?"  // gets number of samples used for the average
#define GET_TRIGGER_COUNT   "TRIGGER:COUNT?"            // gets number of triggers to issue before returning to idle
#define GET_SAMPLE_COUNT    "SAMPLE:COUNT?"             // gets number of samples to take per trigger

#define SET_TRIGGER_IMM     "TRIGGER:SOURCE IMMEDIATE"
#define DISPLAY_ON          "DISPLAY ON"        // turn on the display
#define CLEAR_DISPLAY       "DISPLAY:TEXT:CLEAR"
#define INIT                "INIT"
#define FETCH               "FETCH?"
#define SET_VOLTS_DC        "CONFIGURE:VOLTAGE:DC"
#define SET_CURRENT_DC      "CONFIGURE:CURRENT:DC"
#define SET_VOLTS_AC        "CONFIGURE:VOLTAGE:AC"
#define SET_CURRENT_AC      "CONFIGURE:CURRENT:AC"
#define SET_AUTOZERO        "SENSE:ZERO:AUTO ON"
#define SET_BEEP_OFF        "SYST:BEEP:STATE OFF"
#define GET_ERRORS          "SYST:ERR?"


typedef enum meas_t{  AC_VOLTS, AC_AMPS, DC_VOLTS, DC_AMPS } meas_t;


class HP34401
{
public:
    HP34401(Meter* dmm)
    {
        this->HP_meter = dmm;
        this->meter_data = NULL;
        this->datalog = false;          // set logging to false unless specified by consumer

        // set default values for what to measure with this meter
        this->measurement_type = DC_VOLTS;  // default value
        this->trigger_count = 1;            // default value
        this->samples_per_trigger = 1;       // default value
        strcpy(this->units, "V");
        //this->Reset();
        //usleep(ONE_MILLISECOND*50);
        //Send_Command(SET_BEEP_OFF);
        //usleep(ONE_MILLISECOND*50);
        t_offset_ACA = 0;
        t_offset_ACV = 0;
        t_offset_DCV = 0;
        t_offset_DCA = 0;
        

    }


    // Function Declarations
    static int Get_Num_Chars_Waiting(int port_FD);
    static void Print_Characters_Hex(char* string);

    void Display_Text(const char* text);
    float Measure_Volts_DC();
    float Measure_Current_DC();

    void Enable_Logging();
    void Disable_Logging();
    void Set_Num_Samples_Per_Trigger(int num_samples);
    void Set_Num_Triggers(int num_triggers);
    void Set_Measurement_Type(meas_t meas_type);
    meas_t Get_Measurement_Type();
    char* Get_Description();
    char* Get_Units();
    void Set_Description(const char* descrip);
    void Configure_For_Measurements();
    void Print_Meter_Configs();
    void Set_Remote();
    void Send_Trigger();
    void Retrieve_Measurements();
    int Send_Command(const char* command_string);
    char* Get_Meter_ID();
    char* Get_Errors();
    void Identify_Meter();
    void Display_Measurement();

    bool datalog;
    Meter* HP_meter;
    char units[10];
    Data_Group* meter_data;

private:
    void Read_Response(char* response_string);
    Data_Group* Parse_HP_Line(char* line_string);

    static int parse_USB_Number(char* path);
    void Reset();
    void Configure_Num_Samples_Per_Trigger(int num_samples);
    void Configure_Num_Triggers(int num_triggers);
    void Configure_Volts_DC();
    void Configure_Current_DC();
    void Configure_Volts_AC();
    void Configure_Current_AC();
    void Calc_Wait_Before_Reading();
    int Calc_Milliseconds_Waited();


    char text[15];
    int trigger_count;
    int samples_per_trigger;
    meas_t measurement_type;
    int ms_wait_before_fetch;
    clock_t begin_fetch_wait;
    char description[MAX_DESCRIPTION];
    int t_offset_ACA;
    int t_offset_ACV;
    int t_offset_DCV;
    int t_offset_DCA;

};



#endif