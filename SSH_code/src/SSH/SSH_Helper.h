
#include <libssh/libssh.h>
#include <iterator>
#include <iostream>
#include <fstream>

#include "FileH.h"
#include "StringH.h"




#define POPEN_READ                   "re"  // "r" opens for reading, "e" sets close on exec flag ("FD_CLOEXEC")
#define POPEN_WRITE                   "w"  // "r" opens for reading, "e" sets close on exec flag ("FD_CLOEXEC")

class SSH_Helper
{
public:
    SSH_Helper(){}
    ~SSH_Helper(){}

    static bool Have_Local_SSH_Keys_Generated();
    static void Read_Local_SSH_Keys(char* key_contents, char* local_filepath);
    static void Create_Remote_SSH_DIR();
    static bool Pi_Has_SSH_DIR(ssh_session session);
    static bool Pi_Has_Authorized_Keys_File(ssh_session session);
    static int SCP_Read_Remote_File(ssh_session session, const char* remote_filepath, char* file_contents);
    static int Append_SSH_Key_To_Remote_Host(ssh_session session, char* key);


    // these UNIX file permission bits are available for the mode argument (default is read and write for owner)
    //  S_IRUSR     read permission for the owner
    //  S_IWUSR     write permission for the owner
    //  S_IXUSR     execute permission for the owner of the file
    //  S_IROTH     read permission for other users
    //  S_IWOTH     write permission for other users
    //  S_IXOTH     execute permission for other users
    static int SCP_Write_Remote_File(ssh_session session, const char* local_filepath, const char* remote_dir, int mode = S_IRUSR |  S_IWUSR);
    static int SCP_Create_Remote_Dir(ssh_session session, const char* remote_dir);

    static char** Exec_Shell_Command(char* command);
    static void Free_String_Array_Memory(char** string_array);

    static int show_remote_processes(ssh_session session);
    static int verify_knownhost(ssh_session session);
};

