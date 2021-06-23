

#include "DataLog.h"



void DataLog::Setup_Instruments(HP34401* HP_A, HP34401* HP_B, E3648* E3648A)
{
    HP_A->Set_Remote();
    if(HP_A->datalog == true)
    {
        HP_A->Identify_Meter();
        HP_A->Display_Text(HP_A->HP_meter->meter_string);
        printf("HP A meter string: %s\n", HP_A->HP_meter->meter_string);
        HP_A->Configure_For_Measurements();
    }
    else
    {
        HP_A->Display_Text("------");
    }


    HP_B->Set_Remote();
    if(HP_B->datalog == true)
    {
        HP_B->Identify_Meter();
        HP_B->Display_Text(HP_B->HP_meter->meter_string);
        HP_B->Configure_For_Measurements();
    }
    else
    {
        HP_B->Display_Text("------");
    }

    if(E3648A->control_PS == true)
    {
        printf("Setting up E3648A Power Supply\n");
        E3648A->Set_Remote();
        E3648A->Set_Instrument1();     // sets "Instrument1" as the active instrument to receive commands
        E3648A->Set_Output_OFF();
        E3648A->Set_Volts(12.50);     // start with output voltage of 0
        E3648A->Set_Current(1.50);   // set current limit to only 25mA because we arent driving anything

        E3648A->Set_Instrument2();     // sets "Instrument2" as the active instrument to receive commands
        E3648A->Set_Output_OFF();
        E3648A->Set_Volts(2.50);     // start with output voltage of 0
        E3648A->Set_Current(.1);   // set current limit to only 25mA because we arent driving anything
    }
    else
    {
        E3648A->Display_Text("--OFF--");
        E3648A->Set_Local();
    }
}


void DataLog::Start_Logging(HP34401* HP_A, HP34401* HP_B, E3648* E3648A, Write_Log* FileWrite)
{
    printf("\nDATALOGGING LOOP....\n\n");
    using namespace std;
    clock_t begin, end;
    begin = clock();
    bool logging = true;

    

    if(E3648A->control_PS == true)
    {
        E3648A->Set_Instrument1();
        E3648A->Set_Output_ON();
        E3648A->Set_Instrument2();
        E3648A->Set_Output_ON();
    }

    if(HP_A->datalog == true && HP_B->datalog == true)
    {
        char* description1 = HP_A->Get_Description();
        char* description2 = HP_B->Get_Description();
        FileWrite->Write_Units(description1, description2);
        free(description1);
        free(description2);
    }
    HP_A->Display_Measurement();
    HP_B->Display_Measurement();

    while(logging == true)
    {
        DataLog::Print_Header(HP_A, HP_B);

        for( int i=0; i< 10; ++i)
        {
            if(HP_A->datalog == true)
            {
                HP_A->Send_Trigger();
            }
            if(HP_B->datalog == true)
            {
                HP_B->Send_Trigger();
            }
            if(HP_A->datalog == true)
            {
                HP_A->Retrieve_Measurements(); 
            }
            if(HP_B->datalog == true)
            {
                HP_B->Retrieve_Measurements(); 
            }
            //end = clock();
            //double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            //elapsed_secs *= 1000;


            if(HP_A->datalog == true && HP_B->datalog == true)
            {
                DataLog::Print_Data(HP_A, HP_B);
                DataLog::Write_Data(HP_A, HP_B, FileWrite);
            }
            else if(HP_A->datalog == true && HP_B->datalog == false)
            {
                DataLog::Print_Data(HP_A);
                DataLog::Write_Data(HP_A, FileWrite);
            }
            else if(HP_A->datalog == false && HP_B->datalog == true)
            {
                DataLog::Print_Data(HP_B);
                DataLog::Write_Data(HP_B, FileWrite);
            }
        }
        printf("------------------------------------------------------------------\n");
        if( DataLog::Key_Pressed() )
        {
            char buffer[20];
            StringH::Erase_Num_Chars(buffer, 20);
            DataLog::Read_Keys(buffer, 20);
            int key_pressed = 0;
            for(int i=0; i<strlen(buffer); ++i)
            {
                key_pressed = buffer[i];    // getting decimal ASCII value of character
                if(key_pressed == CARRIAGE_RETURN || key_pressed == QUIT || key_pressed == ENTER_KEY)
                {
                    logging = false;
                    break;
                }
            }
        }
    }
    printf(COLOR_BRIGHT_MAGENTA "DONE.\nTURNING PS OFF\n" COLOR_RESET);

    if(E3648A->control_PS == true)
    {
        E3648A->Set_Volts(0.00);     // start with output voltage of 0
        E3648A->Set_Output_OFF();
    }
}

bool DataLog::Key_Pressed()
{
    int nbytes;
    if (ioctl(STDIN_FILENO, FIONREAD, &nbytes) == 0 && nbytes > 0)    // we have exactly n bytes to read
    {
        return true;
    }
    else
        return false;
}

int DataLog::Get_StdIn_Num_Chars_Available()
{ 
    static bool initflag = false; 

    if (!initflag) 
    {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbytes;
    ioctl(STDIN_FILENO, FIONREAD, &nbytes);  // 0 is STDIN
    return nbytes;
}

void DataLog::Read_Keys(char* buffer, int ssize)
{
    char buff[255];
    int num_read = read(STDIN_FILENO, buff, 255);

    for(int i=0; i<ssize && i<num_read; ++i)
    {
        buffer[i] = buff[i];
    }
}

void DataLog::Print_Header(HP34401* HP_A, HP34401* HP_B)
{
    char* description1 = NULL;
    char* description2 = NULL;
    char* units1 = NULL;
    char* units2 = NULL;
    printf("press 'q' then enter to quit.\n");
    if(HP_A->datalog == true && HP_B->datalog == true)
    {
        description1 = HP_A->Get_Description();
        description2 = HP_B->Get_Description();
        units1 = HP_A->Get_Units();
        units2 = HP_B->Get_Units();
        printf(COLOR_BOLD_BLUE "\t\t%s \t%s\n" COLOR_RESET, description1, description2);
        printf(COLOR_BOLD_BLUE "\t\t%s \t%s\n" COLOR_RESET, units1, units2);
    }
    else if(HP_A->datalog == true && HP_B->datalog == false)
    {
        description1 = HP_A->Get_Description();
        units1 = HP_A->Get_Units();
        printf(COLOR_BOLD_BLUE "\t\t%s\n\t\t%s\n" COLOR_RESET, description1, units1);
    }
    else if(HP_A->datalog == false && HP_B->datalog == true)
    {
        description1 = HP_B->Get_Description();
        units1 = HP_B->Get_Units();
        printf(COLOR_BOLD_BLUE "\t\t%s\n\t\t%s\n" COLOR_RESET, description1, units1);
    }

    if(description1 != NULL) free(description1);
    if(description2 != NULL) free(description2);
    if(units1 != NULL) free(units1);
    if(units2 != NULL) free(units2);
}

void DataLog::Print_Data(HP34401* HP_A, HP34401* HP_B)
{
    Data_Group* data1 = HP_A->meter_data;
    Data_Group* data2 = HP_B->meter_data;
    char* dtime = Write_Log::Get_Time();
    int i = 0;
    while( i< data1->num_points && i< data2->num_points)
    {
        printf(COLOR_YELLOW "%s \t%.6f \t%.6f\n" COLOR_RESET, dtime, data1->data_points[i], data2->data_points[i]);
        ++i;
    }
    free(dtime);
}

void DataLog::Print_Data(HP34401* HP_meter)
{
    Data_Group* data1 = HP_meter->meter_data;
    char* dtime = Write_Log::Get_Time();
    int i = 0;
    while( i< data1->num_points)
    {
        printf(COLOR_YELLOW "%s \t%.6f\n" COLOR_RESET, dtime, data1->data_points[i]);
        ++i;
    }
    free(dtime);
}

void DataLog::Write_Data(HP34401* HP_A, HP34401* HP_B, Write_Log* writefile)
{
    Data_Group* data1 = HP_A->meter_data;
    Data_Group* data2 = HP_B->meter_data;

    writefile->Write_Data(data1, HP_A->units, data2, HP_B->units);

    free(HP_A->meter_data->data_points);
    HP_A->meter_data->data_points = NULL;
    free(HP_A->meter_data);
    HP_A->meter_data = NULL;

    free(HP_B->meter_data->data_points);
    HP_B->meter_data->data_points = NULL;
    free(HP_B->meter_data);
    HP_B->meter_data = NULL;
}

void DataLog::Write_Data(HP34401* HP_meter, Write_Log* writefile)
{
    Data_Group* data1 = HP_meter->meter_data;
    writefile->Write_Data(data1);

    free(HP_meter->meter_data->data_points);
    HP_meter->meter_data->data_points = NULL;
    free(HP_meter->meter_data);
    HP_meter->meter_data = NULL;
}