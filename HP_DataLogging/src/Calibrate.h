
#ifndef _CALIBRATE_H
#define _CALIBRATE_H

#include "Data.h"
#include "HP34401.h"
#include "E3648.h"
#include "./Util/Write_Log.h"
#include "./Menu.h"
#include "Cal_Parameters.h"
#include <ctime>




void Prompt_User_Message(char* message);
void Start_Logging(HP34401* HP_A, HP34401* HP_B, PowerSupply* E3648A, Write_Log* FileWrite);


void Start_Logging(HP34401* HP_A, HP34401* HP_B, PowerSupply* E3648A, Write_Log* FileWrite)
{

    printf(COLOR_LIGHT_MAGENTA "\t## Datalog HP Meters  ##\n\n" COLOR_RESET);


    HP_A->Display_Text("Record V");
    //HP34401_Display_Text(HP34401B, "-------");
    HP_A->Configure_Volts_DC();                       // defined in "HP_34401.h"
    usleep(ONE_MILLISECOND);

    Set_Instrument1(E3648A);     // sets "Instrument1" as the active instrument to receive commands
    Set_Volts(E3648A, 3.00);     // start with output voltage of 0
    Set_Output_ON(E3648A);
    Set_Current(E3648A, 1.00);   // set current limit to only 25mA because we arent driving anything

    bool output_on = true;

    Data_Group* HP_cool_V;
    printf("\nSTARTING LOOP....\n");
    using namespace std;
    clock_t begin, end;
    for( int i=0; i< 20; ++i)
    {
        if(i>1 && output_on)
        {
            printf(COLOR_RED "-PS OFF-\n" COLOR_RESET);
            begin = clock();
            Set_Output_OFF(E3648A);
            output_on = false;
        }

        end = clock();
        HP_cool_V   = HP_A->Retrieve_Measurements(); // measure actual cooler voltage with HP34401_B
        
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        strcpy(HP_cool_V ->description, "Vout");

        if(elapsed_secs > 0)
        {
            elapsed_secs *= 1000;
            printf("elaps time secs: %f\n",elapsed_secs );
        }

        Data::Print_DataGroup(HP_cool_V);

        // Record Data
        //FileWrite->Write_Volt_Data(HP_cool_V, logfile_t::Cooler_V);

        Data::Free_Data_Group_Memory(HP_cool_V);
        printf("------------------------------------------------------------------\n");
    }

    printf(COLOR_BRIGHT_MAGENTA "DONE.\nTURNING PS OFF\n" COLOR_RESET);
    Set_Volts(E3648A, 0.00);     // set output voltage of power supply to 0 
    Set_Output_OFF(E3648A);     // turn output of power supply off
}



#ifdef __cplusplus
}
#endif

#endif
