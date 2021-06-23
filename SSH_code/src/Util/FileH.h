

#ifndef FILE_H
#define FILE_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "StringH.h"
#include "colors.h"


// Data Structure class to hold measurements read from Pi or meters
class FileH
{
public:
        FileH(){}
        ~FileH(){}

        static bool File_Exists(const char* filepath);
        static FILE* Open_Existing_File(const char* filepath);
        static FILE* Create_New_File(const char* filepath);
        static void Close_File(FILE* finished_file);
        static bool Dir_Exists(const char* dir_path);
        static char* Extract_Filename(const char* filepath);
        static char* Extract_Parent_Dir(const char* filepath);
        static void Print_File(const char* filepath);
        static char* Get_Filepath(const char* directory, char* filename);
        static long Get_File_Size(std::string filename);

        static void List_Folders(const char* path);
        static char** List_Files(const char* path);

        



};

#endif
