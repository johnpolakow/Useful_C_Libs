#ifndef MENU_H
#define MENU_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>

#include "StringH.h"
#include "Struct_Defs.h"
#include "colors.h"
#include "HP34401.h"
#include "E3648.h"


static const int NEW_LINE  = 10;	       // ASCII integer code for new line
static const int CARRIAGE_RETURN = 13;	// ASCII integer code for carriage return
static const int QUIT      = 113;       // ASCII for 'q'
static const int ENTER_KEY = 10;        // ASCII character code


const static int CAL_COOLER_DCV = 1;
const static int CAL_COOLER_AC_MA = 2;
const static int CAL_REF100 = 3;
const static int CAL_LOAD = 4;
const static int CAL_DIODE = 5;
const static int CAL_EXIT = 6;
const static int SEL_ERROR = 7;
const static int CAL_AUTOMATIC = 11;
const static int CAL_MANUAL = 12;

using namespace std;
class Device
{
public:
    Device(int num, const char* descrip)
    {
        this->description = descrip;
        this->device_num = num;
    }

    Device(){}

    void Set_Device_Num(int num)
    {
        this->device_num = num;
    }

    void Set_Description(char* descrip)
    {
        this->description = descrip;
    }

    int device_num;
    string description;

};


// Data Structure class to hold measurements read from Pi or meters
class Menu
{
public:
        Menu()
        {
            this->devices = (Device**)malloc(sizeof(Device*)*10);
            this->device_index = 0;
            for(int i = 0; i<10; ++i)
            {
                devices[i] = new Device();
            }
        }


        ~Menu()
        {
            //delete this->devices;
            for(int i = 0; i<10; ++i)
            {
                delete devices[i];
            }
            free(devices);

        }

        static void Print_Main_Menu();
        static void Wait_Enter();
        static void Prompt_User_Message(char* message);
        static void Clear_Console_Screen();
        static int Get_Cal_Selection();
        static int User_Sel_Auto_or_Manual();
        static void Print_Mode_Menu();
        //static void Print_Attached_Devices(Device devices[], int max_index);
        void Print_Attached_Devices();
        
        void Configure_Logging_Options(HP34401* HP_A, HP34401* HP_B, E3648* E3648A);

        static void Free_Device_Mem(Device**  devices, int num_to_free);

private:
        static void Read_Console_Line(char* usr_string, int size);
        Device** devices;
        int device_index;

};

#endif
