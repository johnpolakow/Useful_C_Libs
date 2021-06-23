
#include "HP34401.h"

#define DEBUG false
#define DEBUG_METER false
#define DEBUG_HEX false


void HP34401::Display_Text(const char* text)
{
    char text_command[80];
    StringH::Erase_Num_Chars(text_command, 80);

    if( strlen(text)<15)
    {
        sprintf(text_command, "DISPLAY:TEXT '%s'", text);
    }
    else
    {
        char shortened_string[15];
        StringH::Erase_Num_Chars(shortened_string, 15);
        strncpy(shortened_string, text, 15);
        shortened_string[14] = '\0';

        sprintf(text_command, "DISPLAY:TEXT '%s'", shortened_string);
    }

    HP34401::Send_Command(text_command);
    usleep(ONE_MILLISECOND*50);
}

void HP34401::Reset()
{
    //printf("RESETING %s\n", this->HP_meter->meter_string);
    HP34401::Send_Command(RESET);
    usleep(ONE_MILLISECOND*500);
    this->Set_Remote();
}

void HP34401::Configure_For_Measurements()
{
    switch(this->measurement_type)
    {
        case AC_VOLTS:
            this->Configure_Volts_AC();
            break;

        case AC_AMPS:
            this->Configure_Current_AC();
            break;


        case DC_VOLTS:
            this->Configure_Volts_DC();
            break;

        case DC_AMPS:
            this->Configure_Current_DC();
            break;
        default:
            printf("Problem, no valid measurement");
    }

    //this->Configure_Num_Samples_Per_Trigger(this->samples_per_trigger);
    //this->Configure_Num_Triggers(this->trigger_count);
    this->Print_Meter_Configs();
}

void HP34401::Enable_Logging()
{
    this->datalog = true;
}

void HP34401::Disable_Logging()
{
    this->datalog = false;
}

void HP34401::Set_Num_Samples_Per_Trigger(int num_samples)
{
    this->samples_per_trigger = num_samples;
}

void HP34401::Set_Num_Triggers(int num_triggers)
{
    this->trigger_count = num_triggers;
}

void HP34401::Set_Description(const char* descrip)
{
    strncpy(this->description, descrip, MAX_DESCRIPTION);
}

void HP34401::Set_Measurement_Type(meas_t meas_type)
{
    this->measurement_type = meas_type;
}

meas_t HP34401::Get_Measurement_Type()
{
    return this->measurement_type;
}

char* HP34401::Get_Description()
{
    return strdup(this->description);
}

char* HP34401::Get_Units()
{
    int str_size = 10;
    char units[str_size];
    switch(this->measurement_type)
    {
        case AC_VOLTS:
            strncpy(units, "RMS VAC", str_size);
            break;

        case AC_AMPS:
            strncpy(units, "RMS AAC", str_size);
            break;


        case DC_VOLTS:
            strncpy(units, "DCV", str_size);
            break;

        case DC_AMPS:
            strncpy(units, "ADC", str_size);
            break;
    }
    return strdup(units);
}

void HP34401::Print_Meter_Configs()
{
    int str_size = 20;
    char type_string[str_size];
    StringH::Erase_Num_Chars(type_string, str_size);

    int samples = this->samples_per_trigger;
    int trigs = this->trigger_count;
    
    switch(this->measurement_type)
    {
        case AC_VOLTS:
            strncpy(type_string, "AC Voltage", str_size);
            break;

        case AC_AMPS:
            strncpy(type_string, "AC Current", str_size);
            break;


        case DC_VOLTS:
            strncpy(type_string, "DC Voltage", str_size);
            break;

        case DC_AMPS:
            strncpy(type_string, "DC Current", str_size);
            break;
        default:
            printf("Problem, no valid measurement");
    }
    printf("configuring " COLOR_MAGENTA "%s" COLOR_RESET " %s measurements...\n", this->HP_meter->meter_string, type_string);
    StringH::Trim_WhiteSpace(this->HP_meter->meter_ID);
    printf("\tID: %s \n", this->HP_meter->meter_ID);
    StringH::To_Upper(type_string);
    printf("\tmeas type: " COLOR_MAGENTA "%s" COLOR_RESET " \n\n", type_string);

/*
    printf("\t  samples per trig: %d \n" \
           "\t  triggers per meas: %d \n" \
           "\t  total samples per meas: " COLOR_MAGENTA "%d \n\n" COLOR_RESET, samples, trigs, samples*trigs);    
           */
}

void HP34401::Configure_Current_DC()
{
    HP34401::Send_Command(SET_CURRENT_DC );
    usleep(ONE_MILLISECOND*30);
    strcpy(this->units, "DC_A");
    this->measurement_type = DC_AMPS;
}

void HP34401::Configure_Volts_DC()
{
    HP34401::Send_Command(SET_VOLTS_DC );
    usleep(ONE_MILLISECOND*30);
    strcpy(this->units, "V_DC");
    this->measurement_type = DC_VOLTS;
}

void HP34401::Configure_Current_AC()
{
    HP34401::Send_Command(SET_CURRENT_AC );
    usleep(ONE_MILLISECOND*30);
    strcpy(this->units, "RMS_AAC");
    this->measurement_type = AC_AMPS;
}

void HP34401::Configure_Volts_AC()
{
    HP34401::Send_Command(SET_VOLTS_AC );
    usleep(ONE_MILLISECOND*30);
    strcpy(this->units, "RMS_VAC");
    this->measurement_type = AC_VOLTS;
}

void HP34401::Configure_Num_Samples_Per_Trigger(int num_samples)
{
    HP34401::Send_Command( SET_TRIGGER_IMM );    // sets trigger to immediate. Only have to send the "INIT" command
    usleep(ONE_MILLISECOND*40);

    this->samples_per_trigger = num_samples;
    char command[50];
    sprintf(command, "SAMPLE:COUNT %d", this->samples_per_trigger);     // take 3 samples per trigger
    HP34401::Send_Command( command );        // send command
    usleep(ONE_MILLISECOND*30);
}

void HP34401::Configure_Num_Triggers(int num_triggers)
{
    this->trigger_count = num_triggers;
    char command[50];
    sprintf(command, "TRIGGER:COUNT %d", this->trigger_count );     // set to 5 triggers total before returning to idle
    HP34401::Send_Command(command );          // send command
    usleep(ONE_MILLISECOND*50);
}


void HP34401::Send_Trigger()
{
    HP34401::Send_Command(INIT );        // send command  to take the readings 
    usleep(ONE_MILLISECOND*30);     
    this->begin_fetch_wait = clock();   // start wait clock
    HP34401::Calc_Wait_Before_Reading();
}


// Must wait a minimum amount of time before sending the FETCH command, or meter will beep error, and say it is retrieving stale data
void HP34401::Calc_Wait_Before_Reading()
{
    // wait this long for measurements to complete on HP34401 before sending FETCH command 
    int num_samples = this->samples_per_trigger * this->trigger_count;
    switch(this->measurement_type)
    {
        case AC_VOLTS:
            this->ms_wait_before_fetch =  150 + t_offset_ACV; 
            break;

        case AC_AMPS:
            this->ms_wait_before_fetch =  180 + t_offset_ACA; 
            break;


        case DC_VOLTS:
            this->ms_wait_before_fetch =  50 + t_offset_DCV; 
            break;

        case DC_AMPS:
            this->ms_wait_before_fetch =  80 + t_offset_DCA; 
            break;
    }    
}

void HP34401::Retrieve_Measurements()
{
    char buffer[500];
    StringH::Erase_Num_Chars(buffer, 500);

    // must wait a minimum amount of time for readings to complete on the HP34401 before attempting to read them
    int elapsed_ms = HP34401::Calc_Milliseconds_Waited();
    if( !(elapsed_ms > this->ms_wait_before_fetch) )
    {
        //printf("wait longer\n");
        while(elapsed_ms < this->ms_wait_before_fetch)
        {
            usleep(50);
            elapsed_ms = HP34401::Calc_Milliseconds_Waited();
        }
    }

    HP34401::Send_Command(FETCH );                  // send command to read the data
    usleep(ONE_MILLISECOND *60);

    HP34401::Read_Response(buffer);


    if(this->meter_data != NULL)
    {
        free(this->meter_data->data_points);
        this->meter_data->data_points = NULL;
        free(this->meter_data);
        meter_data = NULL;
    }
    this->meter_data = HP34401::Parse_HP_Line(buffer);
    strcpy(this->meter_data->description, this->description);
    strcpy(this->meter_data->units, this->units);

    return;
}

char* HP34401::Get_Errors()
{
    HP34401::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*150);

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    HP34401::Read_Response(response);

    if( strlen(response) > 0)
        printf("Errors:\n %s\n", response);

    return strdup(response);
}



int HP34401::Calc_Milliseconds_Waited()
{
    clock_t now = clock();
    double elapsed_ms = 1000 * (double(now - this->begin_fetch_wait) / CLOCKS_PER_SEC);
    return (int)elapsed_ms;
}


Data_Group* HP34401::Parse_HP_Line(char* line_string)
{
    double data_points[50];
    int data_index = 0;

    char* line = strdup(line_string);

    //printf("line: %s\n", line);

    while( StringH::String_Contains(line, ",") )
    {
        //printf("line: %s \n", line);
        // remove first item from the list, store in buffer, up to 10 items
        int comma_index = StringH::Index_First_Comma(line);
        char* first_item_in_list  = StringH::Get_Substring(line, 0, comma_index-1);
        if(DEBUG) printf("\t%s  ....\t", first_item_in_list);
        data_points[ data_index ] = StringH::Parse_Scientific_Notation( first_item_in_list );
        if(DEBUG) printf("   %f \n", data_points[ data_index ]);
        ++data_index;
        free(first_item_in_list);

        char* updated_list = StringH::Get_Substring( line, comma_index+1, strlen(line)-1);   // this step updates the "line" string, creating new string w/out first token
        free(line);
        line = updated_list;
    }
    data_points[ data_index ] = StringH::Parse_Scientific_Notation( line );  // parse the last token when no commas remain
    ++data_index;
    free( line );

    //printf("data_index: %d\n", data_index);
    Data_Group* datagroup = (Data_Group*)malloc(sizeof(Data_Group));
    datagroup->data_points = (float*)malloc( sizeof(float) * data_index+1);

    int num_points = 0;
    for(int i = 0; i< data_index; ++i)
    {
        datagroup->data_points[i] = data_points[i];
        ++num_points;
    }
    datagroup->num_points = num_points;
    datagroup->average = Data::Calc_Average(datagroup );
    datagroup->std_dev = Data::Calc_Std_Dev(datagroup );

    // Assign units and descriptino to the datagroup
    meas_t meas_type = this->Get_Measurement_Type();
    char* data_description = this->Get_Description();
    strncpy(datagroup->description, data_description, 25);
    free(data_description);


    switch(meas_type)
    {
        case AC_VOLTS:
            strcpy(datagroup->units, "RMS VAC");
            break;

        case AC_AMPS:
            strcpy(datagroup->units, "RMS AAC");
            break;


        case DC_VOLTS:
            strcpy(datagroup->units, "DC V");
            break;

        case DC_AMPS:
            strcpy(datagroup->units, "DC A");
            break;
    }

    return datagroup;
}

void HP34401::Set_Remote()
{
    HP34401::Send_Command(SET_REMOTE);
    usleep(ONE_MILLISECOND*150);

    HP34401::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*75); //Wait after sending
    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    HP34401::Read_Response(response);

    bool correct = StringH::String_Contains(response, "HEWLETT-PACKARD,34401A,");

    return;
}

int HP34401::Send_Command(const char* command_string)
{
    int port_FD = this->HP_meter->file_descriptor;
    int num_written = 0;

    char write_string[255];
    StringH::Erase_Num_Chars(write_string, 255);
    strcpy(write_string, command_string);
    strcat(write_string, "\r\n");       // add carriage return and newline

    num_written = write(port_FD, write_string, strlen(write_string) );
    if (num_written < 0)
        fputs("write() to HP34401 failed!\n", stderr);

    if( DEBUG_METER )
    {
        char dev_path[50];
        strcpy(dev_path, this->HP_meter->device_path);
        int meter_number = parse_USB_Number(dev_path);
        printf("CMD to " COLOR_BRIGHT_MAGENTA "Meter%d: \t" COLOR_RESET COLOR_BRIGHT_YELLOW "%s\n" COLOR_RESET, meter_number, command_string );
    }
    if( DEBUG_HEX )
    {
        char dev_path[50];
        strcpy(dev_path, this->HP_meter->device_path);
        int meter_number = parse_USB_Number(dev_path);
        Print_Characters_Hex(write_string);      // used to debug line termination
        printf("CMD to " COLOR_BRIGHT_MAGENTA "Meter%d: \t" COLOR_RESET COLOR_BRIGHT_YELLOW "%s\n" COLOR_RESET, meter_number, command_string );
    }

    usleep(ONE_MILLISECOND*200); //Wait after sending to let HP34401 process.

    return num_written;
}

int HP34401::parse_USB_Number(char* path)
{
    //printf("path: %s\n", path);
    char devpath[100];
    strcpy(devpath, path);



    char delimiter[] = "/";    // delimiters to be used in parsing out tokens
    char* token = strtok(devpath, delimiter);


    while( token != NULL)
    {
        if( StringH::String_Contains(token, "ttyUSB") )
        {
            char* position = strrchr(token, 'B');   // gets pointer to character 'B' in "ttyUSB"
            position++; // increment pointer to character after ttyUSB, which should be a digit
            char ch_digit = *position;
            int digit = ch_digit - '0'; // convert ASCII value to digit by subtracting ASCII value for 0
            return digit;
        }
        token = strtok(NULL, delimiter);

    }
    return 0;
}

float HP34401::Measure_Volts_DC()
{

    char buffer[255];
    StringH::Erase_Num_Chars(buffer, 255);

    HP34401::Send_Command("MEAS:VOLT:DC?");
    usleep(ONE_MILLISECOND*100); //Wait after sending

    HP34401::Read_Response(buffer);
    //usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    char dev_path[50];
    strcpy(dev_path, this->HP_meter->device_path);
    int meter_number = parse_USB_Number(dev_path);

    printf("Meter%d DCV: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, meter_number, volts);
    if(DEBUG) printf("\tdev path: %s\n\n", this->HP_meter->device_path);
    return volts;
}

float HP34401::Measure_Current_DC()
{

    char buffer[255];
    StringH::Erase_Num_Chars(buffer, 255);

    HP34401::Send_Command("MEAS:CURRENT:DC?");
    usleep(ONE_MILLISECOND*100); //Wait after sending

    HP34401::Read_Response(buffer);
    //usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    char dev_path[50];
    strcpy(dev_path, this->HP_meter->device_path);
    int meter_number = parse_USB_Number(dev_path);

    printf("Meter%d DC Amps: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, meter_number, volts);
    return volts;
}

void HP34401::Print_Characters_Hex(char* string)
{
    printf("\t\tASCII VALUES of STRING\n");
    printf(" dec. \t HEX \t char\n");
    printf("__________________________________\n");

    for(int i=0; i<strlen(string); ++i)
    {
        char cur_char;
        cur_char = string[i];
        int ascii_val = (int)cur_char;

        printf(" %d \t 0x%02X \t ", ascii_val, ascii_val);

        if(ascii_val >= 33)
            printf("%c\n", cur_char);
        else
        {
            switch(ascii_val)
            {
                case(8):
                    printf("backspace\n");
                    break;
                case(9):
                    printf("horizontal tab\n");
                    break;
                case(10):
                    printf("line feed\n");
                    break;
                case(12):
                    printf("form feed\n");
                    break;
                case(13):
                    printf("carriage return\n");
                    break;
                case(27):
                    printf("escape\n");
                    break;
                case(32):
                    printf("space\n");
                    break;
                case(0):
                    printf("NULL\n");
                    break;
            }
        }
    }
    printf("\n");
}



int HP34401::Get_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);
    if( DEBUG )
        printf("%d bytes in input buffer\n", bytes_ready);

    return bytes_ready;
}

void HP34401::Read_Response(char* response_string)
{
    char buff[255];
    for(int i = 0; i< 255; ++i)
        buff[i] = '\0';     // have to do this otherwise characters from other loop iterations are printed

    int bytes_ready = HP34401::Get_Num_Chars_Waiting(this->HP_meter->file_descriptor);
    if( !(bytes_ready > 0) )
    {
        usleep(ONE_MILLISECOND*15);
        int TIMEOUT_secs = 1;
        double elapsed_secs = 0;
        clock_t begin, end;
        begin = clock();
        while( !(HP34401::Get_Num_Chars_Waiting(this->HP_meter->file_descriptor) > 0) && elapsed_secs < TIMEOUT_secs)
        {
            end = clock();
            elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            usleep(ONE_MILLISECOND*10);
            printf(" %f  w ", elapsed_secs);
        }
        end = clock();
        int elapsed_ms = 1000*(double)((end-begin)/ CLOCKS_PER_SEC);
        printf("elapsed ms: %d\n", elapsed_ms);
        bytes_ready = HP34401::Get_Num_Chars_Waiting(this->HP_meter->file_descriptor);
        if(bytes_ready > 0)
        {
            printf("adding to time delay before sending FETCH\n");
            switch(this->measurement_type)
            {
                case AC_VOLTS:
                    (this->t_offset_ACV) += elapsed_ms + 5;
                    break;

                case AC_AMPS:
                    (this->t_offset_ACA) += elapsed_ms + 5;
                    break;


                case DC_VOLTS:
                    (this->t_offset_DCV) += elapsed_ms + 5;
                    break;

                case DC_AMPS:
                    (this->t_offset_DCA) += elapsed_ms + 5;
                    break;
            }
        }
        else
        {
            printf("Did not see response, sending another Trigger and waiting longer\n");
            this->Send_Trigger();

            
            elapsed_ms = HP34401::Calc_Milliseconds_Waited();
            if( !(elapsed_ms > this->ms_wait_before_fetch) )
            {
                //printf("wait longer\n");
                while(elapsed_ms < this->ms_wait_before_fetch)
                {
                    usleep(100);
                    elapsed_ms = HP34401::Calc_Milliseconds_Waited();
                }
            }

            HP34401::Send_Command(FETCH );                  // send command to read the data
            usleep(ONE_MILLISECOND *50);

        }
    }

    int num_read = read(this->HP_meter->file_descriptor, buff, 255);
    if( num_read > 0 )
    {
        if( DEBUG )
        {
            printf("\tresponse:\t" COLOR_BOLD_CYAN "%s\n" COLOR_RESET, buff);
        }
    }
    else
    {
        printf(COLOR_BOLD_RED "\tno response from HP34401, even after extended wait\n\n" COLOR_RESET);
        response_string[0] = '\0';
        return;
    }

    
    StringH::Trim_WhiteSpace(buff);
    StringH::Erase_String(response_string);
    strcpy(response_string, buff);

    return;
}

// Displays the ID string on the display of each meter
void HP34401::Identify_Meter()
{
  char* ID1 = HP34401::Get_Meter_ID();
  strcpy(this->HP_meter->meter_ID, ID1);
  free(ID1);

  HP34401::Display_Text(this->HP_meter->meter_ID);
  usleep(ONE_MILLISECOND *75);
}

// Displays the ID string on the display of each meter
void HP34401::Display_Measurement()
{
  char measurement[20];
    switch(this->measurement_type)
    {
        case AC_VOLTS:
            strncpy(measurement, "AC VOLTS", 20);
            break;

        case AC_AMPS:
            strncpy(measurement, "AC AMPS", 20);
            break;


        case DC_VOLTS:
            strncpy(measurement, "DC VOLTS", 20);
            break;

        case DC_AMPS:
            strncpy(measurement, "DC AMPS", 20);
            break;
    }
  HP34401::Display_Text(this->HP_meter->meter_ID);
}




// returns HP34401A ID string. Memory for the string must be freed afterwards
char* HP34401::Get_Meter_ID()
{
    HP34401::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*60); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    HP34401::Read_Response(response);
    // Example response:    HEWLETT-PACKARD,34401A,0,2-1-1

    char delims[] = ",";    // delimiters to be used in parsing out tokens
    char* token = strtok(response, delims);

    if( token != NULL && strlen(token) > 0)
    {
        bool correct = StringH::String_Contains(token, "HEWLETT-PACKARD");  // first token
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "HEWLETT-PACKARD");

        token = strtok(NULL, delims);
        correct = StringH::String_Contains(token, "34401A");        // second token
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "34401A");

        char* ID = (char*)malloc( sizeof(char) * 15); // ID actually should only be 7 characters long
        StringH::Erase_Num_Chars(ID, 10);

        token = strtok(NULL, delims);       // third token
        strncpy(ID, token, 2);
        char comma[] = ",";             // intentionally add comma
        strcat(ID, comma);

        token = strtok(NULL, delims);   // last token
        strcat(ID, token);
        StringH::Trim_WhiteSpace(ID);

        return ID;
    }
    else
        printf("Could not read response from HP meter.\n");
    
    return 0;
}