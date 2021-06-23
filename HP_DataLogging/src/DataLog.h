#include <stdio.h>
#include <unistd.h>     //UNIX Standard function definitions
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>

#include "HP34401.h"
#include "Data.h"
#include "E3648.h"
#include "Write_Log.h"
#include "Menu.h"
#include "Cal_Parameters.h"
#include <ctime>


#define STDIN   0
class DataLog
{
public:
    DataLog(){}

    static void Setup_Instruments(HP34401* HP_A, HP34401* HP_B, E3648* E3648A);
    static void Start_Logging(HP34401* HP_A, HP34401* HP_B, E3648* E3648A, Write_Log* FileWrite);
    static void Setup_Datagroup(Data_Group* meas_group, HP34401* meter);
    static void Print_Data(HP34401* HP_A, HP34401* HP_B);
    static void Print_Data(HP34401* HP_meter);
    static void Write_Data(HP34401* HP_A, HP34401* HP_B, Write_Log* writefile);
    static void Write_Data(HP34401* HP_meter, Write_Log* writefile);
    static void Print_Header(HP34401* HP_A, HP34401* HP_B);
    static bool Key_Pressed();
    static int Get_StdIn_Num_Chars_Available();
    static void Read_Keys(char* buffer, int ssize);
};