
#include "FileH.h"

bool FileH::Dir_Exists(const char* dir_path)
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



char* FileH::Get_Filepath(const char* directory, char* filename)
{
    // First allocate memory for the concatenated string:
    int memory_size = ( strlen(directory) + strlen("/") + strlen(filename) + 1);
    char* temp = (char*)malloc( sizeof(char)*  memory_size);
    StringH::Erase_Num_Chars(temp, memory_size);
    strcpy(temp, directory);

    // Get last character to determine if '/' was used at end of dir path
    StringH::Trim_WhiteSpace(temp);
    char last;
    for(int i =0; i<memory_size; ++i)
    {
        if(temp[i] != '\0')
            last = temp[i];
        else if( temp[i] == '\0')
            break;
    }

    if(last == '/') // no need to add an extra '/'
    {
        strcat(temp, filename);
    }
    else
    {
        strcat(temp, "/");
        strcat(temp, filename);
    }
    return temp;
}





/*
|--------------------------------------------------------------------------
| List_Files
|--------------------------------------------------------------------------
|
| List the files in the directory given by path. Only "regular" files are
| returned. Folders are not returned
|
| @param:   path    the path to the directory whose files are listed
| @return:          a pointer to a list of strings, each string is a file in the directory
*/
char** FileH::List_Files(const char* path)
{
  int num_files = 0;
  unsigned char filetype;
  struct dirent *directory_entry;   // Pointer for directory entry
  DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.

  if (directory == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open directory: %s\n", path);
      return NULL;
  }

  printf(LINE_SEPARATOR);
  printf("%s\n", path);
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
            printf("\t%s\n", directory_entry->d_name);
            ++num_files;
      }
  }
  printf(LINE_SEPARATOR);

  free(directory);

  char** file_list = (char**)malloc( sizeof(char*)*(num_files +1) );
  for(int i = 0; i<num_files+1; ++i)
    file_list[i] = NULL;

  int file_index = 0;
  directory = opendir(path);   // opendir() returns a pointer of DIR type.
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
          char* name = (char*)malloc( sizeof(char) * (strlen(directory_entry->d_name)+1));
          strcpy(name, directory_entry->d_name);
          name[strlen(directory_entry->d_name)] = '\0';
          file_list[file_index] = name;
          file_list[file_index+1] = NULL;
          ++file_index;
      }
  }

  free(directory);
  return file_list;
}


/*
|--------------------------------------------------------------------------
| List_Folders
|--------------------------------------------------------------------------
|
| List all regular folders in the directory given in the argument path.
| Hidden folders are not included. (folders beginning with a '.')
|
| @param:     path         filepath to directory whose folders are listed
| @return:                 a pointer to a list of strings, each string is a folder in the directory
*/
void FileH::List_Folders(const char* path)
{
  unsigned char filetype;
  struct dirent *directory_entry;   // Pointer for directory entry
  DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.

  if (directory == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open directory: %s\n", path);
      return;
  }

  printf("\nLIST FOLDERS:\n");
  printf("------------------------------------------------------------------------------\n");
  printf("ls %s\n", path);
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_DIR)
      {
          if(directory_entry->d_name[0] != '.' && directory_entry->d_name[1] != '.')
            printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, directory_entry->d_name);
      }
  }
  printf("------------------------------------------------------------------------------\n");

  closedir(directory);
  return;
}



FILE* FileH::Create_New_File(const char* filepath)
{
    FILE* data_file_ptr;

    // File doesnt exist yet, must be created
    data_file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( data_file_ptr == NULL )
    {
        printf("creation error on new data file:   %s\n", filepath);
        perror("open new data file error");
        return NULL;
    }

    return data_file_ptr;
}

bool FileH::File_Exists(const char* filepath)
{
    FILE *file;
    if ((file = fopen(filepath, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


FILE* FileH::Open_Existing_File(const char* filepath)
{
    FILE* data_file_ptr;
    if( FileH::File_Exists(filepath) )
    {
        data_file_ptr = fopen(filepath, "r"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(data_file_ptr == NULL)
        {
            printf("error opening File:  %s\n", filepath);
            perror("opening existing File");
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
    return data_file_ptr;
}


void FileH::Print_File(const char* filepath)
{
    FILE* file_ptr;
    char readline[256];

    // File doesnt exist yet, must be created
    file_ptr = fopen(filepath, "r"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( file_ptr == NULL )
    {
        printf("read error on file:   %s\n", filepath);
        perror("read file error");
        return;
    }
    printf("\n-----------------%s----------------------------------\n", filepath);
    while( fgets(readline, 256, file_ptr) != NULL )
    {
        printf("%s", readline);
    }
    printf("\n-------------------------------------------------------------------\n");
}

long FileH::Get_File_Size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void FileH::Close_File(FILE* finished_file)
{
    fflush(finished_file);
    fclose(finished_file);
}


// this function takes as an argument something of the form: /home/pi/test/dir1/file1.txt
// and returns a string of the filename: file1.txt
// if an argument has no file extension (no .), then the text after the last path delimiter is returned
// if there are no path delimiters in the argument, the return val is the argument
// if the argument is something of the form: /home/pi/test/dir1/ then NULL is returned
// client is responsible for freeing memory used in return string
// this function only does string processing, it does not verify the actual file or path exist
char* FileH::Extract_Filename(const char* filepath)
{
    if(filepath == NULL || !strlen(filepath)>0)
        return NULL;

    char path_copy[strlen(filepath)+1];
    char* filename = NULL;
    strcpy(path_copy, filepath); 
    StringH::Trim_WhiteSpace(path_copy); // remove leading and trailing whitespace
    
    int index_last_path_delim = 0;  // index of last forward slash in the filepath, start at front of string

    // find index of last path delimiter
    for(int i=0; i<strlen(path_copy); ++ i) 
    {
        if( path_copy[i] == '/')
        {
            index_last_path_delim = i;
            continue;
        }
    }

    if(path_copy[index_last_path_delim+1] == '\0')   // if the function argument is a directory, not a file: /home/pi/test/dir1/
    {
        return NULL;
    }
    else if(index_last_path_delim == 0 && path_copy[0] != '/')  // if there are no path separators in the argument, the string is the file
    {
        return strdup(path_copy);
    }
    else
    {
        filename = StringH::Get_Substring(path_copy, index_last_path_delim+1, strlen(path_copy)-1);
        //printf("filename: %s\n", filename);
    }
    return filename;
}

// this function takes as an argument something of the form: /home/pi/test/dir1/dir2  OR /home/pi/test/dir1/document.txt
// and returns a string of the parent directory: /home/pi/test/dir1
// if an argument is of the form /home/pi/test/dir1/dir2/, it is assumed the user wants the parent directory when inside dir2, which would be /home/pi/test/dir1/dir2
// if there are no path delimiters in the argument, NULL is returned
// client is responsible for freeing memory used in return string
char* FileH::Extract_Parent_Dir(const char* filepath)
{
    if(filepath == NULL || !strlen(filepath)>0)
        return NULL;

    char path_copy[strlen(filepath)+1];
    strcpy(path_copy, filepath); 
    StringH::Trim_WhiteSpace(path_copy); // remove leading and trailing whitespace
    
    int index_last_path_delim = 0;  // index of last forward slash in the filepath, start at front of string

    // find index of last path delimiter
    for(int i=0; i<strlen(path_copy); ++ i) 
    {
        if( path_copy[i] == '/')
        {
            index_last_path_delim = i;
            continue;
        }
    }

    if(index_last_path_delim == 0 && path_copy[0] == '/')   // if the function argument is "/" or "/somedir", then return "/"
    {
        path_copy[1] = '\0';    // terminate the string after the slash and return the string
        return strdup(path_copy);
    }
    else if(index_last_path_delim == 0 && path_copy[0] != '/')  // if there are no path separators in the argument
    {
        return NULL;
    }
    else
    {
        for(int i=index_last_path_delim; i<strlen(path_copy); ++ i) 
        {
            path_copy[i] = '\0';   // erase any remaining chars after the last path separator
        }
    }
    return strdup(path_copy);
}