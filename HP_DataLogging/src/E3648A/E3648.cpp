

#include "E3648.h"



void E3648::Display_Text(const char* text)
{
    char text_command[100];
    StringH::Erase_Num_Chars(text_command, 100);

    if( strlen(text)<15)
    {
        sprintf(text_command, "DISPLAY:TEXT '%s'", text);
    }
    else
    {
        char shortened_string[15];
        StringH::Erase_Num_Chars(shortened_string, 15);
        strncpy(shortened_string, text, 14);
        shortened_string[14] = '\0';

        sprintf(text_command, "DISPLAY:TEXT '%s'", shortened_string);
    }
    E3648::Send_Command(text_command);
    usleep(ONE_MILLISECOND*100);
}


void E3648::Initialize_PowerSupply()
{
  if( !(this->file_descriptor > 0) )
  {
    printf("\n Opening connection to E3648A meter ...\n");
    this->file_descriptor = E3648::open_serial_port(this->device_path);
    printf("\tmfr: %s\n", this->manufacturer);

    printf("\t--setting E3648A to remote mode ...\n");
    E3648::Set_Remote();

    printf("\t--resetting E3648A ...\n");
    E3648::Reset();
  }
  else
  {
    printf("  Cant't open the E3648A Power Supply at:\t%s, (FD == %d)\n", this->device_path, this->file_descriptor);
    printf(COLOR_BOLD_RED "\t Is the USB Attached??\n" COLOR_RESET);
    printf("\n  CALIBRATION CANNOT CONTINUE. exiting...\n");
    exit(0);
  }
}

// returns E3648A ID string. Memory for the string must be freed afterwards
char* E3648::Get_Power_Supply_ID()
{
    E3648::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*100); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    E3648::Read_Response(response);
    // Example response:    HEWLETT-PACKARD,34401A,0,2-1-1

    printf("%s\n", response);
    char delims[] = ",";    // delimiters to be used in parsing out tokens
    char* token = strtok(response, delims);

    if( token != NULL && strlen(token) > 0)
    {

        bool correct = StringH::String_Contains(token, "Agilent Technologies");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "Agilent Technologies");

        token = strtok(NULL, delims);
        correct = StringH::String_Contains(token, "E3648A");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "E3648A");

        char* ID = (char*)malloc( sizeof(char) * 15); // ID actually should only be 7 characters long
        StringH::Erase_Num_Chars(ID, 10);

        token = strtok(NULL, delims);
        strncpy(ID, token, 2);
        char comma[] = ",";
        strcat(ID, comma);

        token = strtok(NULL, delims);
        strcat(ID, token);

        printf("\tE3648A ID: " COLOR_MAGENTA "\t%s" COLOR_RESET, ID);

        return ID;
    }
    else
        printf("Could not read response from Agilent PS.\n");
    return NULL;
}


void E3648::Reset()
{
    E3648::Send_Command(RESET);
    usleep(ONE_MILLISECOND*1000);
    this->ON = false;
}

void E3648::Set_Output_ON()
{
    char command[30];
    StringH::Erase_Num_Chars(command, 30);
    sprintf(command, "%s", "OUTP:STAT ON");
    E3648::Send_Command(command);

    int cur_instrument = this->cur_instrument;
    float volts;
    if(cur_instrument == 1)
         volts = this->instr1_volt;

    if(cur_instrument == 2)
        volts = this->instr2_volt;

    StringH::Erase_Num_Chars(command, 30);
    sprintf(command, "%.3f V%d ON",  volts, cur_instrument);

    E3648::Display_Text(command);
    this->ON = true;
}

void E3648::Set_Output_OFF()
{
    char command[30];
    StringH::Erase_Num_Chars(command, 30);
    sprintf(command, "%s", "OUTP:STAT OFF");
    E3648::Send_Command(command);
    E3648::Display_Text("-- OFF --");
    this->ON = false;
}

// sets the volt range of the currently selected instrument
void E3648::Set_Volt_Range_HIGH()
{
    char command[30];

    sprintf(command, "%s %s", VOLT_RANGE, "HIGH");
    E3648::Send_Command(command);
}

// sets the volt range of the currently selected instrument
void E3648::Set_Volt_Range_LOW()
{
    char command[30];
    StringH::Erase_Num_Chars(command, 30);
    sprintf(command, "%s %s", VOLT_RANGE, "LOW");
    E3648::Send_Command(command);
}

// sets output voltage of currently selected isntrument
void E3648::Set_Volts(double volts)
{
    if( volts > 7.5 )
        Set_Volt_Range_HIGH();

    char command[100];
    StringH::Erase_Num_Chars(command, 100);
    sprintf(command, "%s %f", SET_VOLTAGE, volts);
    E3648::Send_Command(command);

    int cur_instrument = this->cur_instrument;
    if(cur_instrument == 1)
        this->instr1_volt = volts;

    if(cur_instrument == 2)
        this->instr2_volt = volts;

    StringH::Erase_Num_Chars(command, 100);
    if( this->ON == true )
        sprintf(command, "%.3f V%d ON",  volts, cur_instrument);
    else
        sprintf(command, "%.2f V%d OFF",  volts, cur_instrument);
    E3648::Display_Text(command);
}


void E3648::Set_Current(double amps)
{
    if( amps > 5.0 )
        return;

    char* save_display_text = this->Get_Display_Text();
    // Get_Display_Text returns a quoted string, we want to remove the quotes for later display

    for(int i =0; i<strlen(save_display_text); ++i)
    {
        if(save_display_text[i] == '"')
            save_display_text[i] = ' ';
    }
    StringH::Trim_WhiteSpace(save_display_text);
    //printf("current display: '%s'\n", save_display_text);

    char command[30];
    StringH::Erase_Num_Chars(command, 30);
    sprintf(command, "%s %.3f", SET_CURRENT, amps);
    E3648::Send_Command(command);

    int cur_instrument = this->cur_instrument;
    if(cur_instrument == 1)
        this->instr1_volt = amps;

    if(cur_instrument == 2)
        this->instr2_volt = amps;

    StringH::Erase_Num_Chars(command, 30);
    if( this->ON == true )
        sprintf(command, "%.3f A%d ON",  amps, cur_instrument);
    else
        sprintf(command, "%.2f A%d OFF",  amps, cur_instrument);
    E3648::Display_Text(command);

    usleep(ONE_MILLISECOND*100);
    E3648::Display_Text(save_display_text);
    free(save_display_text);
}

char* E3648::Get_Display_Text()
{
    char text_command[80];
    StringH::Erase_Num_Chars(text_command, 80);

    sprintf(text_command, "%s", GET_TEXT);
    E3648::Send_Command(text_command);
    usleep(ONE_MILLISECOND*50);
    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    E3648::Read_Response(response);
    return strdup(response);
}

// Sets Instrument1 as the active instrument to receive commands
void E3648::Set_Instrument1()
{
    char PS_string[40];
    StringH::Erase_Num_Chars(PS_string, 40);

    sprintf(PS_string, "%s", "INST:SEL OUT1");
    E3648::Send_Command(PS_string );
    usleep(ONE_MILLISECOND*50);
    E3648::Display_Text("INSTR 1");
    usleep(ONE_MILLISECOND*100);
    this->cur_instrument = 1;
}

void E3648::Set_Instrument2()
{
    char PS_string[40];
    StringH::Erase_Num_Chars(PS_string, 40);
    sprintf(PS_string, "%s", "INST:SEL OUT2");

    E3648::Send_Command(PS_string );
    usleep(ONE_MILLISECOND*50);
    E3648::Display_Text("INSTR 2");
    usleep(ONE_MILLISECOND*50);
    this->cur_instrument = 2;
}


float E3648::Query_Set_Voltage()
{
    char PS_string[40];
    StringH::Erase_Num_Chars(PS_string, 40);
    sprintf(PS_string, "%s", QUERY_PROG_VOLT);

    E3648::Send_Command(PS_string );
    usleep(ONE_MILLISECOND*50);
    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    E3648::Read_Response(response);
    float volts = StringH::Parse_Scientific_Notation(response);
    return volts;
}

void E3648::Get_Errors()
{
    char PS_string[40];
    StringH::Erase_Num_Chars(PS_string, 40);

    sprintf(PS_string, "%s", "SYST:ERR?");

    E3648::Send_Command(PS_string );
    usleep(ONE_MILLISECOND*1000);
    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    E3648::Read_Response(response);
}

// returns a 1 or a 2 depending on what instrument is selected
int E3648::Query_Instrument()
{
    E3648::Send_Command(INSTR_SEL_QUERY);
    usleep(ONE_MILLISECOND*100);
    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer
    E3648::Read_Response(response);
    int instrument = StringH::Parse_Int(response);

    return instrument;
}


int E3648::open_serial_port(const char* device)
{
    int port;
    port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    /*  O_RDWR POSIX read write
        O_NOCTTY: this program does not want to be the "controlling terminal"
        O_NDELAY": Ignore DCD signal state. If you dont set this, program will sleep until DCD is at space voltage
   */

  printf("opened device " COLOR_MAGENTA "%s\n" COLOR_RESET, device);
  if(port == -1)
    { perror(" - Unable to open ttyUSBX - \n"); return false; }
  else
    fcntl(port, F_SETFL, 0);
        // fnctl - performs operation on file descriptor specified, "port"
        // F_SETFL  - set file descriptor flags. Can only change these flags: O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK
        // 0 - clear all flags

  E3648::setup_serial_port(port);

  return port;
}

int E3648::setup_serial_port(int port_FD)
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
  //SerialPortSettings.c_cflag |=  CRTSCTS;         //Turn on flow control

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

  E3648::Enable_DTR(port_FD);     // Enable output flow control pins
  E3648::Enable_DSR(port_FD);     // Enable output flow control pins
  E3648::Enable_RTS(port_FD);     // Enable output flow control pins

  if(DEBUG_FLOW) E3648::Print_Modem_Bits(port_FD);

  return result;
}

void E3648::Set_Remote()
{
    E3648::Send_Command(SET_REMOTE);
    usleep(ONE_MILLISECOND*30);

    E3648::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*30); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    E3648::Read_Response(response);

    return;
}

void E3648::Set_Local()
{
    E3648::Send_Command(SET_LOCAL);
    usleep(ONE_MILLISECOND*40);

    E3648::Send_Command(GET_ID);
    usleep(ONE_MILLISECOND*40); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    E3648::Read_Response(response);

    return;
}

void E3648::Print_Modem_Bits(int port_FD)
{
    int status_bits;
    ioctl(port_FD, TIOCMGET, &status_bits);   // gets the status of the modem bits: TIOCM_DTR, TIOCM_RTS, TIOCM_CTS, TIOCM_DSR, ...

    int DTR_flag, CTS_flag, RTS_flag, DSR_flag;

    DTR_flag = status_bits & TIOCM_DTR; // Mask out the specific bit
    DSR_flag = status_bits & TIOCM_DSR;
    CTS_flag = status_bits & TIOCM_CTS;
    RTS_flag = status_bits & TIOCM_RTS;

    if( DEBUG_FLOW )
    {
        printf("--Modem Status Bits--\n");

        if(DTR_flag)    printf("\tDTR bit: SET\n");
        else            printf("\tDTR bit: OFF\n");

        if(DSR_flag)    printf("\tDSR bit: SET\n");
        else            printf("\tDSR bit: OFF\n");

        if(CTS_flag)    printf("\tCTS bit: SET\n");
        else            printf("\tCTS bit: OFF\n");

        if(RTS_flag)    printf("\tRTS bit: SET\n\n");
        else            printf("\tRTS bit: OFF\n\n");
    }

    return;
}

int E3648::Send_Command(const char* command_string)
{
    int port_FD = this->file_descriptor;
    int num_written = 0;

    char write_string[255];
    StringH::Erase_Num_Chars(write_string, 255);
    strcpy(write_string, command_string);
    strcat(write_string, "\r\n");       // add carriage return and newline

    //printf(COLOR_LIGHT_MAGENTA "%s\n" COLOR_RESET, command_string);
    num_written = write(port_FD, write_string, strlen(write_string) );
    if (num_written < 0)
        fputs("write() to E3648 failed!\n", stderr);

    usleep(ONE_MILLISECOND*500); //Wait after sending to let E3648A process.

    return num_written;
}

void E3648::Enable_Control(bool setting)
{
    this->control_PS = setting;
}

float E3648::Measure_Volts()
{

    char buffer[255];
    StringH::Erase_Num_Chars(buffer, 255);

    E3648::Send_Command(MEAS_VOLT);
    usleep(ONE_MILLISECOND*100); //Wait after sending

    E3648::Read_Response(buffer);
    usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    if(DEBUG) printf("PS DCV: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, volts);

    return volts;
}

float E3648::Measure_Current()
{

    char buffer[255];
    StringH::Erase_Num_Chars(buffer, 255);

    E3648::Send_Command(MEAS_CURRENT);
    usleep(ONE_MILLISECOND*100); //Wait after sending

    E3648::Read_Response(buffer);
    usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    if(DEBUG) printf("PS DCA: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, volts);
    return volts;
}

void E3648::Print_Hex(char* string)
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

void E3648::Enable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIS, &DTR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void E3648::Disable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIC, &DTR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void E3648::Enable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIS, &DSR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void E3648::Disable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIC, &DSR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void E3648::Enable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIS, &RTS_flag);  //Set RTS pin w/ "TIOCMBIS" command
}

void E3648::Disable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIC,&RTS_flag);   //Clear RTS pin w/ "TIOCMBIC" command
}

int E3648::Get_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);
    if( DEBUG )
        printf("%d bytes in input buffer\n", bytes_ready);

    return bytes_ready;
}

void E3648::Read_Response(char* response_string)
{
    char buff[255];
    for(int i = 0; i< 255; ++i)
        buff[i] = '\0';     // have to do this otherwise characters from other loop iterations are printed

    int bytes_ready = E3648::Get_Num_Chars_Waiting(this->file_descriptor);
    if( !(bytes_ready > 0) )
    {
        usleep(ONE_MILLISECOND*500);
        while( !(E3648::Get_Num_Chars_Waiting(this->file_descriptor) > 0) )
        {
            printf("waiting another 500 ms for response from E3648A\n");
            usleep(ONE_MILLISECOND*500);
        }
    }

    int num_read = read(this->file_descriptor, buff, 255);
    if( num_read > 0 )
    {
        if( DEBUG )
        {
            printf("\tresponse:\t" COLOR_BOLD_CYAN "%s\n" COLOR_RESET, buff);
        }
    }
    else
        printf(COLOR_BOLD_RED "\tno response from E3648A, even after extended wait\n\n" COLOR_RESET);

    StringH::Erase_String(response_string);
    strcpy(response_string, buff);

    return;
}


