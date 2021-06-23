
#ifndef WRITE_LOG
#define WRITE_LOG

#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
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

#include "colors.h"
#include "StringH.h"
#include "Data.h"
#include "Cal_Parameters.h"

#define BUF_LEN   400
#define ADDR_LEN 20
#define FILEPATH_LEN 200
#define FILENAME_LEN 60
#define DIR_LEN 100
#define NAME_LINE_LEN 150
#define TIMEDATE_LINE_LEN 100
#define TIMESTAMP_LINE_LEN 50
#define DATE_HEADER_LEN 15
#define WRITE_LINE_LEN 200


static const char* line_hbreak = "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";



// Data Structure class to hold measurements that are read from Pi or meters
class Write_Log
{
public:
        Write_Log(const char* log_name, const char* comment)    // argument is Ethernet MAC address of the PI we are calibrating
        {
            this->log_file_ptr = NULL;
            StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
            StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);
            StringH::Erase_Num_Chars(this->name, FILENAME_LEN);
            StringH::Erase_Num_Chars(this->header_comment, 30);

            strncpy(this->header_comment, comment, 20);

            char* date = Get_Date();

            char str_hour[20];
            time_t rawtime;
            struct tm* timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            str_hour[0] = '\0';
            strftime(str_hour,25,"%I%p",timeinfo);
            // printf("hour time: %s\n", str_hour);
            str_hour[strlen(str_hour)] = '\0';

            for(int i=0; i<strlen(str_hour); ++i)
            {
                char ch = str_hour[i];
                if(ch == ':')
                    str_hour[i] = '.';
            }

            strcpy(this->Log_Filename, date);
            strcat(this->Log_Filename, "_");
            strcat(this->Log_Filename, str_hour);
            strcat(this->Log_Filename, "_");
            strcat(this->Log_Filename, log_name);
            strcat(this->Log_Filename, ".csv");
            free(date);

            // create LOG dir filepath
            StringH::Erase_Num_Chars(this->Log_Dir, DIR_LEN);
            strcpy(this->Log_Dir, CAL_LOG_DIR);             //  CAL_LOG_DIR is defined in Cal_Paramters.cpp
            this->Log_Dir[ strlen(this->Log_Dir) ] = '\0';

            if( !this->Dir_Exists( this->Log_Dir) )         // if the directory doesnt exist already, create it
            {
                mkdir( this->Log_Dir, 0700);
            }
            sprintf(this->Log_Filepath, "%s/%s", Log_Dir, this->Log_Filename);
            printf("\tSave LOCATION:  " COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, this->Log_Filepath);
        }

        static char* Get_Date();
        static char* Get_Time();
        static FILE* Open_File(char* filepath);
        static void Close_File(FILE* finished_file);
        static bool File_Exists(const char* filename);
        static bool Dir_Exists(char* dir_path);
        void Create_DIR(char* dir_path);

        static FILE* Initialize_Log_File(char* filepath, char* filename);
        FILE* Initialize_Log_File(char* filepath, char* filename, char* file_comment, char* label1, char* label2, char* units1, char* units2);
        FILE* Initialize_Log_File(char* filepath, char* filename, char* file_comment);
        void Write_Data(Data_Group* meter_V);
        void Write_Data(Data_Group* meter1_vals, char* meter1_units, Data_Group* meter2_vals, char* meter2_units);

        void Advance_to_Index(char* write_buffer, int* index_ptr, int target_index);
        void Write_Chars(char* write_buffer, int* column_index, char* print_buffer, int MAX_CHARS);
        void Clear_Print_Buffer(char* buffer);
        void Write_Units(const char* label1, const char* label2);


        char name[FILENAME_LEN];

private:
        FILE* log_file_ptr;                 //  file pointer for log file that is being written
        char Log_Filepath[FILEPATH_LEN];    // temporary variable, contains filepath of file being written. EX: /home/pi/CAL_LOGS/B8.27.EB.87.3D.76/COOL_MA_DATA.log
        char Log_Dir[DIR_LEN];              // unchanging once assigned by constructor. EX:  /home/pi/CAL_LOGS/B8.27.EB.87.3D.76/
        char Log_Filename[FILENAME_LEN];   // temp variable, contains filename (just the name, not path) EX: DIODE_V_DATA.log
        char header_comment[30];

};

#endif
