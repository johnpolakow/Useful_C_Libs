
#include "Write_Log.h"

/*
const char* LOAD_MA_FILE = "LOAD_mA_DATA.log";
const char* LOAD_V_FILE  = "LOAD_V_DATA.log";
const char* DIODE_V_FILE = "DIODE_V_DATA.log";
const char* REF100_FILE  = "REF100_DATA.log";
const char* COOLER_V_FILE  = "COOLER_V_DATA.log";
const char* COOLER_MA_FILE = "COOLER_mA_DATA.log";
*/


char* Write_Log::Get_Date()
{
    char* buffer = (char*)malloc(sizeof(char) * 22);
	 time_t rawtime;
	 struct tm* timeinfo;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
	 strftime (buffer,30,"%m-%d-%y",timeinfo);

	 return buffer;
}

char* Write_Log::Get_Time()
{
   char* buffer = (char*)malloc(sizeof(char) * 25);
	 time_t rawtime;
	 struct tm* timeinfo;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
     buffer[0] = '\0';
	 strftime(buffer,25,"%T",timeinfo);
	 buffer[strlen(buffer)] = '\0';

	 return buffer;
}


bool Write_Log::File_Exists(const char* filename)
{
    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


bool Write_Log::Dir_Exists(char* dir_path)
{
    DIR* dir = opendir(dir_path);
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else if (ENOENT == errno)
    {
        return false;
    }
    else
    {
        return false;
    }
}


FILE* Write_Log::Open_File(char* filepath)
{
    FILE* log_file_ptr;
    if( Write_Log::File_Exists(filepath) )
    {
        log_file_ptr = fopen(filepath, "a"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(log_file_ptr == NULL)
        {
            printf("error opening Log File:  %s\n", filepath);
            perror("opening existing Log File");
            return NULL;
        }
    }
    else
    {
        // File doesnt exist yet, must be created
        log_file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if( log_file_ptr == NULL )
        {
            printf("creation error on new Log file:   %s\n", filepath);
            perror("open new Log error");
            return NULL;
        }
    }
    return log_file_ptr;
}


void Write_Log::Create_DIR(char* dir_path)
{
    if( !Write_Log::Dir_Exists( dir_path) )
    {
        mkdir( dir_path, 0700);  // 0700 is the linux permissions for the directory
    }
    return;
}


FILE* Write_Log::Initialize_Log_File(char* filepath, char* filename, char* file_comment, char* label1, char* label2, char* units1, char* units2)
{
    // Assuming File doesnt exist yet, must be created
    FILE* file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( file_ptr == NULL )
    {
        printf("creation error on new file:   %s\n", filepath);
        perror("open new Log error");
        return NULL;
    }

    char Time_Date_Line[TIMEDATE_LINE_LEN];
    char Declaration_Line[FILENAME_LEN];
    StringH::Erase_Num_Chars(Declaration_Line, FILENAME_LEN);
    StringH::Erase_Num_Chars(Time_Date_Line, TIMEDATE_LINE_LEN);

    char* date = Get_Date();
    char* time = Get_Time();
    sprintf( Time_Date_Line, "// Created:   %s %s", time, date);
    sprintf( Declaration_Line, "\t\t## %s ##\n", filename);
    free(date);
    free(time);

    fprintf(file_ptr , "%s\n\n", Time_Date_Line);                   // filename: XXXXXXX.log
    fprintf(file_ptr , "\t\t%s\n", Declaration_Line );            // write ""## LOAD mA CAL LOG ## "
    fprintf(file_ptr , "\t\t%s\n\n", file_comment );            // write ""## LOAD mA CAL LOG ## "
    fprintf(file_ptr , "\t\t%s\t%s\n", label1, label2 );            // write ""## LOAD mA CAL LOG ## "
    fprintf(file_ptr , "%s", line_hbreak);                     // write separation line: ______________
    fprintf(file_ptr , "\t\t %s\t\t%s\n", units1, units2);                     // write separation line: ______________

    return file_ptr;
}


FILE* Write_Log::Initialize_Log_File(char* filepath, char* filename)
{
    // Assuming File doesnt exist yet, must be created
    FILE* file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( file_ptr == NULL )
    {
        printf("creation error on new file:   %s\n", filepath);
        perror("open new Log error");
        return NULL;
    }

    char Time_Date_Line[TIMEDATE_LINE_LEN];
    char Declaration_Line[FILENAME_LEN];
    StringH::Erase_Num_Chars(Declaration_Line, FILENAME_LEN);
    StringH::Erase_Num_Chars(Time_Date_Line, TIMEDATE_LINE_LEN);

    char* date = Get_Date();
    char* time = Get_Time();
    sprintf( Time_Date_Line, "// Created:   %s %s", time, date);
    sprintf( Declaration_Line, "\t\t   ## %s ##\n", filename);
    free(date);
    free(time);

    fprintf(file_ptr , "%s\n\n", Time_Date_Line);                   // filename: XXXXXXX.log
    fprintf(file_ptr , "\t\t%s\n\n", Declaration_Line );            // write ""## LOAD mA CAL LOG ## "
    fprintf(file_ptr , "%s\n", line_hbreak);                     // write separation line: ______________

    return file_ptr;
}


void Write_Log::Write_Data(Data_Group* meter_vals)
{
    if( File_Exists(this->Log_Filepath) )
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
    }
    else
    {
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename);            // File doesnt exist yet, must be created
    }

    char* time = Get_Time();
    char* date = Get_Date();
    // write the current timestamp to file
    fprintf(log_file_ptr , "%s %s", time, date);
    free(time);
    free(date);

    fprintf(log_file_ptr , "\t[ %s %s]", meter_vals-> description, meter_vals->units);


    char write_line[100];
    StringH::Erase_Num_Chars(write_line, 100);
    if(meter_vals->num_points == 1)
    {
        sprintf(write_line, "\t %.6f ", meter_vals->data_points[0] );
        fprintf(log_file_ptr , "%s\n", write_line);
    }
    else
    {
        int i = 0;
        while( i< (meter_vals->num_points) )
        {
            StringH::Erase_Num_Chars(write_line, 100);
            sprintf(write_line, "\t[ %.6f ]", meter_vals->data_points[i] );
            fprintf(log_file_ptr , "%s\n", write_line);
            ++i;
        }
    }
    //fprintf(log_file_ptr, "---------------------------------------------------------------------------\n\n");      
    Write_Log::Close_File(log_file_ptr);
}



void Write_Log::Write_Units(const char* units1, const char* units2)
{
    if( File_Exists(this->Log_Filepath) )
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
        fprintf(log_file_ptr , "\t\t %s\t\t%s\n", units1, units2);
        Write_Log::Close_File(log_file_ptr);
    }
    else
    {
        return;
    }  
}


void Write_Log::Write_Data(Data_Group* meter1_vals, char* meter1_units, Data_Group* meter2_vals, char* meter2_units)
{
    if( File_Exists(this->Log_Filepath) )
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
    }
    else
    {
        char* description1 = meter1_vals->description;
        char* description2 = meter2_vals->description;
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->header_comment, description1, description2, meter1_units, meter2_units);            // File doesnt exist yet, must be created
    }

    char* time = Get_Time();
    char* date = Get_Date();
    // write the current timestamp to file
    fprintf(log_file_ptr , "%s", time);
    free(time);
    free(date);

    //fprintf(log_file_ptr , "\t[%s_%s,%s_%s]", meter1_vals-> description, meter1_vals->units, meter2_vals-> description, meter2_vals->units);


    char write_line[100];
    StringH::Erase_Num_Chars(write_line, 100);
    if(meter1_vals->num_points == 1)
    {
        sprintf(write_line, "\t %.4f \t %.4f ", meter1_vals->data_points[0], meter2_vals->data_points[0] );
        fprintf(log_file_ptr , "%s\n", write_line);
    }
    else
    {
        int i = 0;
        while( i< (meter1_vals->num_points) )
        {
            StringH::Erase_Num_Chars(write_line, 100);
            sprintf(write_line, "\t %.6f,  %.6f ", meter1_vals->data_points[i], meter2_vals->data_points[i] );
            fprintf(log_file_ptr , "%s\n", write_line);
            ++i;
        }
        fprintf(log_file_ptr, "---------------------------------------------------------------------------\n\n"); 
    }
     
    Write_Log::Close_File(log_file_ptr);
}


void Write_Log::Close_File(FILE* finished_file)
{
    fflush(finished_file);
    fclose(finished_file);
}
