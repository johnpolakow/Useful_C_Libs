

#include "Menu.h"


#define MESSAGE_LENGTH 100

void Menu::Wait_Enter()
{
	int key_pressed;
	bool Enter_Pressed = false;
    printf("\n\t\t< to continue hit Enter >\n");
	while(!Enter_Pressed)
	{
			key_pressed = getchar();
			if((key_pressed == NEW_LINE) || (key_pressed == CARRIAGE_RETURN))
			{ Enter_Pressed = true; }
			if(key_pressed == QUIT)
			{ Enter_Pressed = true; }
	}
	return;
}

void Menu::Read_Console_Line(char* usr_string, int size)
{
    size_t characters;
  	size_t bufsize = 128;
	char* keyboard_input_buffer = (char*)malloc(sizeof( char* ) * bufsize );
	char send_buffer[MESSAGE_LENGTH];

    characters = getline(&keyboard_input_buffer,&bufsize,stdin); // read line from the keyboard (doesnt return until enter key pushed)
    keyboard_input_buffer[characters-1] = '\0';	// null terminate the string
    sprintf(send_buffer, "%s", keyboard_input_buffer);  // copy text read from keyboard to send buffer
    send_buffer[strlen(send_buffer)] = '\0';

    StringH::Erase_String(usr_string);
	strncpy(usr_string, send_buffer, size);     // copy line read from console to usr_string
}


void Menu::Print_Mode_Menu()
{
    Menu::Clear_Console_Screen();
    printf(COLOR_BOLD_MAGENTA "\t--CALIBRATION Select--\n" COLOR_RESET);
    printf(COLOR_YELLOW "Select Cal Mode:  \n" COLOR_RESET);
    printf(COLOR_YELLOW "  1. Automatic\n" COLOR_RESET);
    printf(COLOR_YELLOW "  2. Manual\n" COLOR_RESET);
}

void Menu::Configure_Logging_Options(HP34401* HP_A, HP34401* HP_B, E3648* E3648A)
{

    printf(COLOR_YELLOW "Attached meters: (look at meter display to correlate with options below)\n" COLOR_RESET);
    char device_string[20];
    if(HP_A != NULL && HP_A->HP_meter->file_descriptor > 0)
    {
        HP_A->Display_Text(HP_A->HP_meter->meter_string);

        strncpy(device_string, HP_A->HP_meter->meter_string, 20);
        this->devices[this->device_index]->Set_Description(device_string);
        this->devices[this->device_index]->Set_Device_Num(this->device_index+1);
        ++(this->device_index);
    }
    if(HP_B != NULL && HP_B->HP_meter->file_descriptor > 0)
    {
        HP_B->Display_Text(HP_B->HP_meter->meter_string);

        strncpy(device_string, HP_B->HP_meter->meter_string, 20);
        this->devices[this->device_index]->Set_Description(device_string);
        this->devices[this->device_index]->device_num = this->device_index+1;
        ++(this->device_index);
    }
    if(E3648A != NULL && E3648A->file_descriptor > 0)
    {
        E3648A->Display_Text("E3648A PS"); 
        this->devices[this->device_index]->description = "E3648A PS";
        this->devices[this->device_index]->device_num = this->device_index+1;
        ++(this->device_index);
    }

    Menu::Print_Attached_Devices();


    //Menu::Free_Device_Mem(devices);


}

void Menu::Free_Device_Mem(Device** devices, int num_to_free)
{
    int index = 0;
    while(devices[index])
    {
        delete devices[index];
        ++index;
    }
}

void Menu::Print_Attached_Devices()
{
    int index = 0;
    for(index; index < this->device_index; ++ index)
    {

            //Device* dev = devices[index];
            printf(COLOR_YELLOW "\t%d.  %s\n" COLOR_RESET, this->devices[index]->device_num, this->devices[index]->description.c_str() );
        
    }
    printf("\n\n");
}


// prints message to the terminal console, and prompts for Enter key to be pressed.
// used during the Cooler DC voltage calibration, instructing the user to set voltage to specific values
void Menu::Prompt_User_Message(char* message)
{
    printf("%s\n", message);
    Wait_Enter();
}

void Menu::Clear_Console_Screen()
{
      	printf( "\033[0m\033[2J\033[H");  // clears the terminal and brings cursor to first line
      	printf("\n");
}

void Menu::Print_Main_Menu()
{
    printf(COLOR_YELLOW "\t--Data Log w/ HP34401 & Agilent E3648A Power Supply--\n" COLOR_RESET);

}

int Menu::Get_Cal_Selection()
{
    int ssize = 20;
    char selection[ ssize ];
    StringH::Erase_Num_Chars(selection, ssize);
    Menu::Read_Console_Line(selection, ssize);

    int cal_choice = StringH::Parse_Int(selection);
    if(cal_choice > 0)
    {
        switch(cal_choice)
        {
            case 1:
                return CAL_COOLER_DCV;
            case 2:
                return CAL_COOLER_AC_MA;
            case 3:
                return CAL_REF100;
            case 4:
                return CAL_LOAD;
            case 5:
                return CAL_DIODE;
            case 6:
                return CAL_EXIT;
            default:
                return SEL_ERROR;
        }
    }
    else
        return SEL_ERROR;
}
